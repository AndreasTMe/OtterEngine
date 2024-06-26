#ifndef OTTERENGINE_ENTITY_H
#define OTTERENGINE_ENTITY_H

#include "Core/BaseTypes.h"

namespace Otter
{
    class EntityManager;

    /// @brief Type alias for entity Ids.
    using EntityId = UInt64;

    /**
     * @brief Entity class that is used to identify an entity in the entity-component system.
     */
    struct Entity final
    {
    public:
        /**
         * @brief Constructor.
         */
        Entity() = default;

        /**
        * @brief Destructor.
        */
        ~Entity() = default;

        /**
         * @brief Copy constructor.
         *
         * @param other Entity to copy from.
         */
        Entity(const Entity& other)
        {
            m_Id      = other.m_Id;
            m_IsValid = other.m_IsValid;
        }

        /**
         * @brief Move constructor.
         *
         * @param other Entity to move from.
         */
        Entity(Entity&& other) noexcept
        {
            m_Id      = other.m_Id;
            m_IsValid = other.m_IsValid;

            other.m_Id      = 0;
            other.m_IsValid = false;
        }

        /**
         * @brief Copy assignment operator.
         *
         * @param other Entity to copy from.
         *
         * @return Reference to this entity.
         */
        Entity& operator=(const Entity& other) = default;

        /**
         * @brief Move assignment operator.
         *
         * @param other Entity to move from.
         *
         * @return Reference to this entity.
         */
        Entity& operator=(Entity&& other) noexcept
        {
            m_Id      = other.m_Id;
            m_IsValid = other.m_IsValid;

            other.m_Id      = 0;
            other.m_IsValid = false;

            return *this;
        }

        /**
         * @brief Equality operator.
         *
         * @param other Entity to compare to.
         *
         * @return True if the entities are equal, false otherwise.
         */
        OTR_INLINE bool operator==(const Entity& other) const noexcept { return m_Id == other.m_Id; }

        /**
         * @brief Inequality operator.
         *
         * @param other Entity to compare to.
         *
         * @return True if the entities are not equal, false otherwise.
         */
        OTR_INLINE bool operator!=(const Entity& other) const noexcept { return m_Id != other.m_Id; }

        /**
         * @brief Gets the id of the entity.
         *
         * @return Id of the entity.
         */
        [[nodiscard]] OTR_INLINE EntityId GetId() const noexcept { return m_Id; }

        /**
         * @brief Checks if the entity is valid.
         *
         * @return True if the entity is valid, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsValid() const noexcept { return m_Id > 0 && m_IsValid; }

        /**
         * @brief Gets the hash code of the entity.
         *
         * @return Hash code of the entity.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetHashCode() const noexcept
        {
            return std::hash<UInt64>{ }(m_Id);
        }

    private:
        /**
         * @brief Constructor.
         *
         * @param id Id of the entity.
         */
        explicit Entity(EntityId id) noexcept
            : m_Id(id), m_IsValid(true)
        {
        }

        EntityId m_Id      = 0;
        bool     m_IsValid = false;

        friend class EntityManager;
    };
}

#endif //OTTERENGINE_ENTITY_H
