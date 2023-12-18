#ifndef OTTERENGINE_HASHSET_H
#define OTTERENGINE_HASHSET_H

#include "Core/Function.h"
#include "Core/Collections/Utils/HashBucket.h"
#include "Core/Collections/Utils/HashUtils.h"

#if !OTR_RUNTIME
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"
#endif

namespace Otter
{
    /**
     * @brief A collection of unique items that are stored in buckets and can be accessed by their hash. The capacity
     * of the hashset is automatically expanded to the next prime when the item count reaches the capacity in order
     * to maintain a low collision rate.
     *
     * @tparam T The type of the items in the hashset.
     */
    template<typename T>
    class HashSet final
    {
        /// @brief Alias for HashUtils.
        using HashUtils = Internal::HashUtils;

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
                Buffer::Delete<Slot>(m_Slots, m_Capacity);
        }

        /**
         * @brief Creates a hashset from an initialiser list.
         *
         * @param list The initialiser list.
         */
        HashSet(InitialiserList<T> list)
            : HashSet()
        {
            m_Capacity = HashUtils::GetNextPrime(list.size());

            if (m_Capacity < k_InitialCapacity)
                m_Capacity = k_InitialCapacity;

            m_Slots                = Buffer::New<Slot>(m_Capacity);
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
                Buffer::Delete<Slot>(m_Slots, m_Capacity);

            m_Slots                = other.m_Slots;
            m_Capacity             = other.m_Capacity;
            m_Count                = other.m_Count;
            m_CurrentMaxCollisions = other.m_CurrentMaxCollisions;

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
                Buffer::Delete<Slot>(m_Slots, m_Capacity);

            m_Slots                = std::move(other.m_Slots);
            m_Capacity             = std::move(other.m_Capacity);
            m_Count                = std::move(other.m_Count);
            m_CurrentMaxCollisions = std::move(other.m_CurrentMaxCollisions);

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
        bool TryAdd(const T& value)
        {
            if (m_Count >= m_Capacity || m_CurrentMaxCollisions >= k_MaxCollisions)
                Expand();

            UInt64 hash  = GetHashCode(value) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (!m_Slots[index].HasData)
            {
                m_Slots[index].Data        = value;
                m_Slots[index].Hash        = hash;
                m_Slots[index].HasData     = true;
                m_Slots[index].IsCollision = false;
                m_Slots[index].Collision   = nullptr;

                m_Count++;

                return true;
            }

            if (m_Slots[index].Data == value && m_Slots[index].Hash == hash)
                return false;

            return TryAddCollision(value, index, hash);
        }

        /**
         * @brief Tries to add an item to the hashset.
         *
         * @param item The item to add.
         *
         * @return True if the item was added, false otherwise.
         */
        bool TryAdd(T&& value) noexcept
        {
            if (m_Count >= m_Capacity || m_CurrentMaxCollisions >= k_MaxCollisions)
                Expand();

            UInt64 hash  = GetHashCode(value) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (!m_Slots[index].HasData)
            {
                m_Slots[index].Data        = std::move(value);
                m_Slots[index].Hash        = hash;
                m_Slots[index].HasData     = true;
                m_Slots[index].IsCollision = false;
                m_Slots[index].Collision   = nullptr;

                m_Count++;

                return true;
            }

            if (m_Slots[index].Data == value && m_Slots[index].Hash == hash)
                return false;

            return TryAddCollision(std::move(value), index, hash);
        }

        /**
         * @brief Tries to remove an item from the hashset.
         *
         * @param item The item to remove.
         *
         * @return True if the item was removed, false otherwise.
         */
        bool TryRemove(const T& value)
        {
            UInt64 hash  = GetHashCode(value) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (!m_Slots[index].HasData)
                return false;

            auto* slot = &m_Slots[index];

            while (slot->HasData && slot->Data != value)
            {
                if (!slot->Collision)
                    return false;

                slot = slot->Collision;
            }

            if (slot->HasData && slot->Data == value && slot->Hash == hash)
            {
                if (slot->Collision)
                {
                    if constexpr (std::is_move_assignable_v<T>)
                        slot->Data = std::move(slot->Collision->Data);
                    else
                        slot->Data = slot->Collision->Data;

                    slot->Hash      = slot->Collision->Hash;
                    slot->Collision = slot->Collision->Collision;

                    m_Count--;

                    return true;
                }

                slot->HasData     = false;
                slot->IsCollision = false;
                m_Count--;

                return true;
            }

            return false;
        }

        /**
         * @brief Checks if the hashset contains a given item.
         *
         * @param item The item to check for.
         *
         * @return True if the hashset contains the item, false otherwise.
         */
        [[nodiscard]] bool Contains(const T& value) const
        {
            UInt64 hash  = GetHashCode(value) & k_63BitMask;
            UInt64 index = hash % m_Capacity;

            if (!m_Slots[index].HasData)
                return false;

            auto* slot = &m_Slots[index];

            while (slot->HasData && slot->Data != value)
            {
                if (!slot->Collision)
                    return false;

                slot = slot->Collision;
            }

            if (slot->HasData && slot->Data == value && slot->Hash == hash)
                return true;

            return false;
        }

        /**
         * @brief Performs a given callback on each item in the hashset.
         *
         * @param callback The callback to perform.
         */
        void ForEach(Function<void(const T&)> callback) const
        {
            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Slots[i].HasData)
                    continue;

                callback(m_Slots[i].Data);
            }
        }

        /**
         * @brief Clears the hashset.
         */
        void Clear()
        {
            if (!IsCreated())
                return;

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Slots[i].HasData)
                    continue;

                m_Slots[i].HasData     = false;
                m_Slots[i].IsCollision = false;
            }

            m_Count = 0;
        }

        /**
         * @brief Clears the hashset and deletes the data.
         */
        void ClearDestructive()
        {
            if (IsCreated())
                Buffer::Delete<Slot>(m_Slots, m_Capacity);

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
        [[nodiscard]] ReadOnlySpan<MemoryFootprint, 1> GetMemoryFootprint(const char* const debugName) const
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

            return ReadOnlySpan<MemoryFootprint, 1>{ footprint };
        }
#endif

        /**
         * @brief Gets the item count of the hashset.
         *
         * @return The item count of the hashset.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetCount() const noexcept { return m_Count; }

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

    private:
        struct Slot final
        {
        public:
            T      Data;
            UInt64 Hash;

            bool HasData;
            bool IsCollision;

            Slot* Collision;
        };

        static constexpr Int64   k_63BitMask       = 0x7FFFFFFFFFFFFFFF;
        static constexpr UInt64  k_MaxCollisions   = 2;
        static constexpr UInt16  k_InitialCapacity = 3;
        static constexpr Float16 k_ResizingFactor  = static_cast<Float16>(1.5);

        Slot* m_Slots = nullptr;
        UInt64 m_Capacity             = 0;
        UInt64 m_Count                = 0;
        UInt64 m_CurrentMaxCollisions = 0;

        /**
         * @brief Used to expand the size of the hashset.
         */
        void Expand()
        {
            UInt64 newCapacity = m_Capacity == 0
                                 ? k_InitialCapacity
                                 : HashUtils::GetNextPrime(m_Capacity * k_ResizingFactor);
            Slot* newSlots = Buffer::New<Slot>(newCapacity);
            m_CurrentMaxCollisions = 0;

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Slots[i].HasData || m_Slots[i].IsCollision)
                    continue;

                UInt64 hash  = m_Slots[i].Hash & k_63BitMask;
                UInt64 index = hash % newCapacity;

                if (!newSlots[index].HasData)
                {
                    newSlots[index].Data        = m_Slots[i].Data;
                    newSlots[index].Hash        = hash;
                    newSlots[index].HasData     = true;
                    newSlots[index].IsCollision = false;
                    newSlots[index].Collision   = nullptr;
                }
            }

            UInt64 currentEmptySlot = 0;

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (!m_Slots[i].IsCollision)
                    continue;

                UInt64 hash  = m_Slots[i].Hash & k_63BitMask;
                UInt64 index = hash % newCapacity;

                if (!newSlots[index].HasData)
                {
                    newSlots[index].Data        = m_Slots[i].Data;
                    newSlots[index].Hash        = hash;
                    newSlots[index].HasData     = true;
                    newSlots[index].IsCollision = false;
                    newSlots[index].Collision   = nullptr;

                    continue;
                }

                auto* slot = &newSlots[index];
                auto collisionCount = 0;

                while (slot)
                {
                    collisionCount++;

                    if (!slot->Collision)
                        break;

                    slot = slot->Collision;
                }

                if (collisionCount > m_CurrentMaxCollisions)
                    m_CurrentMaxCollisions = collisionCount;

                while (currentEmptySlot < newCapacity)
                {
                    if (newSlots[currentEmptySlot].HasData)
                    {
                        currentEmptySlot++;
                        continue;
                    }

                    newSlots[currentEmptySlot].Data        = m_Slots[i].Data;
                    newSlots[currentEmptySlot].Hash        = hash;
                    newSlots[currentEmptySlot].HasData     = true;
                    newSlots[currentEmptySlot].IsCollision = true;
                    newSlots[currentEmptySlot].Collision   = nullptr;

                    slot->Collision = &newSlots[currentEmptySlot];
                }
            }

            if (IsCreated())
                Buffer::Delete<Slot>(m_Slots, m_Capacity);

            m_Slots    = newSlots;
            m_Capacity = newCapacity;
        }

        /**
         * @brief Checks whether a slot is available. A slot is available if it does not have data or if it has data
         * and is a collision.
         */
        [[nodiscard]] bool IsAvailable(const UInt64 index) const noexcept
        {
            return !m_Slots[index].HasData || HasCollisionData(index);
        }

        /**
         * @brief Checks whether a slot is a collision.
         */
        [[nodiscard]] bool HasCollisionData(const UInt64 index) const noexcept
        {
            return m_Slots[index].HasData && m_Slots[index].IsCollision;
        }

        /**
         * @brief Tries to add an collision to the hashset.
         *
         * @param item The collision to add.
         * @param collisionIndex The index of the collision.
         * @param hash The hash of the collision.
         *
         * @return True if the collision was added, false otherwise.
         */
        bool TryAddCollision(const T& value, const UInt64 collisionIndex, const UInt64 hash)
        {
            auto* slot = &m_Slots[collisionIndex];
            auto collisionCount = 0;

            while (slot)
            {
                collisionCount++;

                if (slot->HasData && slot->Data == value && slot->Hash == hash)
                    return false;

                if (!slot->Collision)
                    break;

                slot = slot->Collision;
            }

            if (collisionCount > m_CurrentMaxCollisions)
                m_CurrentMaxCollisions = collisionCount;

            for (UInt64 i = 0; i < m_Capacity; i++)
            {
                if (m_Slots[i].HasData)
                    continue;

                m_Slots[i].Data        = value;
                m_Slots[i].Hash        = hash;
                m_Slots[i].HasData     = true;
                m_Slots[i].IsCollision = true;
                m_Slots[i].Collision   = nullptr;

                slot->Collision = &m_Slots[i];
                m_Count++;

                return true;
            }

            return false;
        }
    };
}

#endif //OTTERENGINE_HASHSET_H
