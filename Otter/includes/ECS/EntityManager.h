#ifndef OTTERENGINE_ENTITYMANAGER_H
#define OTTERENGINE_ENTITYMANAGER_H

#include "Core/Collections/List.h"
#include "Core/Collections/Stack.h"
#include "Core/Collections/Dictionary.h"

#include "ECS/Entity.h"
#include "ECS/Archetype.h"
#include "Components/IComponent.h"

namespace Otter
{
    class EntityManager final
    {
        class EntityBuilder;

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

        template<typename TComponent, typename... TComponents>
        requires IsComponent<TComponent>
        void RegisterComponents()
        {
            if (m_ComponentMaskLock)
                return;

            RegisterComponentsRecursive<TComponent, TComponents...>();
        }

        OTR_INLINE void LockComponentMask() { m_ComponentMaskLock = true; }

        [[nodiscard]] EntityBuilder CreateEntity();
        void DestroyEntity(Entity entity);
        void RefreshEntities();

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
        class EntityBuilder final
        {
        public:
            EntityBuilder(EntityManager* entityManager, Entity entity)
                : m_EntityManager(entityManager), m_Entity(entity), m_ComponentMask()
            {
                m_ComponentMask.Reserve(m_EntityManager->m_ComponentToMaskIndex.GetCount());
            }
            ~EntityBuilder() = default;

            template<typename TComponent, typename... TArgs>
            requires IsComponent<TComponent>
            EntityBuilder& With(TArgs&& ... args)
            {
                OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component Id must be greater than 0.");

                UInt64 index;
                if (!m_EntityManager->m_ComponentToMaskIndex.TryGet(TComponent::Id, &index))
                    return *this;

                m_ComponentMask.Set(index, true);

                // TODO: Create component and store it in an archetype.
                // auto component = TComponent(std::forward<TArgs>(args)...);

                return *this;
            }

            Entity Build()
            {
                auto archetype = Archetype(std::move(m_ComponentMask));

                if (!m_EntityManager->m_MaskToArchetype.Contains(m_ComponentMask))
                    m_EntityManager->m_MaskToArchetype.TryAdd(m_ComponentMask, archetype);

                return m_Entity;
            }

        private:
            EntityManager* m_EntityManager;
            Entity m_Entity;
            BitSet m_ComponentMask;
        };

        // Entity Registry
        List<Entity>               m_Entities;
        Dictionary<Entity, UInt64> m_EntityToIndex;
        Stack<Entity>              m_EntitiesToAdd;

        // Component Registry
        Dictionary<ComponentId, UInt64> m_ComponentToMaskIndex;
        bool                            m_ComponentMaskLock = false;

        // Archetype Registry
        Dictionary<BitSet, Archetype> m_MaskToArchetype;

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

            if constexpr (VariadicArgs<TComponents...>::GetSize() > 0 && (IsComponent<TComponents> && ...))
                RegisterComponentsRecursive<TComponents...>();
        }
    };
}

#endif //OTTERENGINE_ENTITYMANAGER_H
