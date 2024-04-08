#ifndef OTTERENGINE_COMPONENTDATA_H
#define OTTERENGINE_COMPONENTDATA_H

#include "Core/Memory.h"
#include "Components/IComponent.h"

namespace Otter
{
    /**
     * @brief Structure for storing component data.
     */
    struct ComponentData final
    {
    private:
        struct DataIterator;

    public:
        /**
         * @brief Constructor.
         */
        ComponentData() = default;

        /**
         * @brief Destructor.
         */
        ~ComponentData()
        {
            if (m_Capacity == 0)
                return;

            Destroy();
        }

        /**
         * @brief Copy constructor.
         */
        ComponentData(const ComponentData& other)
        {
            Copy(other);
        }

        /**
         * @brief Move constructor.
         */
        ComponentData(ComponentData&& other) noexcept
        {
            Move(std::move(other));
        }

        /**
         * @brief Copy assignment operator.
         */
        ComponentData& operator=(const ComponentData& other)
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Destroy();

            Copy(other);

            return *this;
        }

        /**
         * @brief Move assignment operator.
         */
        ComponentData& operator=(ComponentData&& other) noexcept
        {
            if (this == &other)
                return *this;

            if (IsCreated())
                Destroy();

            Move(std::move(other));

            return *this;
        }

        /**
         * @brief Equality operator.
         *
         * @param other The other data.
         *
         * @return True if the data are equal, false otherwise.
         */
        bool operator==(const ComponentData& other) const noexcept
        {
            if (m_Count != other.m_Count
                || m_Capacity != other.m_Capacity
                || m_BytesStored != other.m_BytesStored
                || m_ComponentData.Size != other.m_ComponentData.Size)
                return false;

            if (m_Count == 0)
                return true;

            // TODO: Create a Platform::MemoryCompare function to help with this
            for (UInt64 i = 0; i < m_Count; ++i)
                if (m_ComponentIds[i] != other.m_ComponentIds[i]
                    || m_ComponentSizes[i] != other.m_ComponentSizes[i])
                    return false;

            for (UInt64 i = 0; i < m_Count; ++i)
                if (((Byte*) m_ComponentData.Pointer)[i] != ((Byte*) other.m_ComponentData.Pointer)[i])
                    return false;

            return true;
        }

        /**
         * @brief Inequality operator.
         *
         * @param other The other data.
         *
         * @return True if the data are not equal, false otherwise.
         */
        bool operator!=(const ComponentData& other) const noexcept { return !(*this == other); }

        /**
         * @brief Stores component data in the container.
         *
         * @param componentId The component's Id.
         * @param componentSize The component's size.
         * @param componentData The component's data in bytes.
         */
        void Add(const ComponentId componentId, const UInt64 componentSize, const Byte* const componentData)
        {
            OTR_DEBUG_BLOCK(
                for (UInt64 i = 0; i < m_Count; ++i)
                {
                    OTR_ASSERT(componentId != m_ComponentIds[i], "Component Id already exists in the container.")
                }
            )

            if (m_Count == m_Capacity)
                Expand();

            m_ComponentIds[m_Count]   = componentId;
            m_ComponentSizes[m_Count] = componentSize;

            if (m_BytesStored + componentSize > m_ComponentData.Size)
                m_ComponentData = MemorySystem::Reallocate(m_ComponentData, k_ResizingFactor * m_ComponentData.Size);

            MemorySystem::MemoryCopy(((Byte*) m_ComponentData.Pointer) + m_BytesStored, componentData, componentSize);
            m_BytesStored += componentSize;

            ++m_Count;
        }

        /**
         * @brief Removes component data from the container.
         *
         * @param componentId The component's Id.
         */
        void Remove(const ComponentId componentId)
        {
            if (m_Count == 0)
                return;

            for (UInt64 i = 0; i < m_Count; i++)
            {
                if (m_ComponentIds[i] != componentId)
                    continue;

                m_BytesStored -= m_ComponentSizes[i];
                --m_Count;

                if (i == m_Count)
                    return;

                UInt64 bytesStoredAfter = 0;

                for (UInt64 j = i; j < m_Count; ++j)
                    bytesStoredAfter += m_ComponentSizes[j];

                MemorySystem::MemoryCopy(((Byte*) m_ComponentData.Pointer) + m_BytesStored - bytesStoredAfter,
                                         ((Byte*) m_ComponentData.Pointer) + m_BytesStored,
                                         bytesStoredAfter);
                MemorySystem::MemoryCopy(m_ComponentSizes + i,
                                         m_ComponentSizes + i + 1,
                                         (m_Count - i) * sizeof(UInt64));
                MemorySystem::MemoryCopy(m_ComponentIds + i,
                                         m_ComponentIds + i + 1,
                                         (m_Count - i) * sizeof(ComponentId));

                return;
            }
        }

        /**
         * @brief Gets a pointer to the component ids of the container.
         *
         * @return A pointer to the component ids of the container.
         */
        [[nodiscard]] OTR_INLINE const ComponentId* GetComponentIds() const noexcept { return m_ComponentIds; }

        /**
         * @brief Gets a pointer to the component sizes of the container.
         *
         * @return A pointer to the component sizes of the container.
         */
        [[nodiscard]] OTR_INLINE const UInt64* GetComponentSizes() const noexcept { return m_ComponentSizes; }

        /**
         * @brief Gets a pointer to the component data of the container.
         *
         * @return A pointer to the component data of the container.
         */
        [[nodiscard]] OTR_INLINE const Byte* GetComponentData() const noexcept
        {
            return (Byte*) m_ComponentData.Pointer;
        }

        /**
         * @brief Gets the component count of the container.
         *
         * @return The component count of the container.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetCount() const noexcept { return m_Count; }

        /**
         * @brief Gets a const iterator to the first component of the container.
         *
         * @return A const iterator to the first component of the container.
         */
        [[nodiscard]] OTR_INLINE DataIterator begin() const noexcept
        {
            return { m_ComponentIds, m_ComponentSizes, (Byte*) m_ComponentData.Pointer };
        }

        /**
         * @brief Gets a const iterator to the last component of the container.
         *
         * @return A const iterator to the last component of the container.
         */
        [[nodiscard]] OTR_INLINE DataIterator end() const noexcept
        {
            return {
                m_ComponentIds + m_Count,
                m_ComponentSizes + m_Count,
                (Byte*) m_ComponentData.Pointer + m_BytesStored
            };
        }

    private:
        static constexpr UInt64 k_DefaultCapacity     = 4;
        static constexpr UInt64 k_DefaultDataCapacity = sizeof(UInt64);
        static constexpr UInt16 k_ResizingFactor      = 2;

        ComponentId* m_ComponentIds   = nullptr;
        UInt64     * m_ComponentSizes = nullptr;
        UInt64 m_Count    = 0;
        UInt64 m_Capacity = 0;

        UnsafeHandle m_ComponentData{ };
        UInt64       m_BytesStored = 0;

        /**
         * @brief A linear iterator for the component data container.
         */
        struct DataIterator final
        {
        public:
            /**
             * @brief Constructor.
             */
            DataIterator(ComponentId* const componentIds,
                         UInt64* const componentSizes,
                         Byte* const componentData)
                : m_ComponentIds(componentIds), m_ComponentSizes(componentSizes), m_ComponentData(componentData)
            {
            }

            /**
             * @brief Increments the iterator.
             *
             * @return The incremented iterator.
             */
            OTR_INLINE DataIterator& operator++()
            {
                m_ComponentIds++;
                m_ComponentSizes++;
                m_ComponentData += *m_ComponentSizes;
                return *this;
            }

            /**
             * @brief Increments the iterator.
             *
             * @return The iterator before incrementation.
             */
            OTR_INLINE const DataIterator operator++(int)
            {
                DataIterator iterator = *this;
                ++(*this);
                return iterator;
            }

            /**
             * @brief Dereferences the iterator.
             *
             * @return The component data as a tuple.
             */
            OTR_INLINE std::tuple<const ComponentId, const UInt64, const Byte* const> operator*() const
            {
                return { *m_ComponentIds, *m_ComponentSizes, m_ComponentData };
            }

            /**
             * @brief Equality operator.
             *
             * @param other The other iterator to compare to.
             *
             * @return True if the iterators are equal, false otherwise.
             */
            OTR_INLINE bool operator==(const DataIterator& other) const
            {
                if (m_ComponentIds != other.m_ComponentIds || m_ComponentSizes != other.m_ComponentSizes)
                    return false;

                // TODO: Create a Platform::MemoryCompare function to help with this
                for (UInt64 i = 0; i < *m_ComponentSizes; i++)
                    if (m_ComponentData[i] != other.m_ComponentData[i])
                        return false;

                return true;
            }

            /**
             * @brief Inequality operator.
             *
             * @param other The other iterator to compare to.
             *
             * @return True if the iterators are not equal, false otherwise.
             */
            OTR_INLINE bool operator!=(const DataIterator& other) const { return !(*this == other); }

        private:
            ComponentId* m_ComponentIds;
            UInt64     * m_ComponentSizes;
            Byte       * m_ComponentData;
        };

    OTR_TEST_VISIBLE:
        /**
         * @brief Checks whether the component data container has been created.
         *
         * @return True if the component data container has been created, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsCreated() const noexcept
        {
            return m_ComponentIds
                   && m_ComponentSizes
                   && m_Capacity > 0
                   && m_ComponentData.Pointer
                   && m_ComponentData.Size > 0;
        }

        /**
         * @brief Used to copy another container to this.
         *
         * @param other The other container to copy.
         */
        void Copy(const ComponentData& other)
        {
            m_Count       = other.m_Count;
            m_Capacity    = other.m_Capacity;
            m_BytesStored = other.m_BytesStored;

            if (m_Capacity == 0)
                return;

            m_ComponentIds   = Buffer::New<ComponentId>(m_Capacity);
            m_ComponentSizes = Buffer::New<UInt64>(m_Capacity);
            m_ComponentData  = Unsafe::New(other.m_ComponentData.Size);

            if (m_Count == 0)
                return;

            MemorySystem::MemoryCopy(m_ComponentIds, other.m_ComponentIds, m_Count * sizeof(ComponentId));
            MemorySystem::MemoryCopy(m_ComponentSizes, other.m_ComponentSizes, m_Count * sizeof(UInt64));
            MemorySystem::MemoryCopy(m_ComponentData.Pointer, other.m_ComponentData.Pointer, m_BytesStored);
        }

        /**
         * @brief Used to move another container to this.
         *
         * @param other The other container to move.
         */
        void Move(ComponentData&& other)
        {
            m_ComponentIds   = other.m_ComponentIds;
            m_ComponentSizes = other.m_ComponentSizes;
            m_Count          = other.m_Count;
            m_Capacity       = other.m_Capacity;
            m_ComponentData  = other.m_ComponentData;
            m_BytesStored    = other.m_BytesStored;

            other.m_ComponentIds   = nullptr;
            other.m_ComponentSizes = nullptr;
            other.m_Count          = 0;
            other.m_Capacity       = 0;
            other.m_ComponentData  = { };
            other.m_BytesStored    = 0;
        }

        /**
         * @brief Used to expand the size of the component data container.
         */
        void Expand()
        {
            m_Capacity = m_Capacity == 0 ? k_DefaultCapacity : m_Capacity * k_ResizingFactor;

            auto* newComponentIds   = Buffer::New<ComponentId>(m_Capacity);
            auto* newComponentSizes = Buffer::New<UInt64>(m_Capacity);
            auto newComponentData = Unsafe::New(m_Capacity * k_DefaultDataCapacity);

            if (m_Count > 0)
            {
                MemorySystem::MemoryCopy(newComponentIds, m_ComponentIds, m_Count * sizeof(ComponentId));
                MemorySystem::MemoryCopy(newComponentSizes, m_ComponentSizes, m_Count * sizeof(UInt64));
                MemorySystem::MemoryCopy(newComponentData.Pointer, m_ComponentData.Pointer, m_BytesStored);
            }

            if (IsCreated())
                Destroy();

            m_ComponentIds   = newComponentIds;
            m_ComponentSizes = newComponentSizes;
            m_ComponentData  = newComponentData;
        }

        /**
        * @brief Destroys the component data container.
        */
        void Destroy()
        {
            if (m_ComponentIds)
                Buffer::Delete<ComponentId>(m_ComponentIds, m_Capacity);

            if (m_ComponentSizes)
                Buffer::Delete<UInt64>(m_ComponentSizes, m_Capacity);

            if (m_ComponentData.Pointer && m_ComponentData.Size > 0)
                Unsafe::Delete(m_ComponentData);

            m_ComponentIds   = nullptr;
            m_ComponentSizes = nullptr;
            m_ComponentData.Pointer = nullptr;
        }
    };
}

#endif //OTTERENGINE_COMPONENTDATA_H
