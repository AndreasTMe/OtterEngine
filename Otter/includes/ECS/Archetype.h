#ifndef OTTERENGINE_ARCHETYPE_H
#define OTTERENGINE_ARCHETYPE_H

#include "Core/Collections/BitSet.h"

namespace Otter::ECS
{
    /*
     * Use a HashSet of Archetypes that is used to store the Archetypes that are currently in use.
     * Use a HashMap of Entity to Archetype that is used to store the Archetype that an Entity is currently using.
     * Use a HashMap of Component to Archetype HashSet that is used to store the Archetypes that a Component is in.
     */

    /**
     * @brief Archetype class for ECS. It is used to store sets of components in contiguous blocks of memory. Each
     * entity has an archetype that it belongs to, based on the components that it is registered with.
     */
    class Archetype final
    {
    public:
        /**
         * @brief Constructor.
         */
        Archetype()
            : m_Id(0), m_ComponentMask()
        {
        }

        /**
         * @brief Destructor.
         */
        ~Archetype() = default;

        /**
         * @brief Copy constructor.
         *
         * @param other Archetype to copy from.
         */
        Archetype(const Archetype& other)
        {
            m_Id            = other.m_Id;
            m_ComponentMask = other.m_ComponentMask;
        }

        /**
         * @brief Move constructor.
         *
         * @param other Archetype to move from.
         */
        Archetype(Archetype&& other) noexcept
        {
            m_Id            = other.m_Id;
            m_ComponentMask = std::move(other.m_ComponentMask);

            other.m_Id = 0;
        }

        /**
         * @brief Copy assignment operator.
         *
         * @param other Archetype to copy from.
         *
         * @return Reference to this archetype.
         */
        Archetype& operator=(const Archetype& other)
        {
            if (this == &other)
                return *this;

            m_Id            = other.m_Id;
            m_ComponentMask = other.m_ComponentMask;

            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * @param other Archetype to move from.
         *
         * @return Reference to this archetype.
         */
        Archetype& operator=(Archetype&& other) noexcept
        {
            if (this == &other)
                return *this;

            m_Id            = other.m_Id;
            m_ComponentMask = std::move(other.m_ComponentMask);

            other.m_Id = 0;

            return *this;
        }

        /**
         * @brief Equality operator.
         *
         * @param other Archetype to compare to.
         *
         * @return True if the archetypes are equal, false otherwise.
         */
        OTR_INLINE bool operator==(const Archetype& other) const noexcept { return m_Id == other.m_Id; }

        /**
         * @brief Inequality operator.
         *
         * @param other Archetype to compare to.
         *
         * @return True if the archetypes are not equal, false otherwise.
         */
        OTR_INLINE bool operator!=(const Archetype& other) const noexcept { return m_Id != other.m_Id; }

        /**
         * @brief Gets the id of the archetype.
         *
         * @return Id of the archetype.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetId() const noexcept { return m_Id; }

        /**
         * @brief Checks if the archetype is valid.
         *
         * @return True if the archetype is valid, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsValid() const noexcept { return m_Id > 0; }

        /**
         * @brief Gets the hash code of the archetype.
         *
         * @return Hash code of the archetype.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetHashCode() const noexcept
        {
            return std::hash<UInt64>{ }(m_Id); // TODO: Change from id to component mask
        }

    private:
        UInt64 m_Id;
        BitSet m_ComponentMask;
        // TODO: List of components
    };
}

#endif //OTTERENGINE_ARCHETYPE_H
