#ifndef OTTERENGINE_ENTITYMANAGER_H
#define OTTERENGINE_ENTITYMANAGER_H

#include "Core/Collections/List.h"
#include "Core/Collections/Stack.h"
#include "Core/Collections/HashSet.h"
#include "Core/Collections/Dictionary.h"
#include "ECS/Entity.h"
#include "ECS/Archetype.h"
#include "ECS/ComponentData.h"

namespace Otter
{
    class World;

    /**
     * @brief The entity manager is responsible for creating, destroying and managing entities, registering components
     * and creating archetypes.
     */
    class EntityManager final
    {
        class ArchetypeBuilder;

        class EntityBuilder;

        class EntityBuilderFromArchetype;

        friend class World;

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
            Destroy();
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
         * @brief Checks if the entity manager is locked.
         *
         * @return True if the entity manager is locked, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsLocked() const { return m_ComponentsLock; }

        /**
         * @brief Creates a new archetype by calling a builder.
         *
         * @return The archetype builder.
         */
        [[nodiscard]] OTR_INLINE ArchetypeBuilder CreateArchetype() noexcept
        {
            OTR_ASSERT(m_ComponentsLock, "Component registration must be locked.")

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
         * @brief Refreshes the entity manager's data (entities, archetypes, etc.).
         */
        void RefreshManagerData();

        /**
         * @brief Adds a component to an entity.
         *
         * @tparam TComponent The component type.
         * @tparam TArgs The component arguments.
         *
         * @param entity The entity.
         * @param args The component arguments.
         *
         * @return True if the component was added, false otherwise.
         */
        template<typename TComponent, typename... TArgs>
        requires IsComponent<TComponent>
        bool TryAddComponent(const Entity& entity, TArgs&& ... args)
        {
            OTR_STATIC_ASSERT(TComponent::Id > 0, "Component Id must be greater than 0.")
            OTR_ASSERT(entity.IsValid(), "Entity must be valid.")

            auto component = TComponent(std::forward<TArgs>(args)...);

            return TryAddComponent(entity.GetId(), TComponent::Id, sizeof(TComponent), (Byte*) &component);
        }

        /**
         * @brief Retrieves a component of an entity.
         *
         * @tparam TComponent The component type.
         *
         * @param entity The entity.
         *
         * @return A pointer to the component.
         */
        template<typename TComponent>
        requires IsComponent<TComponent>
        TComponent* GetComponent(const Entity& entity)
        {
            OTR_STATIC_ASSERT(TComponent::Id > 0, "Component Id must be greater than 0.")
            OTR_ASSERT(entity.IsValid(), "Entity must be valid.")

            ArchetypeFingerprint fingerprint;
            if (!m_EntityToFingerprint.TryGet(entity.GetId(), &fingerprint))
                return nullptr;

            OTR_ASSERT(m_FingerprintToArchetype.ContainsKey(fingerprint), "Archetype not found.")

            return m_FingerprintToArchetype[fingerprint]->GetComponentsForEntityUnsafe<TComponent>(entity.GetId());
        }

        /**
         * @brief Removes a component from an entity.
         *
         * @tparam TComponent The component type.
         *
         * @param entity The entity.
         *
         * @return True if the component was removed, false otherwise.
         */
        template<typename TComponent>
        requires IsComponent<TComponent>
        bool TryRemoveComponent(const Entity& entity)
        {
            OTR_STATIC_ASSERT(TComponent::Id > 0, "Component Id must be greater than 0.")
            OTR_ASSERT(entity.IsValid(), "Entity must be valid.")

            return TryRemoveComponent(entity.GetId(), TComponent::Id);
        }

        /**
         * @brief Checks if an entity has a component.
         *
         * @tparam TComponent The component type.
         *
         * @param entity The entity.
         *
         * @return True if the entity has the component, false otherwise.
         */
        template<typename TComponent>
        requires IsComponent<TComponent>
        [[nodiscard]] OTR_INLINE bool HasComponent(const Entity& entity) const
        {
            OTR_STATIC_ASSERT(TComponent::Id > 0, "Component Id must be greater than 0.")
            OTR_ASSERT(entity.IsValid(), "Entity must be valid.")

            return HasComponent(entity.GetId(), TComponent::Id);
        }

        /**
         * @brief Iterates over all entities with a set of components.
         *
         * @tparam TComponent The component type.
         * @tparam TComponents The rest of the components.
         *
         * @param callback The function to call for each set of components.
         */
        template<typename TComponent, typename... TComponents>
        requires IsComponent<TComponent> && AreComponents<TComponents...>
        void ForEach(const Function<void(TComponent*, TComponents* ...)>& callback) const
        {
            OTR_STATIC_ASSERT(TComponent::Id > 0, "Component Id must be greater than 0.")
            OTR_DEBUG_BLOCK(
                if (VariadicArgs<TComponents...>::GetSize() > 0)
                {
                    OTR_STATIC_ASSERT((AreUnique<TComponent, TComponents...>::value), "Components must be unique.");

                    ([&]
                    {
                        OTR_STATIC_ASSERT(TComponents::Id > 0, "Component Id must be greater than 0.")
                    }(), ...);
                }
            )

            OTR_ASSERT(m_FingerprintToArchetype.GetCount() > 0, "No archetypes registered for fingerprint.")
            OTR_ASSERT(m_ComponentToFingerprints.ContainsKey(TComponent::Id),
                       "Component with id {0} not registered.",
                       TComponent::Id)

            if constexpr (VariadicArgs<TComponents...>::GetSize() == 0)
            {
                for (const auto& fingerprint: *m_ComponentToFingerprints[TComponent::Id])
                    m_FingerprintToArchetype[fingerprint]->ForEach(callback);
            }
            else
            {
                UInt64 index;

                OTR_VALIDATE(m_ComponentToFingerprintIndex.TryGet(TComponent::Id, &index),
                             "Component with id {0} not found.",
                             TComponent::Id)

                ArchetypeFingerprint requestedFingerprint;
                requestedFingerprint.Set(index, true);

                ([&]
                {
                    OTR_VALIDATE(m_ComponentToFingerprintIndex.TryGet(TComponents::Id, &index),
                                 "Component with id {0} not found.",
                                 TComponents::Id)
                    requestedFingerprint.Set(index, true);
                }(), ...);

                HashSet<ArchetypeFingerprint> fingerprints;

                for (const auto& fingerprint: *m_ComponentToFingerprints[TComponent::Id])
                    if (fingerprint.Includes(requestedFingerprint))
                        fingerprints.TryAdd(fingerprint);

                ([&]
                {
                    OTR_ASSERT(m_ComponentToFingerprints.ContainsKey(TComponents::Id),
                               "Component with id {0} not registered.",
                               TComponents::Id)

                    for (const auto& fingerprint: *m_ComponentToFingerprints[TComponents::Id])
                        if (fingerprint.Includes(requestedFingerprint))
                            fingerprints.TryAdd(fingerprint);
                }(), ...);

                for (const auto& fingerprint: fingerprints)
                    m_FingerprintToArchetype[fingerprint]->ForEach(callback);
            }
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
                OTR_STATIC_ASSERT(TComponent::Id > 0, "Component Id must be greater than 0.")

                WithInternal(TComponent::Id);

                return *this;
            }

            /**
             * @brief Builds the archetype.
             *
             * @return The archetype.
             */
            Archetype Build();

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
                OTR_STATIC_ASSERT(TComponent::Id > 0, "Component Id must be greater than 0.")

                auto component = TComponent(std::forward<TArgs>(args)...);
                SetComponentDataInternal(TComponent::Id, sizeof(TComponent), (Byte*) &component);

                return *this;
            }

            /**
             * @brief Builds the entity.
             *
             * @return The entity.
             */
            Entity Build();

        private:
            EntityManager* m_EntityManager;
            Entity               m_Entity;
            ArchetypeFingerprint m_Fingerprint;
            ComponentData        m_ComponentData;

            /**
             * @brief Sets the component data. Used internally.
             */
            void SetComponentDataInternal(UInt64 componentId, UInt64 componentSize, const Byte* componentData);
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
            EntityBuilderFromArchetype(EntityManager* entityManager, Entity entity, const Archetype& archetype);

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
                OTR_STATIC_ASSERT(TComponent::Id > 0, "Component Id must be greater than 0.")

                auto component = TComponent(std::forward<TArgs>(args)...);
                SetComponentDataInternal(TComponent::Id, sizeof(TComponent), (Byte*) &component);

                return *this;
            }

            /**
             * @brief Builds the entity.
             *
             * @return The entity.
             */
            Entity Build();

        private:
            EntityManager* m_EntityManager;
            Entity               m_Entity;
            ArchetypeFingerprint m_FingerprintTrack;
            ComponentData        m_ComponentData;

            /**
             * @brief Sets the component data. Used internally.
             */
            void SetComponentDataInternal(UInt64 componentId, UInt64 componentSize, const Byte* componentData);
        };

        // Entity Registry
        List<Entity>                               m_Entities;
        Dictionary<Entity, UInt64>                 m_EntityToIndex;
        Dictionary<EntityId, ArchetypeFingerprint> m_EntityToFingerprint;
        Stack<Entity>                              m_EntitiesToAdd;
        Dictionary<EntityId, ComponentData>        m_EntityToComponentDataToAdd;

        // Component Registry
        Dictionary<ComponentId, UInt64>                     m_ComponentToFingerprintIndex;
        Dictionary<ComponentId, List<ArchetypeFingerprint>> m_ComponentToFingerprints;
        bool                                                m_ComponentsLock = false;

        // Archetype Registry
        Dictionary<ArchetypeFingerprint, Archetype>      m_FingerprintToArchetype;
        Dictionary<ArchetypeFingerprint, Archetype>      m_FingerprintToArchetypeToAdd;
        Dictionary<ArchetypeFingerprint, List<EntityId>> m_FingerprintToEntitiesToRemove;

        /**
         * @brief Destroys the entity manager.
         */
        void Destroy();

        /**
         * @brief Creates an entity.
         *
         * @return The entity.
         */
        Entity CreateEntityInternal();

        /**
         * @brief Refreshes the list of archetypes.
         *
         * @note This method:
         * <br>- Adds components to archetypes.
         * <br>- Adds new archetypes.
         * <br>- Removes entities from archetypes.
         */
        void RefreshArchetypes();

        /**
         * @brief Refreshes the list of entities.
         *
         * @note This method:
         * <br>- Adds new entities.
         * <br>- Removes invalid entities.
         */
        void RefreshEntities();

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
            OTR_STATIC_ASSERT(TComponent::Id > 0, "Component Id must be greater than 0.")

            if (m_ComponentToFingerprintIndex.ContainsKey(TComponent::Id))
                return;

            UInt64 index = m_ComponentToFingerprintIndex.GetCount();
            m_ComponentToFingerprintIndex.TryAdd(TComponent::Id, index);

            m_ComponentToFingerprints.TryAdd(TComponent::Id, List<ArchetypeFingerprint>());

            if constexpr (VariadicArgs<TComponents...>::GetSize() > 0)
                RegisterComponentsInternal<TComponents...>();
        }

        /**
         * @brief Adds a component to an entity.
         *
         * @param entityId The entity id.
         * @param componentId The component id.
         * @param componentSize The component size.
         * @param componentData The component data.
         *
         * @return True if the component was added, false otherwise.
         */
        bool TryAddComponent(EntityId entityId, UInt64 componentId, UInt64 componentSize, const Byte* componentData);

        /**
         * @brief Removes a component from an entity.
         *
         * @param entityId The entity id.
         * @param componentId The component id.
         *
         * @return True if the component was removed, false otherwise.
         */
        bool TryRemoveComponent(EntityId entityId, ComponentId componentId);

        /**
         * @brief Checks if an entity has a component.
         *
         * @param entityId The entity id.
         * @param componentId The component id.
         *
         * @return True if the entity has the component, false otherwise.
         */
        [[nodiscard]] bool HasComponent(EntityId entityId, ComponentId componentId) const;
    };
}

#endif //OTTERENGINE_ENTITYMANAGER_H
