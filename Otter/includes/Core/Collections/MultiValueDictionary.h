#ifndef OTTERENGINE_MULTIVALUEDICTIONARY_H
#define OTTERENGINE_MULTIVALUEDICTIONARY_H

#include "Core/Function.h"
#include "Core/Collections/BitSet.h"
#include "Core/Collections/List.h"
#include "Core/Collections/Utils/HashSlot.h"
#include "Core/Collections/Utils/HashUtils.h"
#include "Core/Collections/Utils/KeyValuePair.h"
#include "Core/Collections/Iterators/SlotIterator.h"

#if !OTR_RUNTIME
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"
#endif

namespace Otter
{
    /**
     * @brief A collection of unique key/multi-value pairs that are stored in a contiguous block of memory and can be
     * accessed by their keys' hash. The capacity of the multi-value dictionary is automatically expanded to the
     * next prime when the item count reaches the capacity in order to maintain a low collision rate.
     *
     * @tparam TKey The type of the keys.
     * @tparam TValue The type of the values.
     */
    template<typename TKey, typename TValue>
    class MultiValueDictionary final
    {
        /// @brief Alias for HashUtils.
        using HashUtils = Internal::HashUtils;

        /// @brief Alias for a KeyValuePair.
        using KeyValuePair = KeyValuePair<TKey, List<TValue>>;

        /// @brief Alias for a slot.
        using Slot = Slot<KeyValuePair>;

        /// @brief Alias for a slot iterator.
        using SlotIterator = SlotIterator<KeyValuePair>;

    public:
        /**
         * @brief Constructor.
         */
        MultiValueDictionary() = default;

        /**
         * @brief Destructor.
         */
        ~MultiValueDictionary()
        {
            if (IsCreated())
                Destroy();
        }

        /**
         * @brief Creates a multi-value dictionary from an initialiser list.
         *
         * @param list The initialiser list.
         */
        MultiValueDictionary(InitialiserList<KeyValuePair> list)
            : MultiValueDictionary()
        {
            m_Capacity             = k_InitialCapacity;
            m_Slots                = Buffer::New < Slot > (m_Capacity);
            m_Count                = 0;
            m_CurrentMaxCollisions = 0;

            for (const KeyValuePair& item: list)
                TryAdd(item.Key, item.Value);
        }

        /**
         * @brief Copy constructor.
         *
         * @param other The multi-value dictionary to copy.
         */
        MultiValueDictionary(const MultiValueDictionary<TKey, TValue>& other)
            : MultiValueDictionary()
        {
            m_Slots                = other.m_Slots;
            m_Capacity             = other.m_Capacity;
            m_Count                = other.m_Count;
            m_CurrentMaxCollisions = other.m_CurrentMaxCollisions;
            m_SlotsInUse           = other.m_SlotsInUse;
            m_Collisions           = other.m_Collisions;
        }

        /**
         * @brief Move constructor.
         *
         * @param other The multi-value dictionary to move.
         */
        MultiValueDictionary(MultiValueDictionary<TKey, TValue>&& other) noexcept
            : MultiValueDictionary()
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
         * @param other The multi-value dictionary to copy.
         *
         * @return A reference to this multi-value dictionary.
         */
        MultiValueDictionary<TKey, TValue>& operator=(const MultiValueDictionary<TKey, TValue>& other)
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Destroy();

            m_Slots                = other.m_Slots;
            m_Capacity             = other.m_Capacity;
            m_Count                = other.m_Count;
            m_CurrentMaxCollisions = other.m_CurrentMaxCollisions;
            m_SlotsInUse           = other.m_SlotsInUse;
            m_Collisions           = other.m_Collisions;

            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * @param other The multi-value dictionary to move.
         *
         * @return A reference to this multi-value dictionary.
         */
        MultiValueDictionary<TKey, TValue>& operator=(MultiValueDictionary<TKey, TValue>&& other) noexcept
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
         * @brief Tries to add a key/multi-value pair to the dictionary.
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
                return TryAddToEmptySlot({ key, { value }}, hash, index);

            if (m_Slots[index].MatchesKey(key, hash))
                return TryAddToListOfValues(value, index);

            if (HasCollisionStoredAt(index))
                return TryAddToCollisionSlot({ key, { value }}, hash, index);

            return TryAddNewCollision({ key, { value }}, index, hash);
        }

        /**
         * @brief Tries to add a key/multi-value pair to the dictionary.
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
                return TryAddToEmptySlot({ key, { value }}, hash, index);

            if (m_Slots[index].MatchesKey(key, hash))
                return TryAddToListOfValues(value, index);

            if (HasCollisionStoredAt(index))
                return TryAddToCollisionSlot({ key, { value }}, hash, index);

            return TryAddNewCollision({ key, { value }}, index, hash);
        }

        /**
         * @brief Tries to get the values associated with the specified key.
         *
         * @param key The key of the value to get.
         * @param values The values associated with the key.
         *
         * @return True if the values were retrieved, false otherwise.
         */
        bool TryGet(const TKey& key, List<TValue>& values) const
        {
            if (IsEmpty())
                return false;

            UInt64 index;
            if (!Exists(key, &index))
                return false;

            values = m_Slots[index].Data.Value;

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
                if constexpr (std::is_move_assignable_v<KeyValuePair>)
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
        [[nodiscard]] bool Contains(const TKey& key) const
        {
            if (IsEmpty())
                return false;

            return Exists(key);
        }

        /**
         * @brief Tries to get the index of a key/multi-value pair in the dictionary.
         *
         * @param key The key of the pair to get the index of.
         * @param outIndex The index of the key/multi-value pair.
         *
         * @return True if the key/multi-value pair was found, false otherwise.
         */
        [[nodiscard]] bool TryGetIndex(const TKey& key, UInt64* outIndex) const
        {
            OTR_ASSERT_MSG(outIndex, "outIndex cannot be null.")

            if (IsEmpty())
                return false;

            return Exists(key, outIndex);
        }

        /**
         * @brief Performs a given callback on each key/multi-value pair in the dictionary.
         *
         * @param callback The callback to perform.
         */
        void ForEach(Function<void(const TKey&, List<TValue>)> callback) const
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
         * @brief Performs a given callback on each key in the multi-value dictionary.
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
         * @brief Performs a given callback on each value in the multi-value dictionary.
         *
         * @param callback The callback to perform.
         */
        void ForEachValue(Function<void(List<TValue>)> callback) const
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
         * @brief Ensures that the multi-value dictionary has a given capacity.
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
         * @brief Clears the multi-value dictionary.
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
         * @brief Clears the multi-value dictionary and deletes the data.
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
//        /**
//         * @brief Gets the memory footprint of the multi-value dictionary.
//         *
//         * @param debugName The name of the dictionary for debugging purposes.
//         *
//         * @return The memory footprint of the multi-value dictionary.
//         */
//        [[nodiscard]] ReadOnlySpan<MemoryFootprint, 3> GetMemoryFootprint(const char* const debugName) const
//        {
//            MemoryFootprint footprint = { };
//            MemorySystem::CheckMemoryFootprint([&]()
//                                               {
//                                                   MemoryDebugPair pair[1];
//                                                   pair[0] = { debugName, m_Slots };
//
//                                                   return MemoryDebugHandle{ pair, 1 };
//                                               },
//                                               &footprint,
//                                               nullptr);
//
//            auto slotsInUseFootprint = m_SlotsInUse.GetMemoryFootprint(OTR_NAME_OF(BitSet));
//            auto collisionsFootprint = m_Collisions.GetMemoryFootprint(OTR_NAME_OF(BitSet));
//
//            return ReadOnlySpan<MemoryFootprint, 3>{ footprint, slotsInUseFootprint[0], collisionsFootprint[0] };
//        }
#endif

        /**
         * @brief Gets the item capacity of the multi-value dictionary.
         *
         * @return The capacity of the multi-value dictionary.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetCapacity() const noexcept { return m_Capacity; }

        /**
         * @brief Gets the item count of the multi-value dictionary.
         *
         * @return The item count of the multi-value dictionary.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetCount() const noexcept { return m_Count; }

        /**
         * @brief Gets the default initial capacity of the multi-value dictionary.
         *
         * @return The default initial capacity of the multi-value dictionary.
         */
        [[nodiscard]] OTR_INLINE static constexpr UInt16 GetDefaultInitialCapacity() noexcept
        {
            return k_InitialCapacity;
        }

        /**
         * @brief Gets the resizing factor of the multi-value dictionary.
         *
         * @return The resizing factor of the multi-value dictionary.
         */
        [[nodiscard]] OTR_INLINE static constexpr Float16 GetResizingFactor() noexcept { return k_ResizingFactor; }

        /**
         * @brief Checks whether the multi-value dictionary has been created. A multi-value dictionary is created
         * when it has been initialised with a valid capacity and has not been destroyed.
         *
         * @return True if the multi-value dictionary has been created, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsCreated() const noexcept { return m_Slots && m_Capacity > 0; }

        /**
         * @brief Checks whether the multi-value dictionary is empty.
         *
         * @return True if the multi-value dictionary is empty, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsEmpty() const noexcept { return m_Count == 0; }

        /**
         * @brief Gets a const iterator to the first element of the multi-value dictionary.
         *
         * @return A const iterator to the first element of the multi-value dictionary.
         */
        OTR_INLINE SlotIterator cbegin() const noexcept
        {
            return SlotIterator(m_Slots,
                                m_Slots,
                                m_Capacity,
                                m_SlotsInUse);
        }

        /**
         * @brief Gets a const iterator to the last element of the multi-value dictionary.
         *
         * @return A const iterator to the last element of the multi-value dictionary.
         */
        OTR_INLINE SlotIterator cend() const noexcept
        {
            return SlotIterator(m_Slots,
                                m_Slots + m_Capacity - 1,
                                m_Capacity,
                                m_SlotsInUse);
        }

        /**
         * @brief Gets a reverse const iterator to the last element of the multi-value dictionary.
         *
         * @return A reverse const iterator to the last element of the multi-value dictionary.
         */
        OTR_INLINE SlotIterator crbegin() const noexcept
        {
            return SlotIterator(m_Slots,
                                m_Slots + m_Capacity - 1,
                                m_Capacity,
                                m_SlotsInUse);
        }

        /**
         * @brief Gets a reverse const iterator to the first element of the multi-value dictionary.
         *
         * @return A reverse const iterator to the first element of the multi-value dictionary.
         */
        OTR_INLINE SlotIterator crend() const noexcept
        {
            return SlotIterator(m_Slots,
                                m_Slots - 1,
                                m_Capacity,
                                m_SlotsInUse);
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
         * @brief Tries to add a key/multi-value pair to an empty slot in the dictionary.
         *
         * @param pair The key/multi-value pair to add.
         * @param hash The hash of the key.
         * @param index The index it should be added to.
         *
         * @return True if the key/multi-value pair was added, false otherwise.
         *
         * @note This function assumes that the slot is empty, so it always returns true.
         */
        bool TryAddToEmptySlot(const KeyValuePair& pair, const UInt64 hash, const UInt64 index)
        {
            m_Slots[index].Set(pair, hash);
            m_SlotsInUse.Set(index, true);
            m_Collisions.Set(index, false);

            m_Count++;

            return true;
        }

        /**
         * @brief Tries to add a value to a list of values in a slot.
         *
         * @param value The value to add.
         * @param index The index it should be added to.
         *
         * @return True if the value was added, false otherwise.
         *
         * @note Always returns true.
         */
        bool TryAddToListOfValues(const TValue& value, const UInt64 index)
        {
            m_Slots[index].Data.Value.Add(value);

            m_Count++;

            return true;
        }

        /**
         * @brief Tries to add a key/multi-value pair to a slot that already has a collision stored in it.
         *
         * @param pair The key/multi-value pair to add.
         * @param hash The hash of the key.
         * @param index The index it should be added to.
         *
         * @return True if the key/multi-value pair was added, false otherwise.
         *
         * @note This function assumes that the slot has a collision stored in it, so it always returns true. This is
         * a destructive operation, so the collision stored in the slot will be overwritten.
         * @note The function finds the original slot that the key/multi-value pair in the collision slot collides with.
         * It then removes the collision from the linked list. The new key/multi-value pair is then added to the collision
         * slot (which is now empty) and re-adds the original collision to the dictionary.
         */
        bool TryAddToCollisionSlot(const KeyValuePair& pair, const UInt64 hash, const UInt64 index)
        {
//            auto collisionData = m_Slots[index].Data;
//            auto* slot = &m_Slots[m_Slots[index].Hash % m_Capacity];
//
//            while (slot->Next && slot->Next != &m_Slots[index])
//                slot = slot->Next;
//
//            if (slot->Next)
//                slot->Next = slot->Next->Next;
//
//            m_Slots[index].Set(pair, hash);
//            m_SlotsInUse.Set(index, true);
//            m_Collisions.Set(index, false);
//
//            return TryAdd(collisionData.Key, collisionData.Value);

            return true;
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
        bool TryAddNewCollision(const KeyValuePair& pair, const UInt64 collisionIndex, const UInt64 hash)
        {
//            auto* slot = &m_Slots[collisionIndex];
//            auto collisionCount = 0;
//
//            while (slot)
//            {
//                collisionCount++;
//
//                if (HasItemStoredAt(slot - m_Slots) && slot->Matches(pair, hash))
//                    return false;
//
//                if (!slot->Next)
//                    break;
//
//                slot = slot->Next;
//            }
//
//            if (collisionCount > m_CurrentMaxCollisions)
//                m_CurrentMaxCollisions = collisionCount;
//
//            for (UInt64 i = 0; i < m_Capacity; i++)
//            {
//                if (HasItemStoredAt(i))
//                    continue;
//
//                m_Slots[i].Set(pair, hash);
//                m_SlotsInUse.Set(i, true);
//                m_Collisions.Set(i, true);
//
//                slot->Next = &m_Slots[i];
//
//                m_Count++;
//
//                return true;
//            }

            return false;
        }

        /**
         * @brief Checks whether a slot has a key/multi-value pair stored in it.
         *
         * @param index The index of the slot.
         *
         * @return True if the slot has a key/multi-value pair stored in it, false otherwise.
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
         * @brief Used to expand the size of the multi-value dictionary.
         *
         * @param amount The amount to expand the dictionary by.
         */
        void Expand(const UInt64 amount = 0)
        {
//            UInt64 newCapacity = CalculateExpandCapacity(amount);
//
//            if (IsEmpty())
//            {
//                RecreateEmpty(newCapacity);
//                return;
//            }
//
//            MultiValueDictionary<TKey, TValue> newDictionary;
//            newDictionary.RecreateEmpty(newCapacity);
//
//            for (UInt64 i = 0; i < m_Capacity; i++)
//            {
//                if (!HasItemStoredAt(i))
//                    continue;
//
//                newDictionary.TryAdd(m_Slots[i].Data.Key, m_Slots[i].Data.Value);
//            }
//
//            if (IsCreated())
//                Destroy();
//
//            m_Slots = Buffer::New < Slot > (newCapacity);
//
//            for (UInt64 i = 0; i < newCapacity; i++)
//                if (newDictionary.HasItemStoredAt(i))
//                    m_Slots[i] = std::move(newDictionary.m_Slots[i]);
//
//            m_Capacity             = newDictionary.m_Capacity;
//            m_CurrentMaxCollisions = newDictionary.m_CurrentMaxCollisions;
//
//            m_SlotsInUse = std::move(newDictionary.m_SlotsInUse);
//            m_Collisions = std::move(newDictionary.m_Collisions);
        }

        /**
         * @brief Recreates the multi-value dictionary with a given capacity. Deletes any existing data.
         *
         * @param capacity The capacity to recreate the multi-value dictionary with.
         */
        void RecreateEmpty(const UInt64 capacity)
        {
            if (IsCreated())
                Destroy();

            m_Slots    = capacity > 0 ? Buffer::New < Slot > (capacity) : nullptr;
            m_Capacity = capacity;
            m_Count    = 0;

            if (capacity == 0)
                return;

            m_SlotsInUse.Reserve(capacity);
            m_Collisions.Reserve(capacity);
        }

        /**
         * @brief Calculates the new capacity when expanding the multi-value dictionary.
         *
         * @param expandAmount The amount to expand the multi-value dictionary by.
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
         * @brief Destroys the multi-value dictionary.
         *
         * @note No checks are performed to see if the multi-value dictionary has been created.
         */
        void Destroy()
        {
            Buffer::Delete<Slot>(m_Slots, m_Capacity);

            m_SlotsInUse.ClearDestructive();
            m_Collisions.ClearDestructive();
        }
    };
}

#endif //OTTERENGINE_MULTIVALUEDICTIONARY_H
