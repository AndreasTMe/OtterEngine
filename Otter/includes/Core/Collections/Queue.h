#ifndef OTTERENGINE_QUEUE_H
#define OTTERENGINE_QUEUE_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Memory.h"

#if !OTR_RUNTIME
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"
#endif

namespace Otter
{
    template<typename T>
    class Queue final
    {
    public:
        Queue()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);
        }

        ~Queue()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);
        }

        Queue(InitialiserList<T> list)
            : Queue()
        {
            m_Capacity   = list.size();
            m_Data       = Buffer::New<T>(m_Capacity);
            m_Count      = 0;
            m_StartIndex = 0;

            for (const T& item: list)
                m_Data[m_Count++] = item;
        }

        Queue(const Queue<T>& other)
            : Queue()
        {
            m_Data       = other.m_Data;
            m_Capacity   = other.m_Capacity;
            m_Count      = other.m_Count;
            m_StartIndex = other.m_StartIndex;
        }

        Queue(Queue<T>&& other) noexcept
            : Queue()
        {
            m_Data       = std::move(other.m_Data);
            m_Capacity   = std::move(other.m_Capacity);
            m_Count      = std::move(other.m_Count);
            m_StartIndex = std::move(other.m_StartIndex);

            other.m_Data       = nullptr;
            other.m_Capacity   = 0;
            other.m_Count      = 0;
            other.m_StartIndex = 0;
        }

        Queue<T>& operator=(const Queue<T>& other)
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data       = other.m_Data;
            m_Capacity   = other.m_Capacity;
            m_Count      = other.m_Count;
            m_StartIndex = other.m_StartIndex;

            return *this;
        }

        Queue<T>& operator=(Queue<T>&& other) noexcept
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data       = std::move(other.m_Data);
            m_Capacity   = std::move(other.m_Capacity);
            m_Count      = std::move(other.m_Count);
            m_StartIndex = std::move(other.m_StartIndex);

            other.m_Data       = nullptr;
            other.m_Capacity   = 0;
            other.m_Count      = 0;
            other.m_StartIndex = 0;

            return *this;
        }

        bool TryEnqueue(const T& item)
        {
            if (m_Count >= m_Capacity)
                Expand();

            auto endIndex = m_StartIndex + m_Count;

            if (endIndex >= m_Capacity)
                endIndex -= m_Capacity;

            m_Data[endIndex] = item;

            m_Count++;

            return true;
        }

        bool TryEnqueue(T&& item) noexcept
        {
            if (m_Count >= m_Capacity)
                Expand();

            auto endIndex = m_StartIndex + m_Count;

            if (endIndex >= m_Capacity)
                endIndex -= m_Capacity;

            m_Data[endIndex] = std::move(item);

            m_Count++;

            return true;
        }

        bool TryDequeue()
        {
            if (IsEmpty())
                return false;

            m_StartIndex++;

            if (m_StartIndex >= m_Capacity)
                m_StartIndex = 0;

            m_Count--;

            return true;
        }

        bool TryDequeue(T& item)
        {
            if (IsEmpty())
                return false;

            item = m_Data[m_StartIndex];
            m_StartIndex++;

            if (m_StartIndex >= m_Capacity)
                m_StartIndex = 0;

            m_Count--;

            return true;
        }

        bool TryPeek(T& item)
        {
            if (IsEmpty())
                return false;

            item = m_Data[m_StartIndex];

            return true;
        }

        void Reserve(const UInt64 capacity)
        {
            RecreateEmpty(capacity);
        }

        void Expand(const UInt64 amount = 0)
        {
            UInt64 newCapacity = CalculateExpandCapacity(amount);

            if (IsEmpty())
            {
                RecreateEmpty(newCapacity);
                return;
            }

            T* newData = Buffer::New<T>(newCapacity);

            if (m_StartIndex + m_Count < m_Capacity)
            {
                for (UInt64 i = 0; i < m_Count; i++)
                    newData[i] = m_Data[i + m_StartIndex];
            }
            else
            {
                const auto endIndex = m_StartIndex + m_Count - m_Capacity;

                for (UInt64 i = 0; i < m_Capacity - m_StartIndex; i++)
                    newData[i] = m_Data[i + m_StartIndex];

                for (UInt64 i = 0; i < endIndex; i++)
                    newData[i + m_Capacity - m_StartIndex] = m_Data[i];
            }

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data       = newData;
            m_Capacity   = newCapacity;
            m_StartIndex = 0;
        }

        void Shrink(const UInt64 amount = 0, const bool isDestructive = false)
        {
            UInt64 newCapacity = CalculateShrinkCapacity(amount, isDestructive);

            if (IsEmpty() || newCapacity == 0)
            {
                RecreateEmpty(newCapacity);
                return;
            }

            T* newData = Buffer::New<T>(newCapacity);

            if (m_StartIndex + m_Count < m_Capacity)
            {
                for (UInt64 i = 0; i < m_Count && i < newCapacity; i++)
                    newData[i] = m_Data[i + m_StartIndex];
            }
            else
            {
                const auto endIndex = m_StartIndex + m_Count - m_Capacity;

                for (UInt64 i = 0; i < m_Capacity - m_StartIndex && i < newCapacity; i++)
                    newData[i] = m_Data[i + m_StartIndex];

                for (UInt64 i = 0; i < endIndex && i < newCapacity; i++)
                    newData[i + m_Capacity - m_StartIndex] = m_Data[i];
            }

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data       = newData;
            m_Capacity   = newCapacity;
            m_Count      = m_Count < newCapacity ? m_Count : newCapacity;
            m_StartIndex = 0;
        }

        bool Contains(const T& item) const
        {
            if (m_StartIndex + m_Count <= m_Capacity)
            {
                for (UInt64 i = m_StartIndex; i < m_Count; i++)
                    if (m_Data[i] == item)
                        return true;
            }
            else
            {
                const auto endIndex = m_StartIndex + m_Count - m_Capacity;

                for (UInt64 i = m_StartIndex; i < m_Capacity; i++)
                    if (m_Data[i] == item)
                        return true;

                for (UInt64 i = 0; i <= endIndex; i++)
                    if (m_Data[i] == item)
                        return true;
            }

            return false;
        }

        bool Contains(T&& item) const noexcept
        {
            if (m_StartIndex + m_Count <= m_Capacity)
            {
                for (UInt64 i = m_StartIndex; i < m_Count; i++)
                    if (m_Data[i] == item)
                        return true;
            }
            else
            {
                const auto endIndex = m_StartIndex + m_Count - m_Capacity;

                for (UInt64 i = m_StartIndex; i < m_Capacity; i++)
                    if (m_Data[i] == item)
                        return true;

                for (UInt64 i = 0; i <= endIndex; i++)
                    if (m_Data[i] == item)
                        return true;
            }

            return false;
        }

        OTR_INLINE void Clear()
        {
            m_StartIndex = 0;
            m_Count      = 0;
        }

        void ClearDestructive()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data       = nullptr;
            m_Capacity   = 0;
            m_Count      = 0;
            m_StartIndex = 0;
        }

#if !OTR_RUNTIME
        ReadOnlySpan<MemoryFootprint, 1> GetMemoryFootprint(const char* const debugName) const
        {
            MemoryFootprint footprint = { };
            OTR_MEMORY_SYSTEM.CheckMemoryFootprint([&]()
                                                   {
                                                       MemoryDebugPair pair[1];
                                                       pair[0] = { debugName, m_Data };

                                                       return MemoryDebugHandle{ pair, 1 };
                                                   },
                                                   &footprint,
                                                   nullptr);

            return ReadOnlySpan<MemoryFootprint, 1>{ footprint };
        }
#endif

        [[nodiscard]] OTR_INLINE constexpr UInt64 GetCapacity() const noexcept { return m_Capacity; }
        [[nodiscard]] OTR_INLINE constexpr UInt64 GetCount() const noexcept { return m_Count; }
        [[nodiscard]] OTR_INLINE constexpr bool IsCreated() const noexcept { return m_Data && m_Capacity > 0; }
        [[nodiscard]] OTR_INLINE constexpr bool IsEmpty() const noexcept { return m_Count == 0; }

    private:
        T* m_Data = nullptr;
        UInt64 m_Capacity   = 0;
        UInt64 m_Count      = 0;
        UInt64 m_StartIndex = 0;

        void MoveIndex(UInt64& index)
        {
            index++;

            if (index > m_Capacity)
                index = 0;
        }

        void RecreateEmpty(const UInt64 capacity)
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data       = capacity > 0 ? Buffer::New<T>(capacity) : nullptr;
            m_Capacity   = capacity;
            m_Count      = 0;
            m_StartIndex = 0;
        }

        UInt64 CalculateExpandCapacity(const UInt64 expandAmount)
        {
            UInt64 newCapacity;

            if (expandAmount == 0)
                newCapacity = m_Capacity == 0 ? 2 : m_Capacity * 1.5;
            else
                newCapacity = m_Capacity + expandAmount;

            return newCapacity;
        }

        UInt64 CalculateShrinkCapacity(const UInt64 shrinkAmount, const bool isDestructive)
        {
            if (m_Capacity == 0)
                return 0;

            UInt64 newCapacity;

            if (shrinkAmount == 0)
                newCapacity = m_Capacity * 0.75;
            else if (shrinkAmount > m_Capacity)
                newCapacity = 0;
            else
                newCapacity = m_Capacity - shrinkAmount;

            if (!isDestructive && newCapacity < m_Count)
                newCapacity = m_Count;

            return newCapacity;
        }
    };
}

#endif //OTTERENGINE_QUEUE_H
