#ifndef OTTERENGINE_COLLECTION_H
#define OTTERENGINE_COLLECTION_H

#include "Core/Memory.h"

#if !OTR_RUNTIME
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"
#endif

namespace Otter
{
    template<typename T>
    class Collection;

    /**
     * @brief A static class that contains methods for creating collections.
     */
    class Collections final
    {
    public:
        /**
         * @brief Creates a new collection from a pointer to some data and their count.
         *
         * @tparam T The type of the collection.
         *
         * @param data The pointer to the data.
         * @param count The count of the data.
         * @param outCollection The collection to create.
         */
        template<typename T>
        static void New(const T* const data, const UInt64 count, Collection<T>& outCollection)
        {
            OTR_INTERNAL_ASSERT_MSG(data != nullptr, "Data cannot be null!")
            OTR_INTERNAL_ASSERT_MSG(count > 0, "Count must be greater than 0!")

            if (outCollection.m_Data && outCollection.m_Capacity > 0)
                Buffer::Delete<T>(outCollection.m_Data, outCollection.m_Capacity);

            outCollection.m_Data     = Buffer::New<T>(count);
            outCollection.m_Capacity = count;
            outCollection.m_Count    = 0;

            for (UInt64 i = 0; i < count; i++)
            {
                outCollection.m_Data[i] = data[i];
                outCollection.m_Count++;
            }
        }

        /**
         * @brief Creates a new collection from an initialiser list.
         *
         * @tparam T The type of the collection.
         *
         * @param list The initialiser list.
         *
         * @return The created collection.
         */
        template<typename T>
        static Collection<T> New(InitialiserList<T> list)
        {
            Collection<T> collection;
            collection.m_Capacity = list.size();
            collection.m_Data     = Buffer::New<T>(collection.m_Capacity);

            collection.m_Count = 0;
            for (const T& item: list)
                collection.m_Data[collection.m_Count++] = item;

            return collection;
        }

        /**
         * @brief Creates a new collection from an initialiser list.
         *
         * @tparam T The type of the collection.
         *
         * @param list The initialiser list.
         * @param outCollection The collection to create.
         */
        template<typename T>
        static void New(InitialiserList<T> list, Collection<T>& outCollection)
        {
            if (outCollection.m_Data && outCollection.m_Capacity > 0)
                Buffer::Delete<T>(outCollection.m_Data, outCollection.m_Capacity);

            outCollection.m_Capacity = list.size();
            outCollection.m_Data     = Buffer::New<T>(outCollection.m_Capacity);

            outCollection.m_Count = 0;
            for (const T& item: list)
                outCollection.m_Data[outCollection.m_Count++] = item;
        }

        /**
         * @brief Creates an empty collection.
         *
         * @tparam T The type of the collection.
         *
         * @return The created collection.
         */
        template<typename T>
        OTR_INLINE static Collection<T> Empty() { return Collection<T>(); }
    };

    /**
     * @brief A collection of items.
     *
     * @tparam T The type of the collection.
     */
    template<typename T>
    class Collection
    {
    public:
        /**
         * @brief Destructor.
         */
        ~Collection()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);
        }

        /**
         * @brief Used to reserve space for the collection.
         *
         * @param capacity The capacity to reserve.
         */
        void Reserve(const UInt64 capacity)
        {
            RecreateEmpty(capacity);
        }

        /**
         * @brief Used to expand the size of the collection by a given amount.
         *
         * @param amount The amount to expand the collection by.
         */
        void Expand(const UInt64 amount = 0)
        {
            UInt64 newCapacity = CalculateExpandCapacity(amount);

            T* newData = Buffer::New<T>(newCapacity);

            for (UInt64 i = 0; i < m_Count; i++)
                newData[i] = m_Data[i];

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data     = newData;
            m_Capacity = newCapacity;
        }

        /**
         * @brief Used to shrink the size of the collection by a given amount.
         *
         * @param amount The amount to shrink the collection by.
         * @param isDestructive Whether or not the shrink is destructive. If true, some data may be lost.
         */
        void Shrink(const UInt64 amount = 0, const bool isDestructive = false)
        {
            UInt64 newCapacity = CalculateShrinkCapacity(amount, isDestructive);

            if (IsEmpty() || newCapacity == 0)
            {
                RecreateEmpty(newCapacity);
                return;
            }

            T* newData = Buffer::New<T>(newCapacity);

            for (UInt64 i = 0; i < m_Count && i < amount; i++)
                newData[i] = m_Data[i];

            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data     = newData;
            m_Capacity = newCapacity;

            if (m_Count >= newCapacity)
                m_Count = newCapacity;
        }

        /**
         * @brief Checks if the collection contains a given item.
         *
         * @param item The item to check for.
         *
         * @return True if the collection contains the item, false otherwise.
         */
        [[nodiscard]] bool Contains(const T& item) const
        {
            for (UInt64 i = 0; i < m_Count; i++)
                if (m_Data[i] == item)
                    return true;

            return false;
        }

        /**
         * @brief Checks if the collection contains a given item.
         *
         * @param item The item to check for.
         *
         * @return True if the collection contains the item, false otherwise.
         */
        [[nodiscard]] bool Contains(T&& item) const noexcept
        {
            for (UInt64 i = 0; i < m_Count; i++)
                if (m_Data[i] == item)
                    return true;

            return false;
        }

        /**
         * @brief Tries to get the index of a given item.
         *
         * @param item The item to get the index of.
         * @param index The index of the item.
         *
         * @return True if the item was found, false otherwise.
         */
        [[nodiscard]] bool TryGetIndexOf(const T& item, UInt64& index) const
        {
            for (UInt64 i = 0; i < m_Count; i++)
                if (m_Data[i] == item)
                {
                    index = i;
                    return true;
                }

            return false;
        }

        /**
         * @brief Tries to get the index of a given item.
         *
         * @param item The item to get the index of.
         * @param index The index of the item.
         *
         * @return True if the item was found, false otherwise.
         */
        [[nodiscard]] bool TryGetIndexOf(T&& item, UInt64& index) const noexcept
        {
            for (UInt64 i = 0; i < m_Count; i++)
                if (m_Data[i] == item)
                {
                    index = i;
                    return true;
                }

            return false;
        }

        /**
         * @brief Clears the collection.
         */
        OTR_INLINE void Clear() { m_Count = 0; }

        /**
         * @brief Clears the collection and deletes the data.
         */
        void ClearDestructive()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data     = nullptr;
            m_Capacity = 0;
            m_Count    = 0;
        }

#if !OTR_RUNTIME
        /**
         * @brief Gets the memory footprint of the collection.
         *
         * @param debugName The name of the collection for debugging purposes.
         *
         * @return The memory footprint of the collection.
         */
        [[nodiscard]] ReadOnlySpan<MemoryFootprint, 1> GetMemoryFootprint(const char* const debugName) const
        {
            MemoryFootprint footprint = { };
            MemorySystem::CheckMemoryFootprint([&]()
                                               {
                                                   MemoryDebugPair pair[1];
                                                   pair[0] = { debugName, m_Data };

                                                   return MemoryDebugHandle{ pair, 1 };
                                               },
                                               &footprint,
                                               nullptr);

            return ReadOnlySpan<MemoryFootprint, 1>{ footprint };
        }
#endif

        /**
         * @brief Gets a pointer to the data of the collection.
         *
         * @return A pointer to the data of the collection.
         */
        [[nodiscard]] OTR_INLINE const T* GetData() const noexcept { return m_Data; }

        /**
         * @brief Gets the capacity of the collection.
         *
         * @return The capacity of the collection.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetCapacity() const noexcept { return m_Capacity; }

        /**
         * @brief Gets the item count of the collection.
         *
         * @return The item count of the collection.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetCount() const noexcept { return m_Count; }

        /**
         * @brief Checks whether the collection has been created. A collection is created when it has been initialised
         * with a valid capacity and has not been destroyed.
         *
         * @return True if the collection has been created, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsCreated() const noexcept { return m_Data && m_Capacity > 0; }

        /**
         * @brief Checks whether the collection is empty.
         *
         * @return True if the collection is empty, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsEmpty() const noexcept { return m_Count == 0; }

    protected:
        /**
         * @brief Constructor.
         */
        Collection()
            : m_Data(nullptr), m_Capacity(0), m_Count(0)
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);
        }

        T* m_Data;
        UInt64 m_Capacity;
        UInt64 m_Count;

        friend class Collections;

    private:
        /**
         * @brief Recreates the collection with a given capacity. Deletes any existing data.
         *
         * @param capacity The capacity to recreate the collection with.
         */
        void RecreateEmpty(const UInt64 capacity)
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Capacity);

            m_Data     = capacity > 0 ? Buffer::New<T>(capacity) : nullptr;
            m_Capacity = capacity;
            m_Count    = 0;
        }

        /**
         * @brief Calculates the new capacity when expanding the collection.
         *
         * @param expandAmount The amount to expand the collection by.
         *
         * @return The new capacity.
         */
        UInt64 CalculateExpandCapacity(const UInt64 expandAmount)
        {
            UInt64 newCapacity;

            if (expandAmount == 0)
                newCapacity = m_Capacity == 0 ? 2 : m_Capacity * 1.5;
            else
                newCapacity = m_Capacity + expandAmount;

            return newCapacity;
        }

        /**
         * @brief Calculates the new capacity when shrinking the collection.
         *
         * @param shrinkAmount The amount to shrink the collection by.
         * @param isDestructive Whether or not the shrink is destructive. If true, some data may be lost.
         *
         * @return The new capacity.
         */
        UInt64 CalculateShrinkCapacity(const UInt64 shrinkAmount, const bool isDestructive)
        {
            if (m_Capacity == 0)
                return 0;

            UInt64 newCapacity;

            if (shrinkAmount == 0)
                newCapacity = m_Capacity * 0.75;
            else if (shrinkAmount > m_Capacity)
                newCapacity = 0;
            else
                newCapacity = m_Capacity - shrinkAmount;

            const auto currentCount = GetCount();

            if (!isDestructive && newCapacity < currentCount)
                newCapacity = currentCount;

            return newCapacity;
        }
    };
}

#define OTR_COLLECTION_CHILD(Type) Type

#define OTR_COLLECTION_CONSTRUCT(Type)                                          \
    OTR_COLLECTION_CHILD(Type)() : Collection<T>() { }                          \
    ~OTR_COLLECTION_CHILD(Type)()                                               \
    {                                                                           \
        if (base::IsCreated())                                                  \
            Buffer::Delete<T>(base::m_Data, base::m_Capacity);                  \
                                                                                \
        base::m_Data     = nullptr;                                             \
        base::m_Capacity = 0;                                                   \
        base::m_Count    = 0;                                                   \
    }                                                                           \
                                                                                \
    OTR_COLLECTION_CHILD(Type)(InitialiserList<T> list)                         \
        : Collection<T>()                                                       \
    {                                                                           \
        base::m_Capacity = list.size();                                         \
        base::m_Data     = Buffer::New<T>(base::m_Capacity);                    \
                                                                                \
        base::m_Count = 0;                                                      \
        for (const T& item: list)                                               \
            base::m_Data[base::m_Count++] = item;                               \
    }

#define OTR_COLLECTION_COPY(Type)                                                           \
    OTR_COLLECTION_CHILD(Type)(const OTR_COLLECTION_CHILD(Type)<T>& other)                  \
        : Collection<T>()                                                                   \
    {                                                                                       \
        base::m_Capacity = other.m_Capacity;                                                \
        base::m_Count    = other.m_Count;                                                   \
        base::m_Data     = other.m_Data;                                                    \
    }                                                                                       \
                                                                                            \
    OTR_COLLECTION_CHILD(Type)<T>& operator=(const OTR_COLLECTION_CHILD(Type)<T>& other)    \
    {                                                                                       \
        if (this == &other)                                                                 \
            return *this;                                                                   \
                                                                                            \
        if (base::IsCreated())                                                              \
            Buffer::Delete<T>(base::m_Data, base::m_Capacity);                              \
                                                                                            \
        base::m_Capacity = other.m_Capacity;                                                \
        base::m_Count    = other.m_Count;                                                   \
        base::m_Data     = other.m_Data;                                                    \
                                                                                            \
        return *this;                                                                       \
    }

#define OTR_COLLECTION_MOVE(Type)                                                               \
    OTR_COLLECTION_CHILD(Type)(OTR_COLLECTION_CHILD(Type)<T>&& other) noexcept                  \
        : Collection<T>()                                                                       \
    {                                                                                           \
        base::m_Capacity = std::move(other.m_Capacity);                                         \
        base::m_Count    = std::move(other.m_Count);                                            \
        base::m_Data     = std::move(other.m_Data);                                             \
                                                                                                \
        other.m_Capacity = 0;                                                                   \
        other.m_Count    = 0;                                                                   \
        other.m_Data     = nullptr;                                                             \
    }                                                                                           \
                                                                                                \
    OTR_COLLECTION_CHILD(Type)<T>& operator=(OTR_COLLECTION_CHILD(Type)<T>&& other) noexcept    \
    {                                                                                           \
        if (this == &other)                                                                     \
            return *this;                                                                       \
                                                                                                \
        if (base::IsCreated())                                                                  \
            Buffer::Delete<T>(base::m_Data, base::m_Capacity);                                  \
                                                                                                \
        base::m_Capacity = std::move(other.m_Capacity);                                         \
        base::m_Count    = std::move(other.m_Count);                                            \
        base::m_Data     = std::move(other.m_Data);                                             \
                                                                                                \
        other.m_Capacity = 0;                                                                   \
        other.m_Count    = 0;                                                                   \
        other.m_Data     = nullptr;                                                             \
                                                                                                \
        return *this;                                                                           \
    }

#endif //OTTERENGINE_COLLECTION_H
