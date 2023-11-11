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
                m_Buckets[i].Items = nullptr;
        }

        HashSet(const HashSet<T>& other)
        {
            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Buckets  = Buffer::New<Bucket>(m_Capacity);

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (other.m_Buckets[i].Capacity == 0)
                    continue;

                m_Buckets[i].Items    = Buffer::New<BucketItem>(other.m_Buckets[i].Capacity);
                m_Buckets[i].Capacity = other.m_Buckets[i].Capacity;
                m_Buckets[i].Count    = other.m_Buckets[i].Count;

                for (UInt64 j = 0; j < m_Buckets[i].Count; j++)
                    m_Buckets[i].Items[j] = other.m_Buckets[i].Items[j];
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
                if (other.m_Buckets[i].Capacity == 0)
                    continue;

                m_Buckets[i].Items    = Buffer::New<BucketItem>(other.m_Buckets[i].Capacity);
                m_Buckets[i].Capacity = other.m_Buckets[i].Capacity;
                m_Buckets[i].Count    = other.m_Buckets[i].Count;

                for (UInt64 j = 0; j < m_Buckets[i].Count; j++)
                    m_Buckets[i].Items[j] = other.m_Buckets[i].Items[j];
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

            if (m_Buckets[index].Capacity == 0)
            {
                InitialiseBucketWithItem(&m_Buckets[index], value, hash);
                m_Count++;

                return true;
            }

            if (ExistsInBucket(value, hash, m_Buckets[index]))
                return false;

            if (m_Buckets[index].Count >= m_Buckets[index].Capacity)
                ResizeBucket(&m_Buckets[index]);

            m_Buckets[index].Items[m_Buckets[index].Count].Data = value;
            m_Buckets[index].Items[m_Buckets[index].Count].Hash = hash;
            m_Buckets[index].Count++;

            m_Count++;

            return true;
        }

        bool TryAdd(T&& value) noexcept
        {
            UInt64 hash  = GetHashCode(value) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (m_Buckets[index].Capacity == 0)
            {
                InitialiseBucketWithItem(&m_Buckets[index], std::move(value), hash);
                m_Count++;

                return true;
            }

            if (ExistsInBucket(value, hash, m_Buckets[index]))
                return false;

            if (m_Buckets[index].Count >= m_Buckets[index].Capacity)
                ResizeBucket(&m_Buckets[index]);

            m_Buckets[index].Items[m_Buckets[index].Count].Data = std::move(value);
            m_Buckets[index].Items[m_Buckets[index].Count].Hash = hash;
            m_Buckets[index].Count++;

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
                if (!m_Buckets[i].Items || m_Buckets[i].Capacity == 0)
                    continue;

                for (UInt64 j = 0; j < m_Buckets[i].Count; j++)
                    action(m_Buckets[i].Items[j].Data);
            }
        }

        void Clear()
        {
            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Buckets[i].Items || m_Buckets[i].Capacity == 0)
                    continue;

                m_Buckets[i].Count = 0;
            }

            m_Count = 0;
        }

        void ClearDestructive()
        {
            if (!m_Buckets)
                return;

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Buckets[i].Items || m_Buckets[i].Capacity == 0)
                    continue;

                Buffer::Delete(m_Buckets[i].Items, m_Buckets[i].Capacity);
                m_Buckets[i].Items    = nullptr;
                m_Buckets[i].Capacity = 0;
                m_Buckets[i].Count    = 0;
            }

            Buffer::Delete(m_Buckets, m_Capacity);
            m_Buckets = nullptr;
        }

        [[nodiscard]] OTR_INLINE UInt64 GetCount() const noexcept { return m_Count; }

    private:
        struct BucketItem
        {
            T      Data;
            UInt64 Hash;
        };

        struct Bucket
        {
            BucketItem* Items;
            UInt64 Capacity;
            UInt64 Count;
        };

        const Int64   k_63BitMask             = 0x7FFFFFFFFFFFFFFF;
        const UInt16  k_InitialBucketCapacity = 3;
        const Float16 k_BucketResizingFactor  = 1.5;

        Bucket* m_Buckets;
        UInt64 m_Capacity;
        UInt64 m_Count;

        void InitialiseBucketWithItem(Bucket* bucket, const T& value, const UInt64& hash) const
        {
            bucket->Items         = Buffer::New<BucketItem>(k_InitialBucketCapacity);
            bucket->Items[0].Data = value;
            bucket->Items[0].Hash = hash;
            bucket->Capacity      = k_InitialBucketCapacity;
            bucket->Count         = 1;
        }

        void ResizeBucket(Bucket* bucket) const
        {
            UInt64 newCapacity = bucket->Capacity * k_BucketResizingFactor;
            BucketItem* newItems = Buffer::New<BucketItem>(newCapacity);

            for (UInt64 i = 0; i < bucket->Count; i++)
                newItems[i] = bucket->Items[i];

            Buffer::Delete(bucket->Items, bucket->Capacity);

            bucket->Items    = newItems;
            bucket->Capacity = newCapacity;
        }

        [[nodiscard]] bool ExistsInBucket(const T& value, const UInt64& hash, const Bucket& bucket) const
        {
            for (UInt64 i = 0; i < bucket.Count; i++)
                if (bucket.Items[i].Data == value && bucket.Items[i].Hash == hash)
                    return true;

            return false;
        }

        [[nodiscard]] bool TryGetInternal(const T& value, T& outValue) const
        {
            UInt64 hash  = GetHashCode(value) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (m_Buckets[index].Capacity == 0)
                return false;

            for (UInt64 i = 0; i < m_Buckets[index].Count; i++)
            {
                if (m_Buckets[index].Items[i].Hash == hash && m_Buckets[index].Items[i].Data == value)
                {
                    outValue = m_Buckets[index].Items[i].Data;
                    return true;
                }
            }

            return false;
        }

        [[nodiscard]] bool TryRemoveInternal(const T& value)
        {
            UInt64 hash  = GetHashCode(value) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (m_Buckets[index].Capacity == 0)
                return false;

            for (UInt64 i = 0; i < m_Buckets[index].Count; i++)
            {
                if (m_Buckets[index].Items[i].Hash == hash && m_Buckets[index].Items[i].Data == value)
                {
                    for (UInt64 j = i; j < m_Buckets[index].Count - 1; j++)
                        m_Buckets[index].Items[j] = m_Buckets[index].Items[j + 1];

                    m_Buckets[index].Count--;
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

            if (m_Buckets[index].Capacity == 0)
                return false;

            if (ExistsInBucket(value, hash, m_Buckets[index]))
                return true;

            return false;
        }
    };
}

#endif //OTTERENGINE_HASHSET_H
