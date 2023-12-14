#ifndef OTTERENGINE_ENTITY_H
#define OTTERENGINE_ENTITY_H

#include "Core/BaseTypes.h"

namespace Otter
{
    /*
     * Planning phase:
     *
     * Entity => UInt64
     *
     * Component:
     * - Has an id
     * - Inherits from IComponent, which has no virtual methods, protected constructor, and deleted copy/move constructors
     *   and assignment operators
     *
     * Archetype:
     * - Has an id
     * - Has a component mask
     * - Has a list of components
     *
     * Use a HashSet of Archetypes that is used to store the Archetypes that are currently in use.
     * Use a HashMap of Entity to Archetype that is used to store the Archetype that an Entity is currently using.
     * Use a HashMap of Component to Archetype HashSet that is used to store the Archetypes that a Component is in.
     */
}

#endif //OTTERENGINE_ENTITY_H
