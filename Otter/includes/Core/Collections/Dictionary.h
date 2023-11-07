#ifndef OTTERENGINE_DICTIONARY_H
#define OTTERENGINE_DICTIONARY_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Memory.h"
#include "Core/Delegates.h"

#include "Core/Collections/Utils/HashUtils.h"

namespace Otter
{
    // TODO: Not the best Dictionary implementation, but it works for now
    // TODO: Resize the Dictionary to minimise collisions
    template<typename TKey, typename TValue>
    class Dictionary final
    {
        using HashUtils = Internal::HashUtils;

    public:
        Dictionary()
            : m_Buckets(nullptr), m_Capacity(0), m_Count(0)
        {
        }

        ~Dictionary()
        {
            ClearDestructive();
        }

        explicit Dictionary(UInt64 capacity)
        {
            m_Capacity = HashUtils::GetNextPrime(capacity);
            m_Count    = 0;
            m_Buckets  = Buffer::New<Bucket>(m_Capacity);

            for (UInt64 i = 0; i < m_Capacity; i++)
                m_Buckets[i].m_Items = nullptr;
        }

        Dictionary(const Dictionary<TKey, TValue>& other)
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

        Dictionary(Dictionary<TKey, TValue>&& other) noexcept
        {
            m_Capacity = other.m_Capacity;
            m_Count    = other.m_Count;
            m_Buckets  = other.m_Buckets;

            other.m_Capacity = 0;
            other.m_Count    = 0;
            other.m_Buckets  = nullptr;
        }

        Dictionary<TKey, TValue>& operator=(const Dictionary<TKey, TValue>& other)
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

        Dictionary<TKey, TValue>& operator=(Dictionary<TKey, TValue>&& other) noexcept
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

        bool TryAdd(const TKey& key, const TValue& value, bool overwrite = false)
        {
            UInt64 hash  = GetHashCode(key) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (m_Buckets[index].m_Capacity == 0)
            {
                InitialiseBucketWithItem(&m_Buckets[index], key, value, hash);
                m_Count++;

                return true;
            }

            for (UInt64 i = 0; i < m_Buckets[index].m_Count; i++)
            {
                if (m_Buckets[index].m_Items[i].m_Pair.m_Key == key && m_Buckets[index].m_Items[i].m_Hash == hash)
                {
                    if (overwrite)
                    {
                        m_Buckets[index].m_Items[i].m_Pair.m_Value = value;
                        return true;
                    }

                    return false;
                }
            }

            if (m_Buckets[index].m_Count >= m_Buckets[index].m_Capacity)
                ResizeBucket(&m_Buckets[index]);

            m_Buckets[index].m_Items[m_Buckets[index].m_Count].m_Pair.m_Key   = key;
            m_Buckets[index].m_Items[m_Buckets[index].m_Count].m_Pair.m_Value = value;
            m_Buckets[index].m_Items[m_Buckets[index].m_Count].m_Hash         = hash;
            m_Buckets[index].m_Count++;

            m_Count++;

            return true;
        }

        bool TryAdd(TKey&& key, TValue&& value, bool overwrite = false) noexcept
        {
            UInt64 hash  = GetHashCode(key) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (m_Buckets[index].m_Capacity == 0)
            {
                InitialiseBucketWithItem(&m_Buckets[index], std::move(key), std::move(value), hash);
                m_Count++;

                return true;
            }

            for (UInt64 i = 0; i < m_Buckets[index].m_Count; i++)
            {
                if (m_Buckets[index].m_Items[i].m_Pair.m_Key == key && m_Buckets[index].m_Items[i].m_Hash == hash)
                {
                    if (overwrite)
                    {
                        m_Buckets[index].m_Items[i].m_Pair.m_Value = value;
                        return true;
                    }

                    return false;
                }
            }

            if (m_Buckets[index].m_Count >= m_Buckets[index].m_Capacity)
                ResizeBucket(&m_Buckets[index]);

            m_Buckets[index].m_Items[m_Buckets[index].m_Count].m_Pair.m_Key   = std::move(key);
            m_Buckets[index].m_Items[m_Buckets[index].m_Count].m_Pair.m_Value = std::move(value);
            m_Buckets[index].m_Items[m_Buckets[index].m_Count].m_Hash         = hash;
            m_Buckets[index].m_Count++;

            m_Count++;

            return true;
        }

        bool TryGet(const TKey& key, TValue& value) const { return TryGetInternal(key, value); }
        bool TryGet(TKey&& key, TValue& value) const noexcept { return TryGetInternal(std::move(key), value); }

        bool TryRemove(const TKey& key) { return TryRemoveInternal(key); }
        bool TryRemove(TKey&& key) noexcept { return TryRemoveInternal(std::move(key)); }

        [[nodiscard]] bool Contains(const TKey& key) const { return ContainsInternal(key); }
        [[nodiscard]] bool Contains(TKey&& key) const noexcept { return ContainsInternal(std::move(key)); }

        void ForEach(Action<const TKey&, const TValue&>& action) const
        {
            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Buckets[i].m_Items || m_Buckets[i].m_Capacity == 0)
                    continue;

                for (UInt64 j = 0; j < m_Buckets[i].m_Count; j++)
                    action(m_Buckets[i].m_Items[j].m_Pair.m_Key, m_Buckets[i].m_Items[j].m_Pair.m_Value);
            }
        }

        void ForEachKey(Action<const TKey&>& action) const
        {
            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Buckets[i].m_Items || m_Buckets[i].m_Capacity == 0)
                    continue;

                for (UInt64 j = 0; j < m_Buckets[i].m_Count; j++)
                    action(m_Buckets[i].m_Items[j].m_Pair.m_Key);
            }
        }

        void ForEachValue(Action<const TValue&>& action) const
        {
            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Buckets[i].m_Items || m_Buckets[i].m_Capacity == 0)
                    continue;

                for (UInt64 j = 0; j < m_Buckets[i].m_Count; j++)
                    action(m_Buckets[i].m_Items[j].m_Pair.m_Value);
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
        struct KeyValuePair
        {
            TKey   m_Key;
            TValue m_Value;
        };

        struct BucketItem
        {
            KeyValuePair m_Pair;
            UInt64       m_Hash;
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

        void InitialiseBucketWithItem(Bucket* bucket, const TKey& key, const TValue& value, const UInt64& hash) const
        {
            bucket->m_Items                   = Buffer::New<BucketItem>(k_InitialBucketCapacity);
            bucket->m_Items[0].m_Pair.m_Key   = key;
            bucket->m_Items[0].m_Pair.m_Value = value;
            bucket->m_Items[0].m_Hash         = hash;
            bucket->m_Capacity                = k_InitialBucketCapacity;
            bucket->m_Count                   = 1;
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

        [[nodiscard]] bool TryGetInternal(const TKey& key, TValue& value) const
        {
            UInt64 hash  = GetHashCode(key) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (m_Buckets[index].m_Capacity == 0)
                return false;

            for (UInt64 i = 0; i < m_Buckets[index].m_Count; i++)
            {
                if (m_Buckets[index].m_Items[i].m_Hash == hash && m_Buckets[index].m_Items[i].m_Pair.m_Key == key)
                {
                    value = m_Buckets[index].m_Items[i].m_Pair.m_Value;
                    return true;
                }
            }

            return false;
        }

        [[nodiscard]] bool TryRemoveInternal(const TKey& key)
        {
            UInt64 hash  = GetHashCode(key) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (m_Buckets[index].m_Capacity == 0)
                return false;

            for (UInt64 i = 0; i < m_Buckets[index].m_Count; i++)
            {
                if (m_Buckets[index].m_Items[i].m_Hash == hash && m_Buckets[index].m_Items[i].m_Pair.m_Key == key)
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

        [[nodiscard]] bool ContainsInternal(const TKey& key) const
        {
            UInt64 hash  = GetHashCode(key) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (m_Buckets[index].m_Capacity == 0)
                return false;

            for (UInt64 i = 0; i < m_Buckets[index].m_Count; i++)
                if (m_Buckets[index].m_Items[i].m_Pair.m_Key == key && m_Buckets[index].m_Items[i].m_Hash == hash)
                    return true;

            return false;
        }
    };
}

#endif //OTTERENGINE_DICTIONARY_H
