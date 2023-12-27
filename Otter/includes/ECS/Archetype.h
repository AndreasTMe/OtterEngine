#ifndef OTTERENGINE_ARCHETYPE_H
#define OTTERENGINE_ARCHETYPE_H

#include "Core/BaseTypes.h"
#include "Core/Collections/BitSet.h"

namespace Otter
{
    class EntityManager;

    using ArchetypeId = UInt16;

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

    private:
        /**
         * @brief Constructor.
         *
         * @param id Id of the entity.
         */
        explicit Archetype(ArchetypeId id) noexcept
            : m_Id(id)
        {
        }

        ArchetypeId m_Id = 0;
        BitSet      m_ComponentMask;

        friend class EntityManager;
    };
}

#endif //OTTERENGINE_ARCHETYPE_H
