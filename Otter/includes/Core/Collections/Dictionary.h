#ifndef OTTERENGINE_DICTIONARY_H
#define OTTERENGINE_DICTIONARY_H

#include "Core/Function.h"
#include "Core/Collections/BitSet.h"
#include "Core/Collections/Utils/HashSlot.h"
#include "Core/Collections/Utils/HashUtils.h"
#include "Core/Collections/Utils/KeyValuePair.h"

#if !OTR_RUNTIME
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"
#endif

namespace Otter
{
    /**
     * @brief A collection of unique key/value pairs that are stored in a contiguous block of memory and can be
     * accessed by their keys' hash. The capacity of the dictionary is automatically expanded to the next prime when
     * the item count reaches the capacity in order to maintain a low collision rate.
     *
     * @tparam TKey The type of the keys.
     * @tparam TValue The type of the values.
     */
    template<typename TKey, typename TValue>
    class Dictionary final
    {
        /// @brief Alias for HashUtils.
        using HashUtils = Internal::HashUtils;

        /// @brief Alias for a KeyValuePair.
        using Pair = KeyValuePair<TKey, TValue>;

        /// @brief Alias for a slot.
        using Slot = Slot<Pair>;

        /// @brief Iterator for a collection of Slots.
        class SlotIterator;

    public:
        /**
         * @brief Constructor.
         */
        Dictionary() = default;

        /**
         * @brief Destructor.
         */
        ~Dictionary()
        {
            if (IsCreated())
                Destroy();
        }

        /**
         * @brief Creates a dictionary from an initialiser list.
         *
         * @param list The initialiser list.
         */
        Dictionary(InitialiserList<Pair> list)
            : Dictionary()
        {
            m_Capacity             = k_InitialCapacity;
            m_Slots                = Buffer::New<Slot>(k_InitialCapacity);
            m_Count                = 0;
            m_CurrentMaxCollisions = 0;

            for (const Pair& item: list)
                TryAdd(item.Key, item.Value);
        }

        /**
         * @brief Copy constructor.
         *
         * @param other The dictionary to copy.
         */
        Dictionary(const Dictionary<TKey, TValue>& other)
        {
            m_Capacity             = other.m_Capacity;
            m_Count                = other.m_Count;
            m_CurrentMaxCollisions = other.m_CurrentMaxCollisions;
            m_SlotsInUse           = other.m_SlotsInUse;
            m_Collisions           = other.m_Collisions;

            if (m_Capacity == 0)
                return;

            m_Slots = Buffer::New<Slot>(m_Capacity);

            if (m_Count > 0)
                MemorySystem::MemoryCopy(m_Slots, other.m_Slots, m_Capacity * sizeof(Slot));
        }

        /**
         * @brief Move constructor.
         *
         * @param other The dictionary to move.
         */
        Dictionary(Dictionary<TKey, TValue>&& other) noexcept
        {
            m_Slots                = std::move(other.m_Slots);
            m_Capacity             = std::move(other.m_Capacity);
            m_Count                = std::move(other.m_Count);
            m_CurrentMaxCollisions = std::move(other.m_CurrentMaxCollisions);
            m_SlotsInUse           = std::move(other.m_SlotsInUse);
            m_Collisions           = std::move(other.m_Collisions);

            other.m_Slots                = nullptr;
            other.m_Capacity             = 0;
            other.m_Count                = 0;
            other.m_CurrentMaxCollisions = 0;
        }

        /**
         * @brief Copy assignment operator.
         *
         * @param other The dictionary to copy.
         *
         * @return A reference to this dictionary.
         */
        Dictionary<TKey, TValue>& operator=(const Dictionary<TKey, TValue>& other)
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Destroy();

            m_Capacity             = other.m_Capacity;
            m_Count                = other.m_Count;
            m_CurrentMaxCollisions = other.m_CurrentMaxCollisions;
            m_SlotsInUse           = other.m_SlotsInUse;
            m_Collisions           = other.m_Collisions;

            if (m_Capacity == 0)
                return *this;

            m_Slots = Buffer::New<Slot>(m_Capacity);

            if (m_Count > 0)
                MemorySystem::MemoryCopy(m_Slots, other.m_Slots, m_Capacity * sizeof(Slot));

            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * @param other The dictionary to move.
         *
         * @return A reference to this dictionary.
         */
        Dictionary<TKey, TValue>& operator=(Dictionary<TKey, TValue>&& other) noexcept
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Destroy();

            m_Slots                = std::move(other.m_Slots);
            m_Capacity             = std::move(other.m_Capacity);
            m_Count                = std::move(other.m_Count);
            m_CurrentMaxCollisions = std::move(other.m_CurrentMaxCollisions);
            m_SlotsInUse           = std::move(other.m_SlotsInUse);
            m_Collisions           = std::move(other.m_Collisions);

            other.m_Slots                = nullptr;
            other.m_Capacity             = 0;
            other.m_Count                = 0;
            other.m_CurrentMaxCollisions = 0;

            return *this;
        }

        /**
         * @brief Equality operator.
         *
         * @param other The other dictionary.
         *
         * @return True if the hashsets are equal, false otherwise.
         */
        bool operator==(const Dictionary<TKey, TValue>& other) const noexcept
        {
            if (m_Count != other.m_Count)
                return false;

            for (UInt64 i = 0; i < m_Capacity; ++i)
                if (m_Slots[i] != other.m_Slots[i])
                    return false;

            return true;
        }

        /**
         * @brief Inequality operator.
         *
         * @param other The other dictionary.
         *
         * @return True if the hashsets are not equal, false otherwise.
         */
        bool operator!=(const Dictionary<TKey, TValue>& other) const noexcept { return !(*this == other); }

        /**
         * @brief Gets the element for the specified key.
         *
         * @param key The key.
         *
         * @return The address of the element for the specified key.
         */
        [[nodiscard]] TValue* operator[](const TKey key) const
        {
            if (IsEmpty())
                return nullptr;

            UInt64 index;
            if (!Exists(key, &index))
                return nullptr;

            return &m_Slots[index].Data.Value;
        }

        /**
         * @brief Tries to add a key/value pair to the dictionary.
         *
         * @param key The key of the pair.
         * @param value The value of the pair.
         *
         * @return True if the pair was added, false otherwise.
         */
        bool TryAdd(const TKey& key, const TValue& value)
        {
            if (m_Count >= m_Capacity || m_CurrentMaxCollisions >= k_MaxCollisions)
                Expand();

            UInt64 hash  = GetHashCode(key) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (!HasItemStoredAt(index))
                return TryAddToEmptySlot({ key, value }, hash, index);

            if (m_Slots[index].MatchesKey(key, hash))
            {
                m_Slots[index].Data.Value = value;
                return true;
            }

            if (HasCollisionStoredAt(index))
                return TryAddToCollisionSlot({ key, value }, hash, index);

            return TryAddNewCollision({ key, value }, index, hash);
        }

        /**
         * @brief Tries to add a key/value pair to the dictionary.
         *
         * @param key The key of the pair.
         * @param value The value of the pair.
         *
         * @return True if the pair was added, false otherwise.
         */
        bool TryAdd(TKey&& key, TValue&& value) noexcept
        {
            if (m_Count >= m_Capacity || m_CurrentMaxCollisions >= k_MaxCollisions)
                Expand();

            UInt64 hash  = GetHashCode(key) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (!HasItemStoredAt(index))
                return TryAddToEmptySlot(Pair(std::move(key), std::move(value)), hash, index);

            if (m_Slots[index].MatchesKey(key, hash))
            {
                m_Slots[index].Data.Value = std::move(value);
                return true;
            }

            if (HasCollisionStoredAt(index))
                return TryAddToCollisionSlot(Pair(std::move(key), std::move(value)), hash, index);

            return TryAddNewCollision(Pair(std::move(key), std::move(value)), index, hash);
        }

        /**
         * @brief Tries to get the value associated with the specified key.
         *
         * @param key The key of the value to get.
         * @param outValue The value associated with the key.
         *
         * @return True if the value was retrieved, false otherwise.
         */
        bool TryGet(const TKey& key, TValue* outValue) const
        {
            OTR_ASSERT(outValue, "outValue cannot be null.")

            if (IsEmpty())
                return false;

            UInt64 index;
            if (!Exists(key, &index))
                return false;

            *outValue = m_Slots[index].Data.Value;

            return true;
        }

        /**
         * @brief Tries to remove the value associated with the specified key.
         *
         * @param key The key of the value to remove.
         *
         * @return True if the value was removed, false otherwise.
         */
        bool TryRemove(const TKey& key)
        {
            if (IsEmpty())
                return false;

            UInt64 index;
            if (!Exists(key, &index))
                return false;

            if (m_Slots[index].Next)
            {
                if constexpr (std::is_move_assignable_v<Pair>)
                    m_Slots[index].Data = std::move(m_Slots[index].Next->Data);
                else
                    m_Slots[index].Data = m_Slots[index].Next->Data;

                m_Slots[index].Hash = m_Slots[index].Next->Hash;
                m_Slots[index].Next = m_Slots[index].Next->Next;
            }

            m_SlotsInUse.Set(index, false);
            m_Collisions.Set(index, false);

            m_Count--;

            return true;
        }

        /**
         * @brief Checks if the dictionary contains a given key.
         *
         * @param key The key of the item to check.
         *
         * @return True if the dictionary contains the key, false otherwise.
         */
        [[nodiscard]] bool ContainsKey(const TKey& key) const
        {
            if (IsEmpty())
                return false;

            return Exists(key);
        }

        /**
         * @brief Tries to get the index of a key/value pair in the dictionary.
         *
         * @param key The key of the pair to get the index of.
         * @param outIndex The index of the key/value pair.
         *
         * @return True if the key/value pair was found, false otherwise.
         */
        [[nodiscard]] bool TryGetIndex(const TKey& key, UInt64* outIndex) const
        {
            OTR_ASSERT(outIndex, "outIndex cannot be null.")

            if (IsEmpty())
                return false;

            return Exists(key, outIndex);
        }

        /**
         * @brief Performs a given callback on each key/value pair in the dictionary.
         *
         * @param callback The callback to perform.
         */
        void ForEach(Function<void(const TKey&, const TValue&)> callback) const
        {
            if (IsEmpty())
                return;

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!HasItemStoredAt(i))
                    continue;

                callback(m_Slots[i].Data.Key, m_Slots[i].Data.Value);
            }
        }

        /**
         * @brief Performs a given callback on each key/value pair in the dictionary.
         *
         * @param callback The callback to perform.
         */
        void ForEach(Function<void(const TKey&, TValue*)> callback) const
        {
            if (IsEmpty())
                return;

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!HasItemStoredAt(i))
                    continue;

                callback(m_Slots[i].Data.Key, &m_Slots[i].Data.Value);
            }
        }

        /**
         * @brief Performs a given callback on each key in the dictionary.
         *
         * @param key The key to perform the callback on.
         * @param callback The callback to perform.
         *
         * @return True if the key was found, false otherwise.
         */
        bool TryForKey(const TKey& key, Function<void(TValue&)> callback) const
        {
            if (IsEmpty())
                return false;

            UInt64 index;
            if (!Exists(key, &index))
                return false;

            callback(m_Slots[index].Data.Value);

            return true;
        }

        /**
         * @brief Performs a given callback on each key in the dictionary.
         *
         * @param callback The callback to perform.
         */
        void ForEachKey(Function<void(const TKey&)> callback) const
        {
            if (IsEmpty())
                return;

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!HasItemStoredAt(i))
                    continue;

                callback(m_Slots[i].Data.Key);
            }
        }

        /**
         * @brief Performs a given callback on each value in the dictionary.
         *
         * @param callback The callback to perform.
         */
        void ForEachValue(Function<void(const TValue&)> callback) const
        {
            if (IsEmpty())
                return;

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!HasItemStoredAt(i))
                    continue;

                callback(m_Slots[i].Data.Value);
            }
        }

        /**
         * @brief Performs a given callback on each value in the dictionary.
         *
         * @param callback The callback to perform.
         */
        void ForEachValue(Function<void(TValue*)> callback) const
        {
            if (IsEmpty())
                return;

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!HasItemStoredAt(i))
                    continue;

                callback(&m_Slots[i].Data.Value);
            }
        }

        /**
         * @brief Ensures that the dictionary has a given capacity.
         *
         * @param capacity The capacity to ensure.
         */
        void EnsureCapacity(const UInt64 capacity)
        {
            if (capacity <= m_Capacity)
                return;

            if (IsEmpty())
            {
                RecreateEmpty(HashUtils::GetNextPrime(capacity));
                return;
            }

            Expand(capacity - m_Capacity);
        }

        /**
         * @brief Clears the dictionary.
         */
        void Clear()
        {
            if (IsEmpty())
                return;

            m_SlotsInUse.Clear();
            m_Collisions.Clear();

            m_Count = 0;
        }

        /**
         * @brief Clears the dictionary and deletes the data.
         */
        void ClearDestructive()
        {
            if (IsCreated())
                Destroy();

            m_Slots    = nullptr;
            m_Capacity = 0;
            m_Count    = 0;
        }

#if !OTR_RUNTIME
        /**
         * @brief Gets the memory footprint of the dictionary.
         *
         * @param debugName The name of the dictionary for debugging purposes.
         *
         * @return The memory footprint of the dictionary.
         */
        [[nodiscard]] ReadOnlySpan<MemoryFootprint, 3> GetMemoryFootprint(const char* const debugName) const
        {
            MemoryFootprint footprint = { };
            MemorySystem::CheckMemoryFootprint([&]()
                                               {
                                                   MemoryDebugPair pair[1];
                                                   pair[0] = { debugName, m_Slots };

                                                   return MemoryDebugHandle{ pair, 1 };
                                               },
                                               &footprint,
                                               nullptr);

            auto slotsInUseFootprint = m_SlotsInUse.GetMemoryFootprint(OTR_NAME_OF(BitSet));
            auto collisionsFootprint = m_Collisions.GetMemoryFootprint(OTR_NAME_OF(BitSet));

            return ReadOnlySpan<MemoryFootprint, 3>{ footprint, slotsInUseFootprint[0], collisionsFootprint[0] };
        }
#endif

        /**
         * @brief Gets the item capacity of the dictionary.
         *
         * @return The capacity of the dictionary.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetCapacity() const noexcept { return m_Capacity; }

        /**
         * @brief Gets the item count of the dictionary.
         *
         * @return The item count of the dictionary.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetCount() const noexcept { return m_Count; }

        /**
         * @brief Gets the default initial capacity of the dictionary.
         *
         * @return The default initial capacity of the dictionary.
         */
        [[nodiscard]] OTR_INLINE static constexpr UInt16 GetDefaultInitialCapacity() noexcept
        {
            return k_InitialCapacity;
        }

        /**
         * @brief Gets the resizing factor of the dictionary.
         *
         * @return The resizing factor of the dictionary.
         */
        [[nodiscard]] OTR_INLINE static constexpr Float16 GetResizingFactor() noexcept { return k_ResizingFactor; }

        /**
         * @brief Checks whether the dictionary has been created. A dictionary is created when it has been initialised
         * with a valid capacity and has not been destroyed.
         *
         * @return True if the dictionary has been created, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsCreated() const noexcept { return m_Slots && m_Capacity > 0; }

        /**
         * @brief Checks whether the dictionary is empty.
         *
         * @return True if the dictionary is empty, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsEmpty() const noexcept { return m_Count == 0; }

        /**
         * @brief Gets a const iterator to the first element of the dictionary.
         *
         * @return A const iterator to the first element of the dictionary.
         */
        SlotIterator begin() const noexcept
        {
            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!HasItemStoredAt(i))
                    continue;

                return SlotIterator(m_Slots, m_Slots + i, m_Capacity, m_SlotsInUse);
            }

            return SlotIterator(m_Slots, m_Slots + m_Capacity, m_Capacity, m_SlotsInUse);
        }

        /**
         * @brief Gets a const iterator to the last element of the dictionary.
         *
         * @return A const iterator to the last element of the dictionary.
         */
        OTR_INLINE SlotIterator end() const noexcept
        {
            return SlotIterator(m_Slots, m_Slots + m_Capacity, m_Capacity, m_SlotsInUse);
        }

        /**
         * @brief Gets a reverse const iterator to the last element of the dictionary.
         *
         * @return A reverse const iterator to the last element of the dictionary.
         */
        SlotIterator rbegin() const noexcept
        {
            for (UInt64 i = m_Capacity - 1; i > 0; i--)
            {
                if (!HasItemStoredAt(i))
                    continue;

                return SlotIterator(m_Slots, m_Slots + i, m_Capacity, m_SlotsInUse);
            }

            return SlotIterator(m_Slots, m_Slots, m_Capacity, m_SlotsInUse);
        }

        /**
         * @brief Gets a reverse const iterator to the first element of the dictionary.
         *
         * @return A reverse const iterator to the first element of the dictionary.
         */
        OTR_INLINE SlotIterator rend() const noexcept
        {
            return SlotIterator(m_Slots, m_Slots, m_Capacity, m_SlotsInUse);
        }

    private:
        static constexpr Int64   k_63BitMask       = 0x7FFFFFFFFFFFFFFF;
        static constexpr UInt64  k_MaxCollisions   = 2;
        static constexpr UInt16  k_InitialCapacity = 3;
        static constexpr Float16 k_ResizingFactor  = static_cast<Float16>(1.5);

        Slot* m_Slots = nullptr;
        UInt64 m_Capacity             = 0;
        UInt64 m_Count                = 0;
        UInt64 m_CurrentMaxCollisions = 0;

        BitSet m_SlotsInUse{ };
        BitSet m_Collisions{ };

        /**
         * @brief Iterator for a collection of Slots.
         */
        struct SlotIterator final
        {
        public:
            /**
             * @brief Constructor.
             *
             * @param head The head of the Slot array.
             * @param ptr The current Slot.
             * @param capacity The capacity of the Slot array.
             * @param slotsInUse The BitSet of the Slots in use.
             */
            SlotIterator(Slot* head,
                         Slot* ptr,
                         const UInt64 capacity,
                         const BitSet& slotsInUse) // NOLINT(*-pass-by-value)
                : k_Head(head), k_Capacity(capacity), k_SlotsInUse(slotsInUse), m_Ptr(ptr)
            {
            }

            /**
             * @brief Increments the iterator.
             *
             * @return The incremented iterator.
             */
            OTR_INLINE SlotIterator& operator++()
            {
                m_Ptr++;

                if (!k_SlotsInUse.Get(m_Ptr - k_Head))
                    while (m_Ptr - k_Head < k_Capacity && !k_SlotsInUse.Get(m_Ptr - k_Head))
                        m_Ptr++;

                return *this;
            }

            /**
             * @brief Increments the iterator.
             *
             * @return The iterator before incrementation.
             */
            OTR_INLINE const SlotIterator operator++(int)
            {
                SlotIterator iterator = *this;
                ++(*this);
                return iterator;
            }

            /**
             * @brief Decrements the iterator.
             *
             * @return The decremented iterator.
             */
            OTR_INLINE SlotIterator& operator--()
            {
                m_Ptr--;

                if (!k_SlotsInUse.Get(m_Ptr - k_Head))
                    while (m_Ptr != k_Head && !k_SlotsInUse.Get(m_Ptr - k_Head))
                        m_Ptr--;

                return *this;
            }

            /**
             * @brief Decrements the iterator.
             *
             * @return The iterator before decrementation.
             */
            OTR_INLINE const SlotIterator operator--(int)
            {
                SlotIterator iterator = *this;
                --(*this);
                return iterator;
            }

            /**
             * @brief Dereferences the iterator.
             *
             * @return The dereferenced object.
             */
            OTR_INLINE KeyValuePair<const TKey, TValue>& operator*() const
            {
                return reinterpret_cast<KeyValuePair<const TKey, TValue>&>(m_Ptr->Data);
            }

            /**
             * @brief Equality operator.
             *
             * @param other The other iterator to compare to.
             *
             * @return True if the iterators are equal, false otherwise.
             */
            OTR_INLINE bool operator==(const SlotIterator& other) const { return m_Ptr == other.m_Ptr; }

            /**
             * @brief Inequality operator.
             *
             * @param other The other iterator to compare to.
             *
             * @return True if the iterators are not equal, false otherwise.
             */
            OTR_INLINE bool operator!=(const SlotIterator& other) const { return !(*this == other); }

        private:
            const Slot* const k_Head;
            const UInt64 k_Capacity;
            const BitSet k_SlotsInUse;

            Slot* m_Ptr;
        };

        /**
         * @brief Tries to add a key/value pair to an empty slot in the dictionary.
         *
         * @param pair The key/value pair to add.
         * @param hash The hash of the key.
         * @param index The index it should be added to.
         *
         * @return True if the key/value pair was added, false otherwise.
         *
         * @note This function assumes that the slot is empty, so it always returns true.
         */
        bool TryAddToEmptySlot(const Pair& pair, const UInt64 hash, const UInt64 index)
        {
            m_Slots[index].Set(pair, hash);
            m_SlotsInUse.Set(index, true);
            m_Collisions.Set(index, false);

            m_Count++;

            return true;
        }

        /**
         * @brief Tries to add a key/value pair to a slot that already has a collision stored in it.
         *
         * @param pair The key/value pair to add.
         * @param hash The hash of the key.
         * @param index The index it should be added to.
         *
         * @return True if the key/value pair was added, false otherwise.
         *
         * @note This function assumes that the slot has a collision stored in it, so it always returns true. This is
         * a destructive operation, so the collision stored in the slot will be overwritten.
         * @note The function finds the original slot that the key/value pair in the collision slot collides with.
         * It then removes the collision from the linked list. The new key/value pair is then added to the collision
         * slot (which is now empty) and re-adds the original collision to the dictionary.
         */
        bool TryAddToCollisionSlot(const Pair& pair, const UInt64 hash, const UInt64 index)
        {
            auto collisionData = m_Slots[index].Data;
            auto* slot = &m_Slots[m_Slots[index].Hash % m_Capacity];

            while (slot->Next && slot->Next != &m_Slots[index])
                slot = slot->Next;

            if (slot->Next)
                slot->Next = slot->Next->Next;

            m_Slots[index].Set(pair, hash);
            m_SlotsInUse.Set(index, true);
            m_Collisions.Set(index, false);

            return TryAdd(collisionData.Key, collisionData.Value);
        }

        /**
         * @brief Tries to add a collision to the dictionary.
         *
         * @param pair The collision to add.
         * @param collisionIndex The index of the collision.
         * @param hash The hash of the collision.
         *
         * @return True if the collision was added, false otherwise.
         */
        bool TryAddNewCollision(const Pair& pair, const UInt64 collisionIndex, const UInt64 hash)
        {
            auto* slot = &m_Slots[collisionIndex];
            auto collisionCount = 0;

            while (slot)
            {
                collisionCount++;

                if (HasItemStoredAt(slot - m_Slots) && slot->Matches(pair, hash))
                    return false;

                if (!slot->Next)
                    break;

                slot = slot->Next;
            }

            if (collisionCount > m_CurrentMaxCollisions)
                m_CurrentMaxCollisions = collisionCount;

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (HasItemStoredAt(i))
                    continue;

                m_Slots[i].Set(pair, hash);
                m_SlotsInUse.Set(i, true);
                m_Collisions.Set(i, true);

                slot->Next = &m_Slots[i];

                m_Count++;

                return true;
            }

            return false;
        }

        /**
         * @brief Checks whether a slot has a key/value pair stored in it.
         *
         * @param index The index of the slot.
         *
         * @return True if the slot has a key/value pair stored in it, false otherwise.
         */
        [[nodiscard]] bool HasItemStoredAt(const UInt64 index) const { return m_SlotsInUse.Get(index); }

        /**
         * @brief Checks whether a slot has a collision stored in it.
         *
         * @param index The index of the slot.
         *
         * @return True if the slot has a collision stored in it, false otherwise.
         */
        [[nodiscard]] bool HasCollisionStoredAt(const UInt64 index) const { return m_Collisions.Get(index); }

        /**
         * @brief Checks an key's presence in the dictionary.
         *
         * @param key The key to get the index of.
         * @param outIndex The index of the key.
         *
         * @return True if the key was found, false otherwise.
         *
         * @note It assumes the dictionary is not empty.
         */
        bool Exists(const TKey& key, UInt64* outIndex = nullptr) const
        {
            UInt64 hash  = GetHashCode(key) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (!HasItemStoredAt(index))
                return false;

            auto* slot = &m_Slots[index];

            while (HasItemStoredAt(slot - m_Slots) && !slot->MatchesKey(key, hash))
            {
                if (!slot->Next)
                    return false;

                slot = slot->Next;
            }

            if (outIndex)
                *outIndex = slot - m_Slots;

            return true;
        }

        /**
         * @brief Used to expand the size of the dictionary.
         *
         * @param amount The amount to expand the dictionary by.
         */
        void Expand(const UInt64 amount = 0)
        {
            UInt64 newCapacity = CalculateExpandCapacity(amount);

            if (IsEmpty())
            {
                RecreateEmpty(newCapacity);
                return;
            }

            Dictionary<TKey, TValue> newDictionary;
            newDictionary.RecreateEmpty(newCapacity);

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!HasItemStoredAt(i))
                    continue;

                newDictionary.TryAdd(m_Slots[i].Data.Key, m_Slots[i].Data.Value);
            }

            if (IsCreated())
                Destroy();

            m_Slots = Buffer::New<Slot>
                (newCapacity);

            for (UInt64 i = 0; i < newCapacity; i++)
                if (newDictionary.HasItemStoredAt(i))
                    m_Slots[i] = std::move(newDictionary.m_Slots[i]);

            m_Capacity             = newDictionary.m_Capacity;
            m_CurrentMaxCollisions = newDictionary.m_CurrentMaxCollisions;

            m_SlotsInUse = std::move(newDictionary.m_SlotsInUse);
            m_Collisions = std::move(newDictionary.m_Collisions);
        }

        /**
         * @brief Recreates the dictionary with a given capacity. Deletes any existing data.
         *
         * @param capacity The capacity to recreate the dictionary with.
         */
        void RecreateEmpty(const UInt64 capacity)
        {
            if (IsCreated())
                Destroy();

            m_Slots    = capacity > 0 ? Buffer::New<Slot>(capacity) : nullptr;
            m_Capacity = capacity;
            m_Count    = 0;

            if (capacity == 0)
                return;

            m_SlotsInUse.Reserve(capacity);
            m_Collisions.Reserve(capacity);
        }

        /**
         * @brief Calculates the new capacity when expanding the dictionary.
         *
         * @param expandAmount The amount to expand the dictionary by.
         *
         * @return The new capacity.
         */
        [[nodiscard]] UInt64 CalculateExpandCapacity(const UInt64 expandAmount) const
        {
            UInt64 newCapacity;

            if (expandAmount == 0)
                newCapacity = m_Capacity == 0
                              ? k_InitialCapacity
                              : HashUtils::GetNextPrime(m_Capacity * k_ResizingFactor);
            else
                newCapacity = HashUtils::GetNextPrime(m_Capacity + expandAmount);

            return newCapacity;
        }

        /**
         * @brief Destroys the dictionary.
         *
         * @note No checks are performed to see if the dictionary has been created.
         */
        void Destroy()
        {
            Buffer::Delete<Slot>(m_Slots, m_Capacity);
            m_Slots = nullptr;

            m_SlotsInUse.ClearDestructive();
            m_Collisions.ClearDestructive();
        }
    };
}

#endif //OTTERENGINE_DICTIONARY_H
