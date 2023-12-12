#ifndef OTTERENGINE_HASHBUCKET_H
#define OTTERENGINE_HASHBUCKET_H

#include "Core/Memory.h"

namespace Otter
{
    /**
     * @brief An object that represents an item stored in a hash bucket.
     *
     * @tparam T The type of the item.
     */
    template<typename T>
    struct BucketItem final
    {
    public:
        T      Data;
        UInt64 Hash;

        /**
         * @brief Constructor.
         */
        BucketItem() : Data(), Hash(0) { }

        /**
         * @brief Destructor.
         */
        ~BucketItem()
        {
            if constexpr (!std::is_trivially_destructible_v<T>)
                Data.~T();

            Hash = 0;
        }

        /**
         * @brief Copy constructor.
         *
         * @param other The bucket item to copy.
         */
        BucketItem(const BucketItem<T>& other)
        {
            Data = other.Data;
            Hash = other.Hash;
        }

        /**
         * @brief Move constructor.
         *
         * @param other The bucket item to move.
         */
        BucketItem(BucketItem<T>&& other) noexcept
        {
            Data = std::move(other.Data);
            Hash = std::move(other.Hash);
        }

        /**
         * @brief Copy assignment operator.
         *
         * @param other The bucket item to copy.
         *
         * @return A reference to this bucket item.
         */
        BucketItem<T>& operator=(const BucketItem<T>& other)
        {
            if (this == &other)
                return *this;

            Data = other.Data;
            Hash = other.Hash;

            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * @param other The bucket item to move.
         *
         * @return A reference to this bucket item.
         */
        BucketItem<T>& operator=(BucketItem<T>&& other) noexcept
        {
            if (this == &other)
                return *this;

            Data = std::move(other.Data);
            Hash = std::move(other.Hash);

            return *this;
        }
    };

    /**
     * @brief An object that represents a bucket in a hash table.
     *
     * @tparam T The type of the items in the bucket.
     */
    template<typename T>
    struct Bucket final
    {
    public:
        BucketItem<T>* Items = nullptr;
        UInt64 Capacity = 0;
        UInt64 Count    = 0;

        /**
         * @brief Constructor.
         */
        Bucket() = default;

        /**
         * @brief Destructor.
         */
        ~Bucket()
        {
            if (IsCreated())
                Buffer::Delete<BucketItem<T>>(Items, Capacity);
        }

        /**
         * @brief Copy constructor.
         *
         * @param other The bucket to copy.
         */
        Bucket(const Bucket& other)
        {
            if (IsCreated())
                Buffer::Delete<BucketItem<T>>(Items, Capacity);

            Items    = other.Items;
            Capacity = other.Capacity;
            Count    = other.Count;
        }

        /**
         * @brief Move constructor.
         *
         * @param other The bucket to move.
         */
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

        /**
         * @brief Copy assignment operator.
         *
         * @param other The bucket to copy.
         *
         * @return A reference to this bucket.
         */
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

        /**
         * @brief Move assignment operator.
         *
         * @param other The bucket to move.
         *
         * @return A reference to this bucket.
         */
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

        /**
         * @brief Creates the bucket. A bucket is considered created if it has a valid pointer to its items and a
         * capacity greater than 0.
         *
         * @return True if the bucket was created, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsCreated() const noexcept { return Items && Capacity > 0; }

        /**
         * @brief Checks if the bucket is empty.
         *
         * @return True if the bucket is empty, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsEmpty() const noexcept { return Count == 0; }
    };
}

#endif //OTTERENGINE_HASHBUCKET_H
