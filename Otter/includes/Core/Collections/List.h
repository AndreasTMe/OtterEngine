#ifndef OTTERENGINE_LIST_H
#define OTTERENGINE_LIST_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Core/Memory.h"

#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    template<typename T>
    struct List final
    {
    public:
        using Iterator = LinearIterator<T>;
        using ConstIterator = LinearIterator<const T>;

        List()
            : m_Data(nullptr), m_Capacity(0), m_Count(0)
        {
        }
        ~List()
        {
            if (m_Data != nullptr && m_Capacity > 0)
                Buffer::Delete(m_Data, m_Capacity);

            m_Data     = nullptr;
            m_Capacity = 0;
            m_Count    = 0;
        }

        OTR_WITH_ITERATOR(Iterator, m_Data, m_Count)
        OTR_WITH_CONST_ITERATOR(ConstIterator, m_Data, m_Count)
        OTR_DISABLE_HEAP_ALLOCATION

        OTR_INLINE explicit List(const T& value)
        {
            m_Capacity = 1;

            m_Data = Buffer::New<T>(m_Capacity);
            m_Data[0] = value;

            m_Count = 1;
        }

        template<typename... Args>
        OTR_INLINE explicit List(const T& value, const Args&& ... args)
        {
            m_Capacity = VariadicArgs<Args...>::GetSize() + 1;
            m_Data     = Buffer::New<T>(m_Capacity);

            UInt64 i = 0;
            m_Data[i++] = value;
            ([&]()
            {
                m_Data[i++] = args;
            }(), ...);

            m_Count = m_Capacity;
        }

        OTR_INLINE List(const List<T>& other)
        {
            if (this == &other)
                return;

            if (m_Data != nullptr && m_Capacity > 0)
                Buffer::Delete(m_Data, m_Capacity);

            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Data     = other.m_Data;
        }

        OTR_INLINE List(List<T>&& other) noexcept
        {
            if (this == &other)
                return;

            if (m_Data != nullptr && m_Capacity > 0)
                Buffer::Delete(m_Data, m_Capacity);

            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Data     = other.m_Data;
        }

        OTR_INLINE List<T>& operator=(const List<T>& other)
        {
            if (this == &other)
                return *this;

            if (m_Data != nullptr && m_Capacity > 0)
                Buffer::Delete(m_Data, m_Capacity);

            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Data     = other.m_Data;

            return *this;
        }

        OTR_INLINE List<T>& operator=(List<T>&& other) noexcept
        {
            if (this == &other)
                return *this;

            if (m_Data != nullptr && m_Capacity > 0)
                Buffer::Delete(m_Data, m_Capacity);

            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Data     = other.m_Data;

            return *this;
        }

        [[nodiscard]] OTR_INLINE T& operator[](UInt64 index)
        {
            OTR_ASSERT_MSG(index < m_Count, "List index out of bounds")
            return m_Data[index];
        }

        [[nodiscard]] OTR_INLINE const T& operator[](UInt64 index) const
        {
            OTR_ASSERT_MSG(index < m_Count, "List index out of bounds")
            return m_Data[index];
        }

        OTR_INLINE void Reserve(UInt64 capacity)
        {
            if (capacity <= m_Capacity)
                return;

            T* newData = Buffer::New<T>(capacity);

            for (UInt64 i = 0; i < m_Count; i++)
                newData[i] = m_Data[i];

            if (m_Count > 0)
                Buffer::Delete(m_Data, m_Count);

            m_Data     = newData;
            m_Capacity = capacity;
        }

        OTR_INLINE void Add(T item)
        {
            if (m_Count >= m_Capacity)
            {
                m_Capacity = m_Capacity == 0 ? 2 : m_Capacity * 1.5;
                T* newData = Buffer::New<T>(m_Capacity);

                for (UInt64 i = 0; i < m_Count; i++)
                    newData[i] = m_Data[i];

                if (m_Count > 0)
                    Buffer::Delete(m_Data, m_Count);

                m_Data = newData;
            }

            m_Data[m_Count++] = item;
        }

        OTR_INLINE bool TryRemove(T item)
        {
            for (UInt64 i = 0; i < m_Count; i++)
                if (m_Data[i] == item)
                    return TryRemoveAt(i);

            return false;
        }

        OTR_INLINE bool TryRemoveAt(UInt64 index)
        {
            if (index >= m_Count)
                return false;

            for (UInt64 i = index; i < m_Count - 1; i++)
                m_Data[i] = m_Data[i + 1];

            m_Count--;
            return true;
        }

        OTR_INLINE bool Contains(T item) const
        {
            for (UInt64 i = 0; i < m_Count; i++)
                if (m_Data[i] == item)
                    return true;

            return false;
        }

        OTR_INLINE void Clear() { m_Count = 0; }

        OTR_INLINE void ClearDestructive()
        {
            if (m_Data != nullptr && m_Capacity > 0)
                Buffer::Delete(m_Data, m_Capacity);

            m_Data     = nullptr;
            m_Capacity = 0;
            m_Count    = 0;
        }

        [[nodiscard]] OTR_INLINE UInt64 GetCapacity() const { return m_Capacity; }
        [[nodiscard]] OTR_INLINE UInt64 GetCount() const { return m_Count; }
        [[nodiscard]] OTR_INLINE bool IsEmpty() const { return m_Count == 0; }

    private:
        T* m_Data;
        UInt64 m_Capacity;
        UInt64 m_Count;
    };
}

#endif //OTTERENGINE_LIST_H
