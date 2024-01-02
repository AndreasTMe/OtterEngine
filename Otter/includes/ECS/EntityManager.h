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
    /**
     * @brief The entity manager is responsible for creating, destroying and managing entities, registering components
     * and creating archetypes.
     */
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

            m_ComponentToFingerprintIndex.ClearDestructive();
            m_ComponentToFingerprints.ClearDestructive();

            m_FingerprintToArchetype.ClearDestructive();
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

        /**
         * @brief Registers a component or a list of components.
         *
         * @tparam TComponent The component type.
         * @tparam TComponents The rest of the components.
         *
         * @return A reference to the entity manager.
         */
        template<typename TComponent, typename... TComponents>
        requires IsComponent<TComponent> && AreComponents<TComponents...>
        EntityManager& RegisterComponents()
        {
            if (m_ComponentsLock)
                return *this;

            RegisterComponentsInternal<TComponent, TComponents...>();

            return *this;
        }

        /**
         * @brief Locks the entity manager's ability to register components.
         */
        OTR_INLINE void LockComponents() { m_ComponentsLock = true; }

        /**
         * @brief Creates a new archetype by calling a builder.
         *
         * @return The archetype builder.
         */
        [[nodiscard]] OTR_INLINE ArchetypeBuilder CreateArchetype() noexcept
        {
            OTR_ASSERT_MSG(m_ComponentsLock, "Component registration must be locked.")

            return ArchetypeBuilder(this);
        }

        /**
         * @brief Creates a new entity by calling a builder.
         *
         * @return The entity builder.
         */
        [[nodiscard]] EntityBuilder CreateEntity();

        /**
         * @brief Creates a new entity by calling a builder.
         *
         * @param archetype The archetype to create the entity from.
         *
         * @return The entity builder.
         */
        [[nodiscard]] EntityBuilder CreateEntityFromArchetype(const Archetype& archetype);

        /**
         * @brief Destroys an entity.
         *
         * @param entity The entity to destroy.
         */
        void DestroyEntity(const Entity& entity);

        /**
         * @brief Refreshes the entities.
         */
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

        /**
         * @brief Gets the entity count.
         *
         * @return The entity count.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetEntityCount() const { return m_Entities.GetCount(); }

        /**
         * @brief Gets the component count.
         *
         * @return The component count.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetComponentCount() const { return m_ComponentToFingerprintIndex.GetCount(); }

        /**
         * @brief Gets the archetype count.
         *
         * @return The archetype count.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetArchetypeCount() const { return m_FingerprintToArchetype.GetCount(); }

    private:
        /**
         * @brief The archetype builder is responsible for creating archetypes.
         */
        class ArchetypeBuilder final
        {
        public:
            /**
             * @brief Constructor.
             *
             * @param entityManager The entity manager.
             */
            explicit ArchetypeBuilder(EntityManager* entityManager)
                : m_EntityManager(entityManager), m_Fingerprint()
            {
            }

            /**
             * @brief Destructor.
             */
            ~ArchetypeBuilder()
            {
                m_EntityManager = nullptr;
                m_Fingerprint.ClearDestructive();
                m_ComponentIds.ClearDestructive();
            }

            /**
             * @brief Adds a component to the archetype.
             *
             * @tparam TComponent The component type.
             *
             * @return A reference to the archetype builder.
             */
            template<typename TComponent>
            requires IsComponent<TComponent>
            ArchetypeBuilder& With()
            {
                OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component Id must be greater than 0.")
                OTR_ASSERT_MSG(m_EntityManager->m_ComponentsLock, "Component registration must be locked.")
                OTR_ASSERT_MSG(m_EntityManager->m_ComponentToFingerprintIndex.ContainsKey(TComponent::Id),
                               "Component must be registered.")

                UInt64 index = 0;
                m_EntityManager->m_ComponentToFingerprintIndex.TryGet(TComponent::Id, &index);

                m_Fingerprint.Set(index, true);
                m_ComponentIds.Add(TComponent::Id);

                return *this;
            }

            /**
             * @brief Builds the archetype.
             *
             * @return The archetype.
             */
            [[nodiscard]] Archetype Build()
            {
                Archetype archetype(m_Fingerprint, m_ComponentIds);
                // TODO: Merge if archetype exists, currently just overwrites.
                m_EntityManager->m_FingerprintToArchetype.TryAdd(m_Fingerprint, archetype);

                return archetype;
            }

        private:
            EntityManager* m_EntityManager;
            ArchetypeFingerprint m_Fingerprint;
            List<ComponentId>    m_ComponentIds;
        };

        class EntityBuilder final
        {
        public:
            EntityBuilder(EntityManager* entityManager, Entity entity)
                : m_EntityManager(entityManager), m_Entity(std::move(entity))
            {
                OTR_ASSERT_MSG(m_Entity.IsValid(), "Entity must be valid.")
            }

            EntityBuilder(EntityManager* entityManager, Entity entity, const Archetype& archetype)
                : m_EntityManager(entityManager), m_Entity(std::move(entity)), m_Archetype(archetype)
            {
                OTR_ASSERT_MSG(m_Entity.IsValid(), "Entity must be valid.")
                OTR_ASSERT_MSG(m_Archetype.GetComponentCount() > 0, "Archetype must have components.")
            }

            ~EntityBuilder() = default;

            template<typename TComponent, typename... TArgs>
            requires IsComponent<TComponent>
            EntityBuilder& SetComponent(TArgs&& ... args)
            {
                OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component Id must be greater than 0.")
                OTR_ASSERT_MSG(m_EntityManager->m_ComponentsLock, "Component registration must be locked.")
                OTR_ASSERT_MSG(m_EntityManager->m_ComponentToFingerprintIndex.ContainsKey(TComponent::Id),
                               "Component must be registered.")

                UInt64 index = 0;
                m_EntityManager->m_ComponentToFingerprintIndex.TryGet(TComponent::Id, &index);

                // TODO: Implement this.

                return *this;
            }

            [[nodiscard]] Entity Build()
            {
                return m_Entity;
            }

        private:
            EntityManager* m_EntityManager;
            Entity    m_Entity;
            Archetype m_Archetype;
        };

        // Entity Registry
        List<Entity>               m_Entities;
        Dictionary<Entity, UInt64> m_EntityToIndex;
        Stack<Entity>              m_EntitiesToAdd;

        // Component Registry
        Dictionary<ComponentId, UInt64>                     m_ComponentToFingerprintIndex;
        Dictionary<ComponentId, List<ArchetypeFingerprint>> m_ComponentToFingerprints;
        bool                                                m_ComponentsLock = false;

        // Archetype Registry
        Dictionary<ArchetypeFingerprint, Archetype>           m_FingerprintToArchetype;
        Dictionary<ArchetypeFingerprint, List<ComponentData>> m_FingerprintToComponentDataToAdd;

        Entity CreateEntityInternal();
        void PopulateEntitiesToAdd();
        void CleanUpEntities();

        template<typename TComponent, typename... TComponents>
        requires IsComponent<TComponent> && AreComponents<TComponents...>
        void RegisterComponentsInternal()
        {
            OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component Id must be greater than 0.")

            if (m_ComponentToFingerprintIndex.ContainsKey(TComponent::Id))
                return;

            UInt64 index = m_ComponentToFingerprintIndex.GetCount();
            m_ComponentToFingerprintIndex.TryAdd(TComponent::Id, index);

            if constexpr (VariadicArgs<TComponents...>::GetSize() > 0)
                RegisterComponentsInternal<TComponents...>();
        }
    };
}

#endif //OTTERENGINE_ENTITYMANAGER_H
