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

        class EntityBuilderFromArchetype;

    public:
        /**
         * @brief Constructor.
         */
        EntityManager() = default;

        /**
         * @brief Destructor.
         */
        ~EntityManager();

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
         * @brief Creates a new entity by calling a builder and using an archetype.
         *
         * @param archetype The archetype to create the entity from.
         *
         * @return The entity builder.
         */
        [[nodiscard]] EntityBuilderFromArchetype CreateEntityFromArchetype(const Archetype& archetype);

        /**
         * @brief Destroys an entity.
         *
         * @param entity The entity to destroy.
         */
        void DestroyEntity(const Entity& entity);

        /**
         * @brief Refreshes the entity manager's managed data (entities, archetypes, etc.).
         */
        void RefreshManagedData();

        template<typename TComponent, typename... TArgs>
        requires IsComponent<TComponent>
        bool TryAddComponent(const Entity& entity, TArgs&& ... args)
        {
            OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component Id must be greater than 0.")

            return false;
        }

        template<typename TComponent>
        requires IsComponent<TComponent>
        bool TryRemoveComponent(const Entity& entity)
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
         * @brief A class responsible for creating archetypes.
         */
        class ArchetypeBuilder final
        {
        public:
            /**
             * @brief Constructor.
             *
             * @param entityManager The entity manager.
             */
            explicit ArchetypeBuilder(EntityManager* entityManager);

            /**
             * @brief Destructor.
             */
            ~ArchetypeBuilder();

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

                WithInternal(TComponent::Id);

                return *this;
            }

            /**
             * @brief Builds the archetype.
             *
             * @return The archetype.
             */
            [[nodiscard]] Archetype Build();

        private:
            EntityManager* m_EntityManager;
            ArchetypeFingerprint m_Fingerprint;
            List<ComponentId>    m_ComponentIds;

            /**
             * @brief Adds a component to the archetype. Used internally.
             *
             * @param componentId The component id.
             */
            void WithInternal(UInt64 componentId);
        };

        /**
         * @brief A class responsible for creating entities.
         */
        class EntityBuilder final
        {
        public:
            /**
             * @brief Constructor.
             *
             * @param entityManager The entity manager.
             * @param entity The entity.
             */
            EntityBuilder(EntityManager* entityManager, Entity entity);

            /**
             * @brief Destructor.
             */
            ~EntityBuilder();

            /**
             * @brief Sets the component data.
             *
             * @tparam TComponent The component type.
             * @tparam TArgs The component arguments.
             *
             * @param args The component arguments.
             *
             * @return A reference to the entity builder.
             */
            template<typename TComponent, typename... TArgs>
            requires IsComponent<TComponent>
            EntityBuilder& SetComponentData(TArgs&& ... args)
            {
                OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component Id must be greater than 0.")

                auto component = TComponent(std::forward<TArgs>(args)...);
                SetComponentDataInternal(TComponent::Id,
                                         ComponentData{ TComponent::Id, (Byte*) &component, sizeof(TComponent) });

                return *this;
            }

            /**
             * @brief Builds the entity.
             *
             * @return The entity.
             */
            [[nodiscard]] Entity Build();

        private:
            EntityManager* m_EntityManager;
            Entity               m_Entity;
            ArchetypeFingerprint m_Fingerprint;
            List<ComponentData>  m_ComponentData;

            /**
             * @brief Sets the component data. Used internally.
             */
            void SetComponentDataInternal(UInt64 componentId, ComponentData&& componentData);
        };

        /**
         * @brief A class responsible for creating entities from archetypes.
         */
        class EntityBuilderFromArchetype final
        {
        public:
            /**
             * @brief Constructor.
             *
             * @param entityManager The entity manager.
             * @param entity The entity.
             * @param archetype The archetype.
             */
            EntityBuilderFromArchetype(EntityManager* entityManager, Entity entity, Archetype archetype);

            /**
             * @brief Destructor.
             */
            ~EntityBuilderFromArchetype();

            /**
             * @brief Sets the component data.
             *
             * @tparam TComponent The component type.
             * @tparam TArgs The component arguments.
             *
             * @param args The component arguments.
             *
             * @return A reference to the entity builder.
             */
            template<typename TComponent, typename... TArgs>
            requires IsComponent<TComponent>
            EntityBuilderFromArchetype& SetComponentData(TArgs&& ... args)
            {
                OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component Id must be greater than 0.")

                auto component = TComponent(std::forward<TArgs>(args)...);
                SetComponentDataInternal(TComponent::Id,
                                         ComponentData{ TComponent::Id, (Byte*) &component, sizeof(TComponent) });

                return *this;
            }

            /**
             * @brief Builds the entity.
             *
             * @return The entity.
             */
            [[nodiscard]] Entity Build();

        private:
            EntityManager* m_EntityManager;
            Entity               m_Entity;
            List<ComponentData>  m_ComponentData;
            Archetype            m_Archetype;
            ArchetypeFingerprint m_FingerprintTrack;

            /**
             * @brief Sets the component data. Used internally.
             */
            void SetComponentDataInternal(UInt64 componentId, ComponentData&& componentData);
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
        Dictionary<ArchetypeFingerprint, Archetype>                                 m_FingerprintToArchetype;
        Stack<Archetype>                                                            m_ArchetypesToAdd;
        Dictionary<ArchetypeFingerprint, Dictionary<EntityId, List<ComponentData>>> m_FingerprintToComponentDataToAdd;

        /**
         * @brief Creates an entity.
         *
         * @return The entity.
         */
        Entity CreateEntityInternal();

        /**
         * @brief Populates the list of archetypes to be added in the next refresh.
         */
        void PopulateNewArchetypes();

        /**
         * @brief Populates the list of entities to be added in the next refresh.
         */
        void PopulateNewEntities();

        /**
         * @brief Removes any entities that are not valid.
         */
        void CleanUpEntities();

        /**
         * @brief Registers a component or a list of components recursively.
         *
         * @tparam TComponent The component type.
         * @tparam TComponents The rest of the components.
         */
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
