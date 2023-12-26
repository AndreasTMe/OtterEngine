#ifndef OTTERENGINE_ENTITYMANAGER_H
#define OTTERENGINE_ENTITYMANAGER_H

#include "Core/Collections/List.h"
#include "Core/Collections/HashSet.h"
#include "Core/Collections/Dictionary.h"

#include "ECS/Entity.h"
#include "ECS/Archetype.h"
#include "Components/IComponent.h"
#include "Components/Core/TransformComponent.h"

namespace Otter
{
    class EntityManager
    {
    public:
        /**
         * @brief Deleted constructor.
         */
        EntityManager() = delete;

        /**
         * @brief Deleted destructor.
         */
        ~EntityManager() = delete;

        /**
         * @brief Deleted copy constructor.
         */
        EntityManager(const EntityManager& other) = delete;

        /**
         * @brief Deleted move constructor.
         */
        EntityManager(EntityManager&& other) noexcept = delete;

        /**
         * @brief Deleted copy assignment operator.
         */
        EntityManager& operator=(const EntityManager& other) = delete;

        /**
         * @brief Deleted move assignment operator.
         */
        EntityManager& operator=(EntityManager&& other) noexcept = delete;

        // TODO: Probably something like this for initialising and destroying the world.
        [[nodiscard]] static bool TryInitialiseWorld();
        static void DestroyWorld();

        [[nodiscard]] static Entity CreateEntity()
        {
            static EntityId id = 0;
            id++;

            return Entity{ id };
        }

        static void DestroyEntity(Entity entity)
        {
            // TODO: Implement this.
        }

        template<typename TComponent>
        requires IsComponent<TComponent>
        static bool TryAddComponent(Entity entity)
        {
            OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component ID must be greater than 0.");

            return false;
        }

        template<typename TComponent>
        requires IsComponent<TComponent>
        static bool TryRemoveComponent(Entity entity)
        {
            OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component ID must be greater than 0.");

            return false;
        }

    private:
        static Dictionary<Entity, ArchetypeId>               s_EntityToArchetype;
        static Dictionary<ComponentId, HashSet<ArchetypeId>> s_ComponentToArchetypes;
        static Dictionary<ArchetypeId, Archetype>            s_Archetypes;
    };
}

#endif //OTTERENGINE_ENTITYMANAGER_H
