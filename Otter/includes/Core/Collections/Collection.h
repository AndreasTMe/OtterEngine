#ifndef OTTERENGINE_COLLECTION_H
#define OTTERENGINE_COLLECTION_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Memory.h"

#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    template<typename T>
    class Collection;

    class Collections final
    {
    public:
        template<typename T>
        static void New(const T* const data, const UInt64& count, Collection<T>& outCollection)
        {
            OTR_INTERNAL_ASSERT_MSG(data != nullptr, "Data cannot be null!")
            OTR_INTERNAL_ASSERT_MSG(count > 0, "Count must be greater than 0!")

            if (outCollection.m_Data != nullptr && outCollection.m_Capacity > 0)
                Buffer::Delete(outCollection.m_Data, outCollection.m_Capacity);

            outCollection.m_Data     = Buffer::New<T>(count);
            outCollection.m_Capacity = count;
            outCollection.m_Count    = 0;

            for (UInt64 i = 0; i < count; i++)
            {
                outCollection.m_Data[i] = data[i];
                outCollection.m_Count++;
            }
        }
    };

    template<typename T>
    class Collection
    {
    public:
        using Iterator = LinearIterator<T>;
        using ConstIterator = LinearIterator<const T>;

        ~Collection() = default;

        OTR_WITH_ITERATOR(Iterator, m_Data, m_Count)
        OTR_WITH_CONST_ITERATOR(ConstIterator, m_Data, m_Count)
        OTR_DISABLE_HEAP_ALLOCATION

        void Reserve(const UInt64& capacity)
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

        void Expand()
        {
            m_Capacity = m_Capacity == 0 ? 2 : m_Capacity * 1.5;

            T* newData = Buffer::New<T>(m_Capacity);

            for (UInt64 i = 0; i < m_Count; i++)
                newData[i] = m_Data[i];

            if (m_Count > 0)
                Buffer::Delete(m_Data, m_Count);

            m_Data = newData;
        }

        bool Contains(const T& item) const
        {
            for (UInt64 i = 0; i < m_Count; i++)
                if (m_Data[i] == item)
                    return true;

            return false;
        }

        bool Contains(T&& item) const noexcept
        {
            for (UInt64 i = 0; i < m_Count; i++)
                if (m_Data[i] == item)
                    return true;

            return false;
        }

        bool TryGetIndexOf(const T& item, UInt64& index) const
        {
            for (UInt64 i = 0; i < m_Count; i++)
                if (m_Data[i] == item)
                {
                    index = i;
                    return true;
                }

            return false;
        }

        bool TryGetIndexOf(T&& item, UInt64& index) const noexcept
        {
            for (UInt64 i = 0; i < m_Count; i++)
                if (m_Data[i] == item)
                {
                    index = i;
                    return true;
                }

            return false;
        }

        OTR_INLINE void Clear() { m_Count = 0; }

        void ClearDestructive()
        {
            if (m_Data != nullptr && m_Capacity > 0)
                Buffer::Delete(m_Data, m_Capacity);

            m_Data     = nullptr;
            m_Capacity = 0;
            m_Count    = 0;
        }

        [[nodiscard]] OTR_INLINE constexpr UInt64 GetCapacity() const { return m_Capacity; }
        [[nodiscard]] OTR_INLINE constexpr UInt64 GetCount() const { return m_Count; }
        [[nodiscard]] OTR_INLINE constexpr bool IsEmpty() const { return m_Count == 0; }

        [[nodiscard]] OTR_INLINE constexpr T* GetData() const { return m_Data; }

    protected:
        Collection()
            : m_Data(nullptr), m_Capacity(0), m_Count(0)
        {
        }

        T* m_Data;
        UInt64 m_Capacity;
        UInt64 m_Count;

        friend class Collections;
    };
}

#define OTR_COLLECTION_CHILD(Type) Type

#define OTR_COLLECTION_CONSTRUCT(Type)                                          \
    OTR_COLLECTION_CHILD(Type)() : Collection<T>() { }                          \
    ~OTR_COLLECTION_CHILD(Type)()                                               \
    {                                                                           \
        if (base::m_Data != nullptr && base::m_Capacity > 0)                    \
            Buffer::Delete(base::m_Data, base::m_Capacity);                     \
                                                                                \
        base::m_Data     = nullptr;                                             \
        base::m_Capacity = 0;                                                   \
        base::m_Count    = 0;                                                   \
    }                                                                           \
                                                                                \
    OTR_COLLECTION_CHILD(Type)(InitialiserList<T> list)                         \
    {                                                                           \
        base::m_Capacity = list.size();                                         \
        base::m_Data     = Buffer::New<T>(base::m_Capacity);                    \
                                                                                \
        base::m_Count = 0;                                                      \
        for (const T& item: list)                                               \
            base::m_Data[base::m_Count++] = item;                               \
    }

#define OTR_COLLECTION_COPY(Type)                                               \
    explicit OTR_COLLECTION_CHILD(Type)(const Collection<T>& other)             \
    {                                                                           \
        if (this == &other)                                                     \
            return;                                                             \
                                                                                \
        if (base::m_Data != nullptr && base::m_Capacity > 0)                    \
            Buffer::Delete(base::m_Data, base::m_Capacity);                     \
                                                                                \
        base::m_Capacity = other.m_Capacity;                                    \
        base::m_Count    = other.m_Count;                                       \
        base::m_Data     = other.m_Data;                                        \
    }                                                                           \
                                                                                \
    OTR_COLLECTION_CHILD(Type)<T>& operator=(const Collection<T>& other)        \
    {                                                                           \
        if (this == &other)                                                     \
            return *this;                                                       \
                                                                                \
        if (base::m_Data != nullptr && base::m_Capacity > 0)                    \
            Buffer::Delete(base::m_Data, base::m_Capacity);                     \
                                                                                \
        base::m_Capacity = other.m_Capacity;                                    \
        base::m_Count    = other.m_Count;                                       \
        base::m_Data     = other.m_Data;                                        \
                                                                                \
        return *this;                                                           \
    }

#define OTR_COLLECTION_MOVE(Type)                                               \
    explicit OTR_COLLECTION_CHILD(Type)(Collection<T>&& other) noexcept         \
    {                                                                           \
        if (this == &other)                                                     \
            return;                                                             \
                                                                                \
        if (base::m_Data != nullptr && base::m_Capacity > 0)                    \
            Buffer::Delete(base::m_Data, base::m_Capacity);                     \
                                                                                \
        base::m_Capacity = other.m_Capacity;                                    \
        base::m_Count    = other.m_Count;                                       \
        base::m_Data     = other.m_Data;                                        \
    }                                                                           \
                                                                                \
    OTR_COLLECTION_CHILD(Type)<T>& operator=(Collection<T>&& other) noexcept    \
    {                                                                           \
        if (this == &other)                                                     \
            return *this;                                                       \
                                                                                \
        if (base::m_Data != nullptr && base::m_Capacity > 0)                    \
            Buffer::Delete(base::m_Data, base::m_Capacity);                     \
                                                                                \
        base::m_Capacity = other.m_Capacity;                                    \
        base::m_Count    = other.m_Count;                                       \
        base::m_Data     = other.m_Data;                                        \
                                                                                \
        return *this;                                                           \
    }

#endif //OTTERENGINE_COLLECTION_H
