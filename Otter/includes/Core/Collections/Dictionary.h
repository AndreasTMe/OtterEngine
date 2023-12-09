#ifndef OTTERENGINE_DICTIONARY_H
#define OTTERENGINE_DICTIONARY_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Memory.h"
#include "Core/Function.h"

#include "Core/Collections/Utils/HashBucket.h"
#include "Core/Collections/Utils/HashUtils.h"
#include "Core/Collections/Utils/KeyValuePair.h"

namespace Otter
{
    template<typename TKey, typename TValue>
    class Dictionary final
    {
        using HashUtils = Internal::HashUtils;
        using KeyValuePair = KeyValuePair<TKey, TValue>;

    public:
        Dictionary()
        {
            if (IsCreated())
                Buffer::Delete<Bucket<KeyValuePair>>(m_Buckets, m_Capacity);
        }

        ~Dictionary()
        {
            if (IsCreated())
                Buffer::Delete<Bucket<KeyValuePair>>(m_Buckets, m_Capacity);
        }

        Dictionary(InitialiserList<KeyValuePair> list)
            : Dictionary()
        {
            m_Capacity = HashUtils::GetNextPrime(list.size());
            m_Count    = 0;
            m_Buckets  = Buffer::New<Bucket<KeyValuePair>>(m_Capacity);

            for (const KeyValuePair& pair: list)
                TryAdd(pair.Key, pair.Value);
        }

        Dictionary(const Dictionary<TKey, TValue>& other)
            : Dictionary()
        {
            m_Buckets  = other.m_Buckets;
            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
        }

        Dictionary(Dictionary<TKey, TValue>&& other) noexcept
            : Dictionary()
        {
            m_Buckets  = std::move(other.m_Buckets);
            m_Capacity = std::move(other.m_Capacity);
            m_Count    = std::move(other.m_Count);

            other.m_Buckets  = nullptr;
            other.m_Capacity = 0;
            other.m_Count    = 0;
        }

        Dictionary<TKey, TValue>& operator=(const Dictionary<TKey, TValue>& other)
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Buffer::Delete<Bucket<KeyValuePair>>(m_Buckets, m_Capacity);

            m_Buckets  = other.m_Buckets;
            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;

            return *this;
        }

        Dictionary<TKey, TValue>& operator=(Dictionary<TKey, TValue>&& other) noexcept
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Buffer::Delete<Bucket<KeyValuePair>>(m_Buckets, m_Capacity);

            m_Buckets  = std::move(other.m_Buckets);
            m_Capacity = std::move(other.m_Capacity);
            m_Count    = std::move(other.m_Count);

            other.m_Buckets  = nullptr;
            other.m_Capacity = 0;
            other.m_Count    = 0;

            return *this;
        }

        bool TryAdd(const TKey& key, const TValue& value)
        {
            if (m_Count >= m_Capacity)
                Expand();

            UInt64 hash  = GetHashCode(key) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (!m_Buckets[index].IsCreated())
            {
                m_Buckets[index].Items         = Buffer::New<BucketItem<KeyValuePair>>(k_InitialCapacity);
                m_Buckets[index].Items[0].Data = KeyValuePair{ key, value };
                m_Buckets[index].Items[0].Hash = hash;
                m_Buckets[index].Capacity      = k_InitialCapacity;
                m_Buckets[index].Count         = 1;

                m_Count++;

                return true;
            }

            if (ExistsInBucket(KeyValuePair{ key, value }, hash, m_Buckets[index]))
                return false;

            if (m_Buckets[index].Count >= m_Buckets[index].Capacity)
                ResizeBucket(&m_Buckets[index]);

            m_Buckets[index].Items[m_Buckets[index].Count].Data = KeyValuePair{ key, value };
            m_Buckets[index].Items[m_Buckets[index].Count].Hash = hash;
            m_Buckets[index].Count++;

            m_Count++;

            return true;
        }

        bool TryAdd(TKey&& key, TValue&& value) noexcept
        {
            if (m_Count >= m_Capacity)
                Expand();

            UInt64 hash  = GetHashCode(key) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (!m_Buckets[index].IsCreated())
            {
                m_Buckets[index].Items         = Buffer::New<BucketItem<KeyValuePair>>(k_InitialCapacity);
                m_Buckets[index].Items[0].Data = std::move(KeyValuePair{ key, value });
                m_Buckets[index].Items[0].Hash = hash;
                m_Buckets[index].Capacity      = k_InitialCapacity;
                m_Buckets[index].Count         = 1;

                m_Count++;

                return true;
            }

            if (ExistsInBucket(KeyValuePair{ key, value }, hash, m_Buckets[index]))
                return false;

            if (m_Buckets[index].Count >= m_Buckets[index].Capacity)
                ResizeBucket(&m_Buckets[index]);

            m_Buckets[index].Items[m_Buckets[index].Count].Data = std::move(KeyValuePair{ key, value });
            m_Buckets[index].Items[m_Buckets[index].Count].Hash = hash;
            m_Buckets[index].Count++;

            m_Count++;

            return true;
        }

        bool TryGet(const TKey& key, TValue* outValue) const
        {
            if (!IsCreated())
                return false;

            UInt64 hash  = GetHashCode(key) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (!m_Buckets[index].IsCreated())
                return false;

            for (UInt64 i = 0; i < m_Buckets[index].Count; i++)
            {
                if (m_Buckets[index].Items[i].Hash == hash && m_Buckets[index].Items[i].Data.Key == key)
                {
                    *outValue = m_Buckets[index].Items[i].Data.Value;
                    return true;
                }
            }

            return false;
        }

        bool TryRemove(const TKey& key)
        {
            if (!IsCreated())
                return false;

            UInt64 hash  = GetHashCode(key) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (!m_Buckets[index].IsCreated())
                return false;

            for (UInt64 i = 0; i < m_Buckets[index].Count; i++)
            {
                if (m_Buckets[index].Items[i].Hash == hash && m_Buckets[index].Items[i].Data.Key == key)
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

        [[nodiscard]] bool Contains(const TKey& key) const
        {
            if (!IsCreated())
                return false;

            UInt64 hash  = GetHashCode(key) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (!m_Buckets[index].IsCreated())
                return false;

            for (UInt64 i = 0; i < m_Buckets[index].Count; i++)
                if (m_Buckets[index].Items[i].Hash == hash && m_Buckets[index].Items[i].Data.Key == key)
                    return true;

            return false;
        }

        void ForEach(Function<void(const TKey&, const TValue&)> callback) const
        {
            if (!IsCreated())
                return;

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Buckets[i].IsCreated())
                    continue;

                for (UInt64 j = 0; j < m_Buckets[i].Count; j++)
                    callback(m_Buckets[i].Items[j].Data.Key, m_Buckets[i].Items[j].Data.Value);
            }
        }

        void ForEachKey(Function<void(const TKey&)> callback) const
        {
            if (!IsCreated())
                return;

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Buckets[i].IsCreated())
                    continue;

                for (UInt64 j = 0; j < m_Buckets[i].Count; j++)
                    callback(m_Buckets[i].Items[j].Data.Key);
            }
        }

        void ForEachValue(Function<void(const TValue&)> callback) const
        {
            if (!IsCreated())
                return;

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Buckets[i].IsCreated())
                    continue;

                for (UInt64 j = 0; j < m_Buckets[i].Count; j++)
                    callback(m_Buckets[i].Items[j].Data.Value);
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

                Buffer::Delete<BucketItem<KeyValuePair>>(m_Buckets[i].Items, m_Buckets[i].Capacity);
                m_Buckets[i].Items    = nullptr;
                m_Buckets[i].Capacity = 0;
                m_Buckets[i].Count    = 0;
            }

            m_Count = 0;
        }

        void ClearDestructive()
        {
            if (IsCreated())
                Buffer::Delete<Bucket<KeyValuePair>>(m_Buckets, m_Capacity);

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

            MemorySystem::CheckMemoryFootprint([&]()
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

        Bucket<KeyValuePair>* m_Buckets = nullptr;
        UInt64 m_Capacity = 0;
        UInt64 m_Count    = 0;

        void Expand()
        {
            UInt64 newCapacity = m_Capacity == 0
                                 ? k_InitialCapacity
                                 : HashUtils::GetNextPrime(m_Capacity * k_ResizingFactor);
            Bucket<KeyValuePair>* newBuckets = Buffer::New<Bucket<KeyValuePair>>(newCapacity);

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
                        newBuckets[index].Items         = Buffer::New<BucketItem<KeyValuePair>>(k_InitialCapacity);
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
                Buffer::Delete<Bucket<KeyValuePair>>(m_Buckets, m_Capacity);

            m_Buckets  = newBuckets;
            m_Capacity = newCapacity;
        }

        void ResizeBucket(Bucket<KeyValuePair>* bucket) const
        {
            UInt64 newCapacity = bucket->Capacity * k_ResizingFactor;
            BucketItem<KeyValuePair>* newItems = Buffer::New<BucketItem<KeyValuePair>>(newCapacity);

            for (UInt64 i = 0; i < bucket->Count; i++)
                newItems[i] = bucket->Items[i];

            Buffer::Delete<BucketItem<KeyValuePair>>(bucket->Items, bucket->Capacity);

            bucket->Items    = newItems;
            bucket->Capacity = newCapacity;
        }

        [[nodiscard]] bool ExistsInBucket(const KeyValuePair& value,
                                          const UInt64 hash,
                                          const Bucket<KeyValuePair>& bucket) const
        {
            for (UInt64 i = 0; i < bucket.Count; i++)
                if (bucket.Items[i].Data.Key == value.Key && bucket.Items[i].Hash == hash)
                    return true;

            return false;
        }
    };
}

#endif //OTTERENGINE_DICTIONARY_H
