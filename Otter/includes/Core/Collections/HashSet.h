#ifndef OTTERENGINE_HASHSET_H
#define OTTERENGINE_HASHSET_H

#include "Core/Function.h"
#include "Core/Collections/BitSet.h"
#include "Core/Collections/Utils/HashSlot.h"
#include "Core/Collections/Utils/HashUtils.h"
#include "Core/Collections/Iterators/SlotIterator.h"

#if !OTR_RUNTIME
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"
#endif

namespace Otter
{
    /**
     * @brief A collection of unique items that are stored in a contiguous block of memory and can be accessed by
     * their hash. The capacity of the hashset is automatically expanded to the next prime when the item count
     * reaches the capacity in order to maintain a low collision rate.
     *
     * @tparam T The type of the items in the hashset.
     */
    template<typename T>
    class HashSet final
    {
        /// @brief Alias for HashUtils.
        using HashUtils = Internal::HashUtils;

        /// @brief Alias for a slot.
        using Slot = Slot<T>;

        /// @brief Alias for a hashset iterator.
        using Iterator = SlotIterator<T>;

    public:
        /**
         * @brief Constructor.
         */
        HashSet() = default;

        /**
         * @brief Destructor.
         */
        ~HashSet()
        {
            if (IsCreated())
                Destroy();
        }

        /**
         * @brief Creates a hashset from an initialiser list.
         *
         * @param list The initialiser list.
         */
        HashSet(InitialiserList<T> list)
            : HashSet()
        {
            m_Capacity             = k_InitialCapacity;
            m_Slots                = Buffer::New < Slot > (m_Capacity);
            m_Count                = 0;
            m_CurrentMaxCollisions = 0;

            for (const T& item: list)
                TryAdd(item);
        }

        /**
         * @brief Copy constructor.
         *
         * @param other The hashset to copy.
         */
        HashSet(const HashSet<T>& other)
            : HashSet()
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
         * @param other The hashset to move.
         */
        HashSet(HashSet<T>&& other) noexcept
            : HashSet()
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
         * @param other The hashset to copy.
         *
         * @return A reference to this hashset.
         */
        HashSet<T>& operator=(const HashSet<T>& other)
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
         * @param other The hashset to move.
         *
         * @return A reference to this hashset.
         */
        HashSet<T>& operator=(HashSet<T>&& other) noexcept
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
         * @brief Tries to add an item to the hashset.
         *
         * @param item The item to add.
         *
         * @return True if the item was added, false otherwise.
         */
        bool TryAdd(const T& item)
        {
            if (m_Count >= m_Capacity || m_CurrentMaxCollisions >= k_MaxCollisions)
                Expand();

            UInt64 hash  = GetHashCode(item) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (!HasItemStoredAt(index))
                return TryAddToEmptySlot(item, hash, index);

            if (m_Slots[index].Matches(item, hash))
                return false;

            if (HasCollisionStoredAt(index))
                return TryAddToCollisionSlot(item, hash, index);

            return TryAddNewCollision(item, index, hash);
        }

        /**
         * @brief Tries to add an item to the hashset.
         *
         * @param item The item to add.
         *
         * @return True if the item was added, false otherwise.
         */
        bool TryAdd(T&& item) noexcept
        {
            if (m_Count >= m_Capacity || m_CurrentMaxCollisions >= k_MaxCollisions)
                Expand();

            UInt64 hash  = GetHashCode(item) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (!HasItemStoredAt(index))
                return TryAddToEmptySlot(std::move(item), hash, index);

            if (m_Slots[index].Matches(item, hash))
                return false;

            if (HasCollisionStoredAt(index))
                return TryAddToCollisionSlot(std::move(item), hash, index);

            return TryAddNewCollision(std::move(item), index, hash);
        }

        /**
         * @brief Tries to remove an item from the hashset.
         *
         * @param item The item to remove.
         *
         * @return True if the item was removed, false otherwise.
         */
        bool TryRemove(const T& item)
        {
            if (IsEmpty())
                return false;

            UInt64 index;
            if (!Exists(item, &index))
                return false;

            if (m_Slots[index].Next)
            {
                if constexpr (std::is_move_assignable_v<T>)
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
         * @brief Checks if the hashset contains a given item.
         *
         * @param item The item to check for.
         *
         * @return True if the hashset contains the item, false otherwise.
         */
        [[nodiscard]] bool Contains(const T& item) const
        {
            if (IsEmpty())
                return false;

            return Exists(item);
        }

        /**
         * @brief Tries to get the index of an item in the hashset.
         *
         * @param item The item to get the index of.
         * @param outIndex The index of the item.
         *
         * @return True if the item was found, false otherwise.
         */
        [[nodiscard]] bool TryGetIndex(const T& item, UInt64* outIndex) const
        {
            OTR_ASSERT_MSG(outIndex, "outIndex cannot be null.")

            if (IsEmpty())
                return false;

            return Exists(item, outIndex);
        }

        /**
         * @brief Performs a given callback on each item in the hashset.
         *
         * @param callback The callback to perform.
         */
        void ForEach(Function<void(const T&)> callback) const
        {
            if (IsEmpty())
                return;

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!HasItemStoredAt(i))
                    continue;

                callback(m_Slots[i].Data);
            }
        }

        /**
         * @brief Ensures that the hashset has a given capacity.
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
         * @brief Clears the hashset.
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
         * @brief Clears the hashset and deletes the data.
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
         * @brief Gets the memory footprint of the hashset.
         *
         * @param debugName The name of the hashset for debugging purposes.
         *
         * @return The memory footprint of the hashset.
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
         * @brief Gets the item capacity of the hashset.
         *
         * @return The capacity of the hashset.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetCapacity() const noexcept { return m_Capacity; }

        /**
         * @brief Gets the item count of the hashset.
         *
         * @return The item count of the hashset.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetCount() const noexcept { return m_Count; }

        /**
         * @brief Gets the default initial capacity of the hashset.
         *
         * @return The default initial capacity of the hashset.
         */
        [[nodiscard]] OTR_INLINE static constexpr UInt16 GetDefaultInitialCapacity() noexcept
        {
            return k_InitialCapacity;
        }

        /**
         * @brief Gets the resizing factor of the hashset.
         *
         * @return The resizing factor of the hashset.
         */
        [[nodiscard]] OTR_INLINE static constexpr Float16 GetResizingFactor() noexcept { return k_ResizingFactor; }

        /**
         * @brief Checks whether the hashset has been created. A hashset is created when it has been initialised
         * with a valid capacity and has not been destroyed.
         *
         * @return True if the hashset has been created, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsCreated() const noexcept { return m_Slots && m_Capacity > 0; }

        /**
         * @brief Checks whether the hashset is empty.
         *
         * @return True if the hashset is empty, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsEmpty() const noexcept { return m_Count == 0; }

        /**
         * @brief Gets a const iterator to the first element of the hash set.
         *
         * @return A const iterator to the first element of the hash set.
         */
        OTR_INLINE Iterator begin() const noexcept
        {
            return Iterator(m_Slots, m_Slots, m_Capacity, m_SlotsInUse);
        }

        /**
         * @brief Gets a const iterator to the last element of the hash set.
         *
         * @return A const iterator to the last element of the hash set.
         */
        OTR_INLINE Iterator end() const noexcept
        {
            return Iterator(m_Slots, m_Slots + m_Capacity - 1, m_Capacity, m_SlotsInUse);
        }

        /**
         * @brief Gets a reverse const iterator to the last element of the hash set.
         *
         * @return A reverse const iterator to the last element of the hash set.
         */
        OTR_INLINE Iterator rbegin() const noexcept
        {
            return Iterator(m_Slots, m_Slots + m_Capacity - 1, m_Capacity, m_SlotsInUse);
        }

        /**
         * @brief Gets a reverse const iterator to the first element of the hash set.
         *
         * @return A reverse const iterator to the first element of the hash set.
         */
        OTR_INLINE Iterator rend() const noexcept
        {
            return Iterator(m_Slots, m_Slots - 1, m_Capacity, m_SlotsInUse);
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
         * @brief Tries to add an item to an empty slot in the hashset.
         *
         * @param item The item to add.
         * @param hash The hash of the item.
         * @param index The index it should be added to.
         *
         * @return True if the item was added, false otherwise.
         *
         * @note This function assumes that the slot is empty, so it always returns true.
         */
        bool TryAddToEmptySlot(const T& item, const UInt64 hash, const UInt64 index)
        {
            m_Slots[index].Set(item, hash);
            m_SlotsInUse.Set(index, true);
            m_Collisions.Set(index, false);

            m_Count++;

            return true;
        }

        /**
         * @brief Tries to add an item to a slot that already has a collision stored in it.
         *
         * @param item The item to add.
         * @param hash The hash of the item.
         * @param index The index it should be added to.
         *
         * @return True if the item was added, false otherwise.
         *
         * @note This function assumes that the slot has a collision stored in it, so it always returns true. This is
         * a destructive operation, so the collision stored in the slot will be overwritten.
         * @note The function finds the original slot that the item in the collision slot collides with. It then removes
         * the collision from the linked list. The new item is then added to the collision slot (which is now empty) and
         * re-adds the original collision to the hashset.
         */
        bool TryAddToCollisionSlot(const T& item, const UInt64 hash, const UInt64 index)
        {
            auto collisionData = m_Slots[index].Data;
            auto* slot = &m_Slots[m_Slots[index].Hash % m_Capacity];

            while (slot->Next && slot->Next != &m_Slots[index])
                slot = slot->Next;

            if (slot->Next)
                slot->Next = slot->Next->Next;

            m_Slots[index].Set(item, hash);
            m_SlotsInUse.Set(index, true);
            m_Collisions.Set(index, false);

            return TryAdd(std::move(collisionData));
        }

        /**
         * @brief Tries to add a collision to the hashset.
         *
         * @param item The collision to add.
         * @param collisionIndex The index of the collision.
         * @param hash The hash of the collision.
         *
         * @return True if the collision was added, false otherwise.
         */
        bool TryAddNewCollision(const T& item, const UInt64 collisionIndex, const UInt64 hash)
        {
            auto* slot = &m_Slots[collisionIndex];
            auto collisionCount = 0;

            while (slot)
            {
                collisionCount++;

                if (HasItemStoredAt(slot - m_Slots) && slot->Matches(item, hash))
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

                m_Slots[i].Set(item, hash);
                m_SlotsInUse.Set(i, true);
                m_Collisions.Set(i, true);

                slot->Next = &m_Slots[i];

                m_Count++;

                return true;
            }

            return false;
        }

        /**
         * @brief Checks whether a slot has an item stored in it.
         *
         * @param index The index of the slot.
         *
         * @return True if the slot has an item stored in it, false otherwise.
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
         * @brief Checks an item's presence in the hashset.
         *
         * @param item The item to get the index of.
         * @param outIndex The index of the item.
         *
         * @return True if the item was found, false otherwise.
         *
         * @note It assumes the hashset is not empty.
         */
        bool Exists(const T& item, UInt64* outIndex = nullptr) const
        {
            UInt64 hash  = GetHashCode(item) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (!HasItemStoredAt(index))
                return false;

            auto* slot = &m_Slots[index];

            while (HasItemStoredAt(slot - m_Slots) && !slot->Matches(item, hash))
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
         * @brief Used to expand the size of the hashset.
         *
         * @param amount The amount to expand the hashset by.
         */
        void Expand(const UInt64 amount = 0)
        {
            UInt64 newCapacity = CalculateExpandCapacity(amount);

            if (IsEmpty())
            {
                RecreateEmpty(newCapacity);
                return;
            }

            HashSet<T> newHashSet;
            newHashSet.RecreateEmpty(newCapacity);

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!HasItemStoredAt(i))
                    continue;

                newHashSet.TryAdd(m_Slots[i].Data);
            }

            if (IsCreated())
                Destroy();

            m_Slots = Buffer::New < Slot > (newCapacity);

            for (UInt64 i = 0; i < newCapacity; i++)
                if (newHashSet.HasItemStoredAt(i))
                    m_Slots[i] = std::move(newHashSet.m_Slots[i]);

            m_Capacity             = newHashSet.m_Capacity;
            m_CurrentMaxCollisions = newHashSet.m_CurrentMaxCollisions;

            m_SlotsInUse = std::move(newHashSet.m_SlotsInUse);
            m_Collisions = std::move(newHashSet.m_Collisions);
        }

        /**
         * @brief Recreates the hashset with a given capacity. Deletes any existing data.
         *
         * @param capacity The capacity to recreate the hashset with.
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
         * @brief Calculates the new capacity when expanding the hashset.
         *
         * @param expandAmount The amount to expand the hashset by.
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
         * @brief Destroys the hashset.
         *
         * @note No checks are performed to see if the hashset has been created.
         */
        void Destroy()
        {
            Buffer::Delete<Slot>(m_Slots, m_Capacity);

            m_SlotsInUse.ClearDestructive();
            m_Collisions.ClearDestructive();
        }
    };
}

#endif //OTTERENGINE_HASHSET_H
