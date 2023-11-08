#ifndef OTTERENGINE_HASHSET_H
#define OTTERENGINE_HASHSET_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Memory.h"
#include "Core/Delegates.h"

#include "Core/Collections/Utils/HashUtils.h"

namespace Otter
{
    // TODO: Not the best HashSet implementation, but it works for now
    // TODO: Resize the HashSet to minimise collisions
    template<typename T>
    class HashSet final
    {
        using HashUtils = Internal::HashUtils;

    public:
        HashSet()
            : m_Buckets(nullptr), m_Capacity(0), m_Count(0)
        {
        }

        ~HashSet()
        {
            ClearDestructive();
        }

        explicit HashSet(UInt64 capacity)
        {
            m_Capacity = HashUtils::GetNextPrime(capacity);
            m_Count    = 0;
            m_Buckets  = Buffer::New<Bucket>(m_Capacity);

            for (UInt64 i = 0; i < m_Capacity; i++)
                m_Buckets[i].m_Items = nullptr;
        }

        HashSet(const HashSet<T>& other)
        {
            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Buckets  = Buffer::New<Bucket>(m_Capacity);

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (other.m_Buckets[i].m_Capacity == 0)
                    continue;

                m_Buckets[i].m_Items    = Buffer::New<BucketItem>(other.m_Buckets[i].m_Capacity);
                m_Buckets[i].m_Capacity = other.m_Buckets[i].m_Capacity;
                m_Buckets[i].m_Count    = other.m_Buckets[i].m_Count;

                for (UInt64 j = 0; j < m_Buckets[i].m_Count; j++)
                    m_Buckets[i].m_Items[j] = other.m_Buckets[i].m_Items[j];
            }
        }

        HashSet(HashSet<T>&& other) noexcept
        {
            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Buckets  = other.m_Buckets;

            other.m_Capacity = 0;
            other.m_Count    = 0;
            other.m_Buckets  = nullptr;
        }

        HashSet<T>& operator=(const HashSet<T>& other)
        {
            if (this == &other)
                return *this;

            ClearDestructive();

            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Buckets  = Buffer::New<Bucket>(m_Capacity);

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (other.m_Buckets[i].m_Capacity == 0)
                    continue;

                m_Buckets[i].m_Items    = Buffer::New<BucketItem>(other.m_Buckets[i].m_Capacity);
                m_Buckets[i].m_Capacity = other.m_Buckets[i].m_Capacity;
                m_Buckets[i].m_Count    = other.m_Buckets[i].m_Count;

                for (UInt64 j = 0; j < m_Buckets[i].m_Count; j++)
                    m_Buckets[i].m_Items[j] = other.m_Buckets[i].m_Items[j];
            }

            return *this;
        }

        HashSet<T>& operator=(HashSet<T>&& other) noexcept
        {
            if (this == &other)
                return *this;

            ClearDestructive();

            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Buckets  = other.m_Buckets;

            other.m_Capacity = 0;
            other.m_Count    = 0;
            other.m_Buckets  = nullptr;

            return *this;
        }

        bool TryAdd(const T& value)
        {
            UInt64 hash  = GetHashCode(value) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (m_Buckets[index].m_Capacity == 0)
            {
                InitialiseBucketWithItem(&m_Buckets[index], value, hash);
                m_Count++;

                return true;
            }

            if (ExistsInBucket(value, hash, m_Buckets[index]))
                return false;

            if (m_Buckets[index].m_Count >= m_Buckets[index].m_Capacity)
                ResizeBucket(&m_Buckets[index]);

            m_Buckets[index].m_Items[m_Buckets[index].m_Count].m_Data = value;
            m_Buckets[index].m_Items[m_Buckets[index].m_Count].m_Hash = hash;
            m_Buckets[index].m_Count++;

            m_Count++;

            return true;
        }

        bool TryAdd(T&& value) noexcept
        {
            UInt64 hash  = GetHashCode(value) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (m_Buckets[index].m_Capacity == 0)
            {
                InitialiseBucketWithItem(&m_Buckets[index], std::move(value), hash);
                m_Count++;

                return true;
            }

            if (ExistsInBucket(value, hash, m_Buckets[index]))
                return false;

            if (m_Buckets[index].m_Count >= m_Buckets[index].m_Capacity)
                ResizeBucket(&m_Buckets[index]);

            m_Buckets[index].m_Items[m_Buckets[index].m_Count].m_Data = std::move(value);
            m_Buckets[index].m_Items[m_Buckets[index].m_Count].m_Hash = hash;
            m_Buckets[index].m_Count++;

            m_Count++;

            return true;
        }

        bool TryGet(const T& value, T& outValue) const { return TryGetInternal(value, outValue); }
        bool TryGet(T&& value, T& outValue) const noexcept { return TryGetInternal(std::move(value), outValue); }

        bool TryRemove(const T& value) { return TryRemoveInternal(value); }
        bool TryRemove(T&& value) noexcept { return TryRemoveInternal(std::move(value)); }

        [[nodiscard]] bool Contains(const T& value) const { return ContainsInternal(value); }
        [[nodiscard]] bool Contains(T&& value) const noexcept { return ContainsInternal(std::move(value)); }

        void ForEach(Action<const T&>& action) const
        {
            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Buckets[i].m_Items || m_Buckets[i].m_Capacity == 0)
                    continue;

                for (UInt64 j = 0; j < m_Buckets[i].m_Count; j++)
                    action(m_Buckets[i].m_Items[j].m_Data);
            }
        }

        void Clear()
        {
            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Buckets[i].m_Items || m_Buckets[i].m_Capacity == 0)
                    continue;

                m_Buckets[i].m_Count = 0;
            }

            m_Count = 0;
        }

        void ClearDestructive()
        {
            if (!m_Buckets)
                return;

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Buckets[i].m_Items || m_Buckets[i].m_Capacity == 0)
                    continue;

                Buffer::Delete(m_Buckets[i].m_Items, m_Buckets[i].m_Capacity);
                m_Buckets[i].m_Items    = nullptr;
                m_Buckets[i].m_Capacity = 0;
                m_Buckets[i].m_Count    = 0;
            }

            Buffer::Delete(m_Buckets, m_Capacity);
            m_Buckets = nullptr;
        }

        [[nodiscard]] OTR_INLINE UInt64 GetCount() const noexcept { return m_Count; }

    private:
        struct BucketItem
        {
            T      m_Data;
            UInt64 m_Hash;
        };

        struct Bucket
        {
            BucketItem* m_Items;
            UInt64 m_Capacity;
            UInt64 m_Count;
        };

        const Int64   k_63BitMask             = 0x7FFFFFFFFFFFFFFF;
        const UInt16  k_InitialBucketCapacity = 3;
        const Float16 k_BucketResizingFactor  = 1.5;

        Bucket* m_Buckets;
        UInt64 m_Capacity;
        UInt64 m_Count;

        void InitialiseBucketWithItem(Bucket* bucket, const T& value, const UInt64& hash) const
        {
            bucket->m_Items           = Buffer::New<BucketItem>(k_InitialBucketCapacity);
            bucket->m_Items[0].m_Data = value;
            bucket->m_Items[0].m_Hash = hash;
            bucket->m_Capacity        = k_InitialBucketCapacity;
            bucket->m_Count           = 1;
        }

        void ResizeBucket(Bucket* bucket) const
        {
            UInt64 newCapacity = bucket->m_Capacity * k_BucketResizingFactor;
            BucketItem* newItems = Buffer::New<BucketItem>(newCapacity);

            for (UInt64 i = 0; i < bucket->m_Count; i++)
                newItems[i] = bucket->m_Items[i];

            Buffer::Delete(bucket->m_Items, bucket->m_Capacity);

            bucket->m_Items    = newItems;
            bucket->m_Capacity = newCapacity;
        }

        [[nodiscard]] bool ExistsInBucket(const T& value, const UInt64& hash, const Bucket& bucket) const
        {
            for (UInt64 i = 0; i < bucket.m_Count; i++)
                if (bucket.m_Items[i].m_Data == value && bucket.m_Items[i].m_Hash == hash)
                    return true;

            return false;
        }

        [[nodiscard]] bool TryGetInternal(const T& value, T& outValue) const
        {
            UInt64 hash  = GetHashCode(value) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (m_Buckets[index].m_Capacity == 0)
                return false;

            for (UInt64 i = 0; i < m_Buckets[index].m_Count; i++)
            {
                if (m_Buckets[index].m_Items[i].m_Hash == hash && m_Buckets[index].m_Items[i].m_Data == value)
                {
                    outValue = m_Buckets[index].m_Items[i].m_Data;
                    return true;
                }
            }

            return false;
        }

        [[nodiscard]] bool TryRemoveInternal(const T& value)
        {
            UInt64 hash  = GetHashCode(value) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (m_Buckets[index].m_Capacity == 0)
                return false;

            for (UInt64 i = 0; i < m_Buckets[index].m_Count; i++)
            {
                if (m_Buckets[index].m_Items[i].m_Hash == hash && m_Buckets[index].m_Items[i].m_Data == value)
                {
                    for (UInt64 j = i; j < m_Buckets[index].m_Count - 1; j++)
                        m_Buckets[index].m_Items[j] = m_Buckets[index].m_Items[j + 1];

                    m_Buckets[index].m_Count--;
                    m_Count--;

                    return true;
                }
            }

            return false;
        }

        [[nodiscard]] bool ContainsInternal(const T& value) const
        {
            UInt64 hash  = GetHashCode(value) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (m_Buckets[index].m_Capacity == 0)
                return false;

            if (ExistsInBucket(value, hash, m_Buckets[index]))
                return true;

            return false;
        }
    };
}

#endif //OTTERENGINE_HASHSET_H
