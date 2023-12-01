#ifndef OTTERENGINE_COLLECTION_H
#define OTTERENGINE_COLLECTION_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Memory.h"

namespace Otter
{
    template<typename T>
    class Collection;

    class Collections final
    {
    public:
        template<typename T>
        static void New(const T* const data, const UInt64 count, Collection<T>& outCollection)
        {
            OTR_INTERNAL_ASSERT_MSG(data != nullptr, "Data cannot be null!")
            OTR_INTERNAL_ASSERT_MSG(count > 0, "Count must be greater than 0!")

            if (outCollection.m_Data && outCollection.m_Capacity > 0)
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

        template<typename T>
        static Collection<T> New(InitialiserList<T> list)
        {
            Collection<T> collection;
            collection.m_Capacity = list.size();
            collection.m_Data     = Buffer::New<T>(collection.m_Capacity);

            collection.m_Count = 0;
            for (const T& item: list)
                collection.m_Data[collection.m_Count++] = item;

            return collection;
        }

        template<typename T>
        static void New(InitialiserList<T> list, Collection<T>& outCollection)
        {
            if (outCollection.m_Data && outCollection.m_Capacity > 0)
                Buffer::Delete(outCollection.m_Data, outCollection.m_Capacity);

            outCollection.m_Capacity = list.size();
            outCollection.m_Data     = Buffer::New<T>(outCollection.m_Capacity);

            outCollection.m_Count = 0;
            for (const T& item: list)
                outCollection.m_Data[outCollection.m_Count++] = item;
        }

        template<typename T>
        OTR_INLINE static Collection<T> Empty() { return Collection<T>(); }
    };

    template<typename T>
    class Collection
    {
    public:
        ~Collection()
        {
            if (IsCreated())
                Buffer::Delete(m_Data, m_Capacity);
        }

        void Reserve(const UInt64 capacity)
        {
            RecreateEmpty(capacity);
        }

        void Expand(const UInt64 amount = 0)
        {
            UInt64 newCapacity = CalculateExpandCapacity(amount);

            T* newData = Buffer::New<T>(newCapacity);

            for (UInt64 i = 0; i < m_Count; i++)
                newData[i] = m_Data[i];

            if (IsCreated())
                Buffer::Delete(m_Data, m_Capacity);

            m_Data     = newData;
            m_Capacity = newCapacity;
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

            for (UInt64 i = 0; i < m_Count && i < amount; i++)
                newData[i] = m_Data[i];

            if (IsCreated())
                Buffer::Delete(m_Data, m_Capacity);

            m_Data     = newData;
            m_Capacity = newCapacity;

            if (m_Count >= newCapacity)
                m_Count = newCapacity;
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
            if (IsCreated())
                Buffer::Delete(m_Data, m_Capacity);

            m_Data     = nullptr;
            m_Capacity = 0;
            m_Count    = 0;
        }

        [[nodiscard]] OTR_INLINE constexpr UInt64 GetCapacity() const { return m_Capacity; }
        [[nodiscard]] OTR_INLINE constexpr UInt64 GetCount() const { return m_Count; }
        [[nodiscard]] OTR_INLINE bool IsCreated() { return m_Data && m_Capacity > 0; }
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

    private:
        void RecreateEmpty(const UInt64 capacity)
        {
            if (IsCreated())
                Buffer::Delete(m_Data, m_Capacity);

            m_Data     = capacity > 0 ? Buffer::New<T>(capacity) : nullptr;
            m_Capacity = capacity;
            m_Count    = 0;
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

            const auto currentCount = GetCount();

            if (!isDestructive && newCapacity < currentCount)
                newCapacity = currentCount;

            return newCapacity;
        }
    };
}

#define OTR_COLLECTION_CHILD(Type) Type

#define OTR_COLLECTION_CONSTRUCT(Type)                                          \
    OTR_COLLECTION_CHILD(Type)() : Collection<T>() { }                          \
    ~OTR_COLLECTION_CHILD(Type)()                                               \
    {                                                                           \
        if (base::IsCreated())                                                  \
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

#define OTR_COLLECTION_COPY(Type)                                                           \
    OTR_COLLECTION_CHILD(Type)(const OTR_COLLECTION_CHILD(Type)<T>& other)                  \
    {                                                                                       \
        if (this == &other)                                                                 \
            return;                                                                         \
                                                                                            \
        if (base::IsCreated())                                                              \
            Buffer::Delete(base::m_Data, base::m_Capacity);                                 \
                                                                                            \
        base::m_Capacity = other.m_Capacity;                                                \
        base::m_Count    = other.m_Count;                                                   \
        base::m_Data     = other.m_Data;                                                    \
    }                                                                                       \
                                                                                            \
    OTR_COLLECTION_CHILD(Type)<T>& operator=(const OTR_COLLECTION_CHILD(Type)<T>& other)    \
    {                                                                                       \
        if (this == &other)                                                                 \
            return *this;                                                                   \
                                                                                            \
        if (base::IsCreated())                                                              \
            Buffer::Delete(base::m_Data, base::m_Capacity);                                 \
                                                                                            \
        base::m_Capacity = other.m_Capacity;                                                \
        base::m_Count    = other.m_Count;                                                   \
        base::m_Data     = other.m_Data;                                                    \
                                                                                            \
        return *this;                                                                       \
    }

#define OTR_COLLECTION_MOVE(Type)                                                               \
    OTR_COLLECTION_CHILD(Type)(OTR_COLLECTION_CHILD(Type)<T>&& other) noexcept                  \
    {                                                                                           \
        if (this == &other)                                                                     \
            return;                                                                             \
                                                                                                \
        if (base::IsCreated())                                                                  \
            Buffer::Delete(base::m_Data, base::m_Capacity);                                     \
                                                                                                \
        base::m_Capacity = std::move(other.m_Capacity);                                         \
        base::m_Count    = std::move(other.m_Count);                                            \
        base::m_Data     = std::move(other.m_Data);                                             \
                                                                                                \
        other.m_Capacity = 0;                                                                   \
        other.m_Count    = 0;                                                                   \
        other.m_Data     = nullptr;                                                             \
    }                                                                                           \
                                                                                                \
    OTR_COLLECTION_CHILD(Type)<T>& operator=(OTR_COLLECTION_CHILD(Type)<T>&& other) noexcept    \
    {                                                                                           \
        if (this == &other)                                                                     \
            return *this;                                                                       \
                                                                                                \
        if (base::IsCreated())                                                                  \
            Buffer::Delete(base::m_Data, base::m_Capacity);                                     \
                                                                                                \
        base::m_Capacity = std::move(other.m_Capacity);                                         \
        base::m_Count    = std::move(other.m_Count);                                            \
        base::m_Data     = std::move(other.m_Data);                                             \
                                                                                                \
        other.m_Capacity = 0;                                                                   \
        other.m_Count    = 0;                                                                   \
        other.m_Data     = nullptr;                                                             \
                                                                                                \
        return *this;                                                                           \
    }

#endif //OTTERENGINE_COLLECTION_H
