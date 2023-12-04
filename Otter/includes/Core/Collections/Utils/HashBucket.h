#ifndef OTTERENGINE_HASHBUCKET_H
#define OTTERENGINE_HASHBUCKET_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Memory.h"

namespace Otter
{
    template<typename T>
    struct BucketItem final
    {
    public:
        T      Data;
        UInt64 Hash;

        BucketItem() : Data(), Hash(0) { }
        ~BucketItem()
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
                Data.~T();

            Hash = 0;
        }

        BucketItem(const BucketItem<T>& other)
        {
            Data = other.Data;
            Hash = other.Hash;
        }

        BucketItem(BucketItem<T>&& other) noexcept
        {
            Data = std::move(other.Data);
            Hash = std::move(other.Hash);
        }

        BucketItem<T>& operator=(const BucketItem<T>& other)
        {
            if (this == &other)
                return *this;

            Data = other.Data;
            Hash = other.Hash;

            return *this;
        }

        BucketItem<T>& operator=(BucketItem<T>&& other) noexcept
        {
            if (this == &other)
                return *this;

            Data = std::move(other.Data);
            Hash = std::move(other.Hash);

            return *this;
        }
    };

    template<typename T>
    struct Bucket final
    {
    public:
        BucketItem<T>* Items = nullptr;
        UInt64 Capacity = 0;
        UInt64 Count    = 0;

        Bucket()
        {
            if (IsCreated())
                Buffer::Delete<BucketItem<T>>(Items, Capacity);
        }

        ~Bucket()
        {
            if (IsCreated())
                Buffer::Delete<BucketItem<T>>(Items, Capacity);
        }

        Bucket(const Bucket& other)
        {
            if (IsCreated())
                Buffer::Delete<BucketItem<T>>(Items, Capacity);

            Items    = other.Items;
            Capacity = other.Capacity;
            Count    = other.Count;
        }

        Bucket(Bucket&& other) noexcept
        {
            if (IsCreated())
                Buffer::Delete<BucketItem<T>>(Items, Capacity);

            Items    = std::move(other.Items);
            Capacity = std::move(other.Capacity);
            Count    = std::move(other.Count);

            other.Items    = nullptr;
            other.Capacity = 0;
            other.Count    = 0;
        }

        Bucket& operator=(const Bucket& other)
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Buffer::Delete<BucketItem<T>>(Items, Capacity);

            Items    = other.Items;
            Capacity = other.Capacity;
            Count    = other.Count;

            return *this;
        }

        Bucket& operator=(Bucket&& other) noexcept
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Buffer::Delete<BucketItem<T>>(Items, Capacity);

            Items    = std::move(other.Items);
            Capacity = std::move(other.Capacity);
            Count    = std::move(other.Count);

            other.Items    = nullptr;
            other.Capacity = 0;
            other.Count    = 0;

            return *this;
        }

        [[nodiscard]] OTR_INLINE constexpr bool IsCreated() const noexcept { return Items && Capacity > 0; }
        [[nodiscard]] OTR_INLINE constexpr bool IsEmpty() const noexcept { return Count == 0; }
    };
}

#endif //OTTERENGINE_HASHBUCKET_H
