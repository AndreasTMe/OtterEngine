#ifndef OTTERENGINE_ARCHETYPE_H
#define OTTERENGINE_ARCHETYPE_H

#include "Core/BaseTypes.h"
#include "Core/Collections/Dictionary.h"
#include "Core/Collections/UnsafeList.h"

namespace Otter
{
    class EntityManager;

    class Archetype
    {
    public:
        /**
         * @brief Constructor.
         */
        Archetype() = default;

        /**
         * @brief Destructor.
         */
        ~Archetype() = default;

        /**
         * @brief Copy constructor.
         */
        Archetype(const Archetype& other) = default;

        /**
         * @brief Move constructor.
         */
        Archetype(Archetype&& other) noexcept = default;

        /**
         * @brief Copy assignment operator.
         */
        Archetype& operator=(const Archetype& other) = default;

        /**
         * @brief Move assignment operator.
         */
        Archetype& operator=(Archetype&& other) noexcept = default;

        /**
         * @brief Equality operator.
         *
         * @param other The other archetype.
         *
         * @return True if the archetypes are equal, false otherwise.
         */
        bool operator==(const Archetype& other) const noexcept
        {
            return m_ComponentMask == other.m_ComponentMask
                   && m_MaskIndexToComponentData == other.m_MaskIndexToComponentData;
        }

        /**
         * @brief Inequality operator.
         *
         * @param other The other archetype.
         *
         * @return True if the archetypes are not equal, false otherwise.
         */
        bool operator!=(const Archetype& other) const noexcept { return !(*this == other); }

    private:
        /**
         * @brief Constructor.
         *
         * @param componentMask The component mask of the archetype.
         */
        explicit Archetype(BitSet&& componentMask) noexcept
            : m_ComponentMask(std::move(componentMask))
        {
        }

        BitSet                         m_ComponentMask;
        Dictionary<UInt64, UnsafeList> m_MaskIndexToComponentData;

        friend class EntityManager;
    };
}

#endif //OTTERENGINE_ARCHETYPE_H
