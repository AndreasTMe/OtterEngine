#ifndef OTTERENGINE_HASHSET_H
#define OTTERENGINE_HASHSET_H

#include <vector>

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Memory.h"

namespace Otter
{
    template<typename T>
    class HashSet final
    {
    public:
//        HashSet(const HashSet<T>& other);
//        HashSet<T>& operator=(const HashSet<T>& other);
//        HashSet(HashSet<T>&& other) noexcept;
//        HashSet<T>& operator=(HashSet<T>&& other) noexcept;

//        bool TryGet(T&& item, T& outItem) const noexcept;
//        bool TryAdd(T&& item) noexcept;
//        bool TryRemove(T&& item) noexcept;
//        bool Contains(T&& item) const noexcept;

        HashSet()
        {
            UInt64 alignedSize = OTR_ALIGNED_OFFSET(sizeof(void*), OTR_PLATFORM_MEMORY_ALIGNMENT);
            m_BucketHandle = Unsafe::New(m_TempSetCapacity * alignedSize);
        }
        ~HashSet()
        {
            if (m_BucketHandle.m_Pointer)
                Unsafe::Delete(m_BucketHandle);
        }

        bool TryAdd(const T& value)
        {
            auto index = GetIndex(value);
            auto* bucket = (Bucket*) m_BucketHandle.m_Pointer;
            UInt64 alignedSize = OTR_ALIGNED_OFFSET(sizeof(void*), OTR_PLATFORM_MEMORY_ALIGNMENT);

            if (!bucket[index].m_Data)
            {
                auto bucketContentHandle = Unsafe::New(alignedSize * m_TempBucketCapacity);

                bucket[index].m_Data = (T*) bucketContentHandle.m_Pointer;
                bucket[index].m_Data[0] = value;
                bucket[index].m_Capacity = bucketContentHandle.m_Size;
                bucket[index].m_Count    = 1;

                return true;
            }

            for (UInt64 i = 0; i < bucket[index].m_Count; i++)
                if (bucket[index].m_Data[i] == value)
                    return false;

            bucket[index].m_Data[bucket[index].m_Count++] = value;
            return true;
        }

        bool TryGet(const T& value, T& outValue) const
        {
            auto index = GetIndex(value);
            auto* bucket = (Bucket*) m_BucketHandle.m_Pointer;

            if (!bucket[index].m_Data)
                return false;

            for (UInt64 i = 0; i < bucket[index].m_Count; i++)
            {
                if (bucket[index].m_Data[i] == value)
                {
                    outValue = bucket[index].m_Data[i];
                    return true;
                }
            }

            return false;
        }

        bool TryRemove(const T& value)
        {
            auto index = GetIndex(value);
            auto* bucket = (Bucket*) m_BucketHandle.m_Pointer;

            if (!bucket[index].m_Data)
                return false;

            for (UInt64 i = 0; i < bucket[index].m_Count; i++)
            {
                if (bucket[index].m_Data[i] == value)
                {
                    for (UInt64 j = i; j < bucket[index].m_Count; j++)
                        bucket[index].m_Data[j] = bucket[index].m_Data[j + 1];

                    return true;
                }
            }

            return false;
        }

        bool Contains(const T& value) const
        {
            auto index = GetIndex(value);
            auto* bucket = (Bucket*) m_BucketHandle.m_Pointer;

            if (!bucket[index].m_Data)
                return false;

            for (UInt64 i = 0; i < bucket[index].m_Count; i++)
                if (bucket[index].m_Data[i] == value)
                    return true;

            return false;
        }

        OTR_INLINE void ClearDestructive()
        {
            if (m_BucketHandle.m_Pointer && m_BucketHandle.m_Size)
            {
                auto* bucket = (Bucket*) m_BucketHandle.m_Pointer;

                for (UInt64 i = 0; i < m_BucketHandle.m_Size; i++)
                    if (bucket[i].m_Data)
                        Unsafe::Delete(UnsafeHandle{ bucket[i].m_Data, bucket[i].m_Capacity });

                Unsafe::Delete(m_BucketHandle);
            }

            m_BucketHandle.m_Pointer = nullptr;
            m_BucketHandle.m_Size    = 0;
        }

    private:
//        const Int32 k_31BitMask = 0x7FFFFFFF;
        UInt64 m_TempSetCapacity    = 10;
        UInt64 m_TempBucketCapacity = 10;

        struct Bucket
        {
            T* m_Data;
            UInt64 m_Capacity;
            UInt64 m_Count;
        };

        UnsafeHandle m_BucketHandle{ };

        [[nodiscard]] OTR_INLINE UInt64 GetIndex(const T& item) const
        {
            return GetHashCode(item) % m_TempSetCapacity;
        }
    };
}

#endif //OTTERENGINE_HASHSET_H
