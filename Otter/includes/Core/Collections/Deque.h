#ifndef OTTERENGINE_DEQUE_H
#define OTTERENGINE_DEQUE_H

#include "Core/Memory.h"
#include "Core/Collections/Iterators/LinearIterator.h"

#if !OTR_RUNTIME
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"
#endif

namespace Otter
{
    template<typename T>
    class Deque final
    {
        using Iterator = LinearIterator<T>;
        using ConstIterator = LinearIterator<const T>;

    public:
        Deque() = default;
        ~Deque()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);
        }

        Deque(InitialiserList<T> list)
            : Deque()
        {
            m_Capacity = list.size();
            m_Data     = Buffer::New<T>(m_Capacity);

            m_Count = 0;
            for (const T& item: list)
                m_Data[m_Count++] = item;
        }

        Deque(const Deque<T>& other)
            : Deque()
        {
            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Data     = other.m_Data;
        }

        Deque(Deque<T>&& other) noexcept
            : Deque()
        {
            m_Capacity = std::move(other.m_Capacity);
            m_Count    = std::move(other.m_Count);
            m_Data     = std::move(other.m_Data);

            other.m_Capacity = 0;
            other.m_Count    = 0;
            other.m_Data     = nullptr;
        }

        Deque<T>& operator=(const Deque<T>& other)
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Data     = other.m_Data;

            return *this;
        }

        Deque<T>& operator=(Deque<T>&& other) noexcept
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Capacity = std::move(other.m_Capacity);
            m_Count    = std::move(other.m_Count);
            m_Data     = std::move(other.m_Data);

            other.m_Capacity = 0;
            other.m_Count    = 0;
            other.m_Data     = nullptr;

            return *this;
        }

        void PushFront(const T& value)
        {
            if (m_Count >= m_Capacity)
                Expand();

            for (UInt64 i = m_Count; i > 0; i--)
                m_Data[i] = m_Data[i - 1];

            m_Data[0] = value;
            m_Count++;
        }

        void PushFront(T&& value)
        {
            if (m_Count >= m_Capacity)
                Expand();

            for (UInt64 i = m_Count; i > 0; i--)
                m_Data[i] = m_Data[i - 1];

            m_Data[0] = std::move(value);
            m_Count++;
        }

        void PushBack(const T& value)
        {
            if (m_Count >= m_Capacity)
                Expand();

            m_Data[m_Count++] = value;
        }

        void PushBack(T&& value)
        {
            if (m_Count >= m_Capacity)
                Expand();

            m_Data[m_Count++] = std::move(value);
        }

        bool TryPopFront()
        {
            if (m_Count == 0)
                return false;

            for (UInt64 i = 0; i < m_Count - 1; i++)
                m_Data[i] = m_Data[i + 1];

            m_Count--;

            return true;
        }

        bool TryPopBack()
        {
            if (m_Count == 0)
                return false;

            m_Count--;

            return true;
        }

        [[nodiscard]] bool TryPeekFront(T* outItem)
        {
            if (m_Count == 0)
                return false;

            *outItem = m_Data[0];

            return true;
        }

        [[nodiscard]] bool TryPeekBack(T* outItem)
        {
            if (m_Count == 0)
                return false;

            *outItem = m_Data[m_Count - 1];

            return true;
        }

        bool TryRemove(const T& item)
        {
            for (UInt64 i = 0; i < m_Count; i++)
                if (m_Data[i] == item)
                    return TryRemoveAt(i);

            return false;
        }

        bool TryRemove(T&& item) noexcept
        {
            for (UInt64 i = 0; i < m_Count; i++)
                if (m_Data[i] == item)
                    return TryRemoveAt(i);

            return false;
        }

        bool TryRemoveAt(const UInt64 index)
        {
            if (index >= m_Count)
                return false;

            for (UInt64 i = index; i < m_Count - 1; i++)
                m_Data[i] = m_Data[i + 1];

            m_Count--;
            return true;
        }

        void Reserve(const UInt64 capacity)
        {
            if (capacity <= m_Capacity)
                return;

            T* newData = Buffer::New<T>(capacity);

            for (UInt64 i = 0; i < m_Count; i++)
                newData[i] = m_Data[i];

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data     = newData;
            m_Capacity = capacity;
        }

        void Expand(const UInt64 amount = 0)
        {
            if (amount == 0)
                m_Capacity = m_Capacity == 0 ? 2 : m_Capacity * 1.5;
            else
                m_Capacity += amount;

            T* newData = Buffer::New<T>(m_Capacity);

            for (UInt64 i = 0; i < m_Count; i++)
                newData[i] = m_Data[i];

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data = newData;
        }

        OTR_INLINE void Clear() { m_Count = 0; }

        void ClearDestructive()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data     = nullptr;
            m_Capacity = 0;
            m_Count    = 0;
        }

#if !OTR_RUNTIME
        ReadOnlySpan<MemoryFootprint, 1> GetMemoryFootprint(const char* const debugName) const
        {
            MemoryFootprint footprint = { };
            MemorySystem::CheckMemoryFootprint([&]()
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

        [[nodiscard]] OTR_INLINE const T* GetData() const { return m_Data; }
        [[nodiscard]] OTR_INLINE UInt64 GetCapacity() const { return m_Capacity; }
        [[nodiscard]] OTR_INLINE UInt64 GetCount() const { return m_Count; }
        [[nodiscard]] OTR_INLINE bool IsCreated() const { return m_Data && m_Capacity > 0; }
        [[nodiscard]] OTR_INLINE bool IsEmpty() const { return m_Count == 0; }

        OTR_INLINE Iterator begin() noexcept { return Iterator(m_Data); }
        OTR_INLINE Iterator end() noexcept { return Iterator(m_Data + m_Count); }
        OTR_INLINE Iterator rbegin() noexcept { return Iterator(m_Data + m_Count - 1); }
        OTR_INLINE Iterator rend() noexcept { return Iterator(m_Data - 1); }

        OTR_INLINE ConstIterator begin() const noexcept { return ConstIterator(m_Data); }
        OTR_INLINE ConstIterator end() const noexcept { return ConstIterator(m_Data + m_Count); }
        OTR_INLINE ConstIterator rbegin() const noexcept { return ConstIterator(m_Data + m_Count - 1); }
        OTR_INLINE ConstIterator rend() const noexcept { return ConstIterator(m_Data - 1); }

    private:
        T* m_Data = nullptr;
        UInt64 m_Capacity = 0;
        UInt64 m_Count    = 0;
    };
}

#endif //OTTERENGINE_DEQUE_H
