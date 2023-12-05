#ifndef OTTERENGINE_HASHSET_H
#define OTTERENGINE_HASHSET_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Memory.h"
#include "Core/Function.h"

#include "Core/Collections/Utils/HashBucket.h"
#include "Core/Collections/Utils/HashUtils.h"

namespace Otter
{
    template<typename T>
    class HashSet final
    {
        using HashUtils = Internal::HashUtils;

    public:
        HashSet()
        {
            if (IsCreated())
                Buffer::Delete<Bucket<T>>(m_Buckets, m_Capacity);
        }

        ~HashSet()
        {
            if (IsCreated())
                Buffer::Delete<Bucket<T>>(m_Buckets, m_Capacity);
        }

        HashSet(InitialiserList<T> list)
            : HashSet()
        {
            m_Capacity = HashUtils::GetNextPrime(list.size());
            m_Count    = 0;
            m_Buckets  = Buffer::New<Bucket<T>>(m_Capacity);

            for (const T& item: list)
                TryAdd(item);
        }

        HashSet(const HashSet<T>& other)
            : HashSet()
        {
            m_Buckets  = other.m_Buckets;
            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
        }

        HashSet(HashSet<T>&& other) noexcept
            : HashSet()
        {
            m_Buckets  = std::move(other.m_Buckets);
            m_Capacity = std::move(other.m_Capacity);
            m_Count    = std::move(other.m_Count);

            other.m_Buckets  = nullptr;
            other.m_Capacity = 0;
            other.m_Count    = 0;
        }

        HashSet<T>& operator=(const HashSet<T>& other)
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Buffer::Delete<Bucket<T>>(m_Buckets, m_Capacity);

            m_Buckets  = other.m_Buckets;
            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;

            return *this;
        }

        HashSet<T>& operator=(HashSet<T>&& other) noexcept
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Buffer::Delete<Bucket<T>>(m_Buckets, m_Capacity);

            m_Buckets  = std::move(other.m_Buckets);
            m_Capacity = std::move(other.m_Capacity);
            m_Count    = std::move(other.m_Count);

            other.m_Buckets  = nullptr;
            other.m_Capacity = 0;
            other.m_Count    = 0;

            return *this;
        }

        bool TryAdd(const T& value)
        {
            if (m_Count >= m_Capacity)
                Expand();

            UInt64 hash  = GetHashCode(value) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (!m_Buckets[index].IsCreated())
            {
                m_Buckets[index].Items         = Buffer::New<BucketItem<T>>(k_InitialCapacity);
                m_Buckets[index].Items[0].Data = value;
                m_Buckets[index].Items[0].Hash = hash;
                m_Buckets[index].Capacity      = k_InitialCapacity;
                m_Buckets[index].Count         = 1;

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
            if (m_Count >= m_Capacity)
                Expand();

            UInt64 hash  = GetHashCode(value) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (!m_Buckets[index].IsCreated())
            {
                m_Buckets[index].Items         = Buffer::New<BucketItem<T>>(k_InitialCapacity);
                m_Buckets[index].Items[0].Data = std::move(value);
                m_Buckets[index].Items[0].Hash = hash;
                m_Buckets[index].Capacity      = k_InitialCapacity;
                m_Buckets[index].Count         = 1;

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

        bool TryRemove(const T& value)
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

        [[nodiscard]] bool Contains(const T& value) const
        {
            UInt64 hash  = GetHashCode(value) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (m_Buckets[index].Capacity == 0)
                return false;

            if (ExistsInBucket(value, hash, m_Buckets[index]))
                return true;

            return false;
        }

        void ForEach(Function<void(const T&)> callback)
        {
            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Buckets[i].IsCreated())
                    continue;

                for (UInt64 j = 0; j < m_Buckets[i].Count; j++)
                    callback(m_Buckets[i].Items[j].Data);
            }
        }

        void Clear()
        {
            if (!IsCreated())
                return;

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Buckets[i].IsCreated())
                    continue;

                Buffer::Delete<BucketItem<T>>(m_Buckets[i].Items, m_Buckets[i].Capacity);
                m_Buckets[i].Items    = nullptr;
                m_Buckets[i].Capacity = 0;
                m_Buckets[i].Count    = 0;
            }

            m_Count = 0;
        }

        void ClearDestructive()
        {
            if (IsCreated())
                Buffer::Delete<Bucket<T>>(m_Buckets, m_Capacity);

            m_Buckets  = nullptr;
            m_Capacity = 0;
            m_Count    = 0;
        }

#if !OTR_RUNTIME
        void GetMemoryFootprint(const char* const debugName,
                                MemoryFootprint* outFootprints,
                                UInt64* outFootprintsSize) const
        {
            if (!outFootprints)
            {
                *outFootprintsSize = 1 + m_Capacity;
                return;
            }

            OTR_MEMORY_SYSTEM.CheckMemoryFootprint([&]()
                                                   {
                                                       MemoryDebugPair pairs[1 + m_Capacity];
                                                       pairs[0] = { debugName, m_Buckets };

                                                       for (UInt64 i = 0; i < m_Capacity; i++)
                                                       {
                                                           pairs[i + 1] = MemoryDebugPair(
                                                               ("bucket_" + std::to_string(i)).c_str(),
                                                               m_Buckets[i].IsCreated() ? m_Buckets[i].Items
                                                                                        : nullptr
                                                           );
                                                       }

                                                       return MemoryDebugHandle{ pairs, 1 + m_Capacity };
                                                   },
                                                   outFootprints,
                                                   nullptr);
        }
#endif

        [[nodiscard]] OTR_INLINE constexpr UInt64 GetCount() const noexcept { return m_Count; }
        [[nodiscard]] OTR_INLINE constexpr bool IsCreated() const noexcept { return m_Buckets && m_Capacity > 0; }
        [[nodiscard]] OTR_INLINE constexpr bool IsEmpty() const noexcept { return m_Count == 0; }

    private:
        static constexpr Int64   k_63BitMask       = 0x7FFFFFFFFFFFFFFF;
        static constexpr UInt16  k_InitialCapacity = 3;
        static constexpr Float16 k_ResizingFactor  = static_cast<Float16>(1.5);

        Bucket<T>* m_Buckets = nullptr;
        UInt64 m_Capacity = 0;
        UInt64 m_Count    = 0;

        void Expand()
        {
            UInt64 newCapacity = m_Capacity == 0
                                 ? k_InitialCapacity
                                 : HashUtils::GetNextPrime(m_Capacity * k_ResizingFactor);
            Bucket<T>* newBuckets = Buffer::New<Bucket<T>>(newCapacity);

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Buckets[i].IsCreated())
                    continue;

                for (UInt64 j = 0; j < m_Buckets[i].Count; j++)
                {
                    UInt64 hash  = m_Buckets[i].Items[j].Hash & k_63BitMask;
                    UInt64 index = hash % newCapacity;

                    if (!newBuckets[index].IsCreated())
                    {
                        newBuckets[index].Items         = Buffer::New<BucketItem<T>>(k_InitialCapacity);
                        newBuckets[index].Items[0].Data = m_Buckets[i].Items[j].Data;
                        newBuckets[index].Items[0].Hash = hash;
                        newBuckets[index].Capacity      = k_InitialCapacity;
                        newBuckets[index].Count         = 1;

                        continue;
                    }

                    if (ExistsInBucket(m_Buckets[i].Items[j].Data, hash, newBuckets[index]))
                        continue;

                    if (newBuckets[index].Count >= newBuckets[index].Capacity)
                        ResizeBucket(&newBuckets[index]);

                    newBuckets[index].Items[newBuckets[index].Count].Data = m_Buckets[i].Items[j].Data;
                    newBuckets[index].Items[newBuckets[index].Count].Hash = hash;
                    newBuckets[index].Count++;
                }
            }

            if (IsCreated())
                Buffer::Delete<Bucket<T>>(m_Buckets, m_Capacity);

            m_Buckets  = newBuckets;
            m_Capacity = newCapacity;
        }

        void ResizeBucket(Bucket<T>* bucket)
        {
            UInt64 newCapacity = bucket->Capacity * k_ResizingFactor;
            BucketItem<T>* newItems = Buffer::New<BucketItem<T>>(newCapacity);

            for (UInt64 i = 0; i < bucket->Count; i++)
                newItems[i] = bucket->Items[i];

            Buffer::Delete<BucketItem<T>>(bucket->Items, bucket->Capacity);

            bucket->Items    = newItems;
            bucket->Capacity = newCapacity;
        }

        [[nodiscard]] bool ExistsInBucket(const T& value, const UInt64 hash, const Bucket<T>& bucket) const
        {
            for (UInt64 i = 0; i < bucket.Count; i++)
                if (bucket.Items[i].Data == value && bucket.Items[i].Hash == hash)
                    return true;

            return false;
        }
    };
}

#endif //OTTERENGINE_HASHSET_H
