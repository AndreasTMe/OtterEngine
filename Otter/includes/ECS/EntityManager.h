#ifndef OTTERENGINE_ENTITYMANAGER_H
#define OTTERENGINE_ENTITYMANAGER_H

#include "Core/Collections/List.h"
#include "Core/Collections/Stack.h"
#include "Core/Collections/Dictionary.h"

#include "ECS/Entity.h"
#include "ECS/Archetype.h"
#include "Components/IComponent.h"
#include "Components/Core/TransformComponent.h"
#include "Components/Core/SpriteComponent.h"

namespace Otter
{
    class EntityManager
    {
    public:
        /**
         * @brief Constructor.
         */
        EntityManager() = default;

        /**
         * @brief Destructor.
         */
        ~EntityManager() = default;

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

        [[nodiscard]] Entity CreateEntity();
        void DestroyEntity(Entity entity);
        void RefreshEntities();

        template<typename TComponent, typename... TComponents>
        requires IsComponent<TComponent>
        void RegisterComponents()
        {
            if (m_ComponentMaskLock)
                return;

            RegisterComponentsRecursive<TComponent, TComponents...>();
        }

        OTR_INLINE void LockComponentMask() { m_ComponentMaskLock = true; }

        template<typename TComponent>
        requires IsComponent<TComponent>
        bool TryAddComponent(Entity entity)
        {
            OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component Id must be greater than 0.");

            return false;
        }

        template<typename TComponent>
        requires IsComponent<TComponent>
        bool TryRemoveComponent(Entity entity)
        {
            OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component Id must be greater than 0.");

            return false;
        }

    private:
        // Entity Registry
        List<Entity>               m_Entities;
        Dictionary<Entity, UInt64> m_EntityToIndex;
        Stack<Entity>              m_EntitiesToAdd;

        // Component Registry
        Dictionary<ComponentId, UInt64> m_ComponentToMaskIndex;
        bool                            m_ComponentMaskLock = false;

        void PopulateEntitiesToAdd();
        void CleanUpEntities();

        template<typename TComponent, typename... TComponents>
        requires IsComponent<TComponent>
        void RegisterComponentsRecursive()
        {
            OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component Id must be greater than 0.");

            if (m_ComponentToMaskIndex.Contains(TComponent::Id))
                return;

            UInt64 index = m_ComponentToMaskIndex.GetCount();
            m_ComponentToMaskIndex.TryAdd(TComponent::Id, index);

            if constexpr (sizeof...(TComponents) > 0 && (IsComponent<TComponents> && ...))
                RegisterComponentsRecursive<TComponents...>();
        }
    };
}

#endif //OTTERENGINE_ENTITYMANAGER_H
