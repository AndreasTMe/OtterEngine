#ifndef OTTERENGINE_ENTITYMANAGER_H
#define OTTERENGINE_ENTITYMANAGER_H

#include "Core/Collections/List.h"
#include "Core/Collections/Stack.h"
#include "Core/Collections/HashSet.h"
#include "Core/Collections/Dictionary.h"

#include "ECS/Entity.h"
#include "ECS/Archetype.h"
#include "Components/IComponent.h"

namespace Otter
{
    class EntityManager final
    {
        class ArchetypeBuilder;

        class EntityBuilder;

    public:
        /**
         * @brief Constructor.
         */
        EntityManager() = default;

        /**
         * @brief Destructor.
         */
        ~EntityManager()
        {
            m_Entities.ClearDestructive();
            m_EntityToIndex.ClearDestructive();
            m_EntitiesToAdd.ClearDestructive();

            m_ComponentToMaskIndex.ClearDestructive();
            m_ComponentToMask.ClearDestructive();

            m_MaskToArchetype.ClearDestructive();
        }

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
        requires IsComponent<TComponent> && AreComponents<TComponents...>
        EntityManager& RegisterComponents()
        {
            if (m_ComponentMaskLock)
                return *this;

            RegisterComponentsRecursive<TComponent, TComponents...>();

            return *this;
        }

        OTR_INLINE void LockComponentMask() { m_ComponentMaskLock = true; }

        /**
         * @brief Creates a new archetype by calling a builder.
         *
         * @return The archetype builder.
         */
        [[nodiscard]] ArchetypeBuilder CreateArchetype() noexcept { return ArchetypeBuilder(this); }

        [[nodiscard]] EntityBuilder CreateEntity();
        void DestroyEntity(Entity entity);
        void RefreshEntities();

        template<typename TComponent>
        requires IsComponent<TComponent>
        bool TryAddComponent(Entity entity)
        {
            OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component Id must be greater than 0.")

            return false;
        }

        template<typename TComponent>
        requires IsComponent<TComponent>
        bool TryRemoveComponent(Entity entity)
        {
            OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component Id must be greater than 0.")

            return false;
        }

    private:
        class ArchetypeBuilder final
        {
        public:
            explicit ArchetypeBuilder(EntityManager* entityManager)
                : m_EntityManager(entityManager), m_ComponentMask()
            {
            }

            ~ArchetypeBuilder()
            {
                m_EntityManager = nullptr;
                m_ComponentMask.ClearDestructive();
                m_ComponentIds.ClearDestructive();
            }

            template<typename TComponent>
            requires IsComponent<TComponent>
            ArchetypeBuilder& With()
            {
                OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component Id must be greater than 0.")
                OTR_ASSERT_MSG(m_EntityManager->m_ComponentMaskLock, "Component mask must be locked.")
                OTR_ASSERT_MSG(m_EntityManager->m_ComponentToMaskIndex.ContainsKey(TComponent::Id),
                               "Component must be registered.")

                UInt64 index = 0;
                m_EntityManager->m_ComponentToMaskIndex.TryGet(TComponent::Id, &index);

                m_ComponentMask.Set(index, true);
                m_ComponentIds.Add(TComponent::Id);

                return *this;
            }

            Archetype Build()
            {
                Archetype archetype(m_ComponentMask, m_ComponentIds);
                m_EntityManager->m_MaskToArchetype.TryAdd(m_ComponentMask, archetype);

                return archetype;
            }

        private:
            EntityManager* m_EntityManager;
            BitSet            m_ComponentMask;
            List<ComponentId> m_ComponentIds;
        };

        class EntityBuilder final
        {
        public:
            EntityBuilder(EntityManager* entityManager, Entity entity)
                : m_EntityManager(entityManager), m_Entity(entity)
            {
                OTR_ASSERT_MSG(m_Entity.IsValid(), "Entity must be valid.")
            }

            ~EntityBuilder() = default;

        private:
            EntityManager* m_EntityManager;
            Entity m_Entity;
        };

        // Entity Registry
        List<Entity>               m_Entities;
        Dictionary<Entity, UInt64> m_EntityToIndex;
        Stack<Entity>              m_EntitiesToAdd;

        // Component Registry
        Dictionary<ComponentId, UInt64>       m_ComponentToMaskIndex;
        Dictionary<ComponentId, List<BitSet>> m_ComponentToMask;
        bool                                  m_ComponentMaskLock = false;

        // Archetype Registry
        Dictionary<BitSet, Archetype> m_MaskToArchetype;

        void PopulateEntitiesToAdd();
        void CleanUpEntities();

        template<typename TComponent, typename... TComponents>
        requires IsComponent<TComponent> && AreComponents<TComponents...>
        void RegisterComponentsRecursive()
        {
            OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component Id must be greater than 0.")

            if (m_ComponentToMaskIndex.ContainsKey(TComponent::Id))
                return;

            UInt64 index = m_ComponentToMaskIndex.GetCount();
            m_ComponentToMaskIndex.TryAdd(TComponent::Id, index);

            if constexpr (VariadicArgs<TComponents...>::GetSize() > 0)
                RegisterComponentsRecursive<TComponents...>();
        }
    };
}

#endif //OTTERENGINE_ENTITYMANAGER_H
