#ifndef OTTERENGINE_COLLECTION_H
#define OTTERENGINE_COLLECTION_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Core/Memory.h"

#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
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

    protected:
        Collection()
            : m_Data(nullptr), m_Capacity(0), m_Count(0)
        {
        }

        T* m_Data;
        UInt64 m_Capacity;
        UInt64 m_Count;
    };
}

#define OTR_COLLECTION_CHILD(Type) Type

#define OTR_COLLECTION_CONSTRUCT(Type)                                                      \
    OTR_COLLECTION_CHILD(Type)() : Collection<T>() { }                                      \
    ~OTR_COLLECTION_CHILD(Type)()                                                           \
    {                                                                                       \
        if (base::m_Data != nullptr && base::m_Capacity > 0)                                \
            Buffer::Delete(base::m_Data, base::m_Capacity);                                 \
                                                                                            \
        base::m_Data     = nullptr;                                                         \
        base::m_Capacity = 0;                                                               \
        base::m_Count    = 0;                                                               \
    }                                                                                       \
                                                                                            \
    OTR_INLINE explicit OTR_COLLECTION_CHILD(Type)(const T& value)                          \
    {                                                                                       \
        base::m_Capacity = 1;                                                               \
                                                                                            \
        base::m_Data = Buffer::New<T>(base::m_Capacity);                                    \
        base::m_Data[0] = value;                                                            \
                                                                                            \
        base::m_Count = 1;                                                                  \
    }                                                                                       \
                                                                                            \
    template<typename... Args>                                                              \
    OTR_INLINE explicit OTR_COLLECTION_CHILD(Type)(const T& value, const Args&& ... args)   \
    {                                                                                       \
        base::m_Capacity = VariadicArgs<Args...>::GetSize() + 1;                            \
        base::m_Data     = Buffer::New<T>(base::m_Capacity);                                \
                                                                                            \
        UInt64 i = 0;                                                                       \
        base::m_Data[i++] = value;                                                          \
        ([&]()                                                                              \
        {                                                                                   \
            base::m_Data[i++] = args;                                                       \
        }(), ...);                                                                          \
                                                                                            \
        base::m_Count = base::m_Capacity;                                                   \
    }

#define OTR_COLLECTION_COPY(Type)                                                   \
    OTR_INLINE explicit OTR_COLLECTION_CHILD(Type)(const Collection<T>& other)      \
    {                                                                               \
        if (this == &other)                                                         \
            return;                                                                 \
                                                                                    \
        if (base::m_Data != nullptr && base::m_Capacity > 0)                        \
            Buffer::Delete(base::m_Data, base::m_Capacity);                         \
                                                                                    \
        base::m_Capacity = other.m_Capacity;                                        \
        base::m_Count    = other.m_Count;                                           \
        base::m_Data     = other.m_Data;                                            \
    }                                                                               \
                                                                                    \
    OTR_INLINE OTR_COLLECTION_CHILD(Type)<T>& operator=(const Collection<T>& other) \
    {                                                                               \
        if (this == &other)                                                         \
            return *this;                                                           \
                                                                                    \
        if (base::m_Data != nullptr && base::m_Capacity > 0)                        \
            Buffer::Delete(base::m_Data, base::m_Capacity);                         \
                                                                                    \
        base::m_Capacity = other.m_Capacity;                                        \
        base::m_Count    = other.m_Count;                                           \
        base::m_Data     = other.m_Data;                                            \
                                                                                    \
        return *this;                                                               \
    }

#define OTR_COLLECTION_MOVE(Type)                                                       \
    OTR_INLINE explicit OTR_COLLECTION_CHILD(Type)(Collection<T>&& other) noexcept      \
    {                                                                                   \
        if (this == &other)                                                             \
            return;                                                                     \
                                                                                        \
        if (base::m_Data != nullptr && base::m_Capacity > 0)                            \
            Buffer::Delete(base::m_Data, base::m_Capacity);                             \
                                                                                        \
        base::m_Capacity = other.m_Capacity;                                            \
        base::m_Count    = other.m_Count;                                               \
        base::m_Data     = other.m_Data;                                                \
    }                                                                                   \
                                                                                        \
    OTR_INLINE OTR_COLLECTION_CHILD(Type)<T>& operator=(Collection<T>&& other) noexcept \
    {                                                                                   \
        if (this == &other)                                                             \
            return *this;                                                               \
                                                                                        \
        if (base::m_Data != nullptr && base::m_Capacity > 0)                            \
            Buffer::Delete(base::m_Data, base::m_Capacity);                             \
                                                                                        \
        base::m_Capacity = other.m_Capacity;                                            \
        base::m_Count    = other.m_Count;                                               \
        base::m_Data     = other.m_Data;                                                \
                                                                                        \
        return *this;                                                                   \
    }

#endif //OTTERENGINE_COLLECTION_H
