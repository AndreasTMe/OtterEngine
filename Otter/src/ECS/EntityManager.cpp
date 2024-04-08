#include "ECS/EntityManager.h"

namespace Otter
{
    EntityManager::~EntityManager()
    {
        m_Entities.ClearDestructive();
        m_EntityToIndex.ClearDestructive();
        m_EntitiesToAdd.ClearDestructive();
        m_EntityToFingerprint.ClearDestructive();
        m_EntityToComponentDataToAdd.ClearDestructive();

        m_ComponentToFingerprintIndex.ClearDestructive();
        m_ComponentToFingerprints.ClearDestructive();

        m_FingerprintToArchetype.ClearDestructive();
        m_FingerprintToArchetypeToAdd.ClearDestructive();
        m_FingerprintToEntitiesToRemove.ClearDestructive();
    }

    EntityManager::EntityBuilder EntityManager::CreateEntity()
    {
        OTR_ASSERT(m_ComponentsLock, "Component mask must be locked before creating entities.")

        return { this, CreateEntityInternal() };
    }

    EntityManager::EntityBuilderFromArchetype EntityManager::CreateEntityFromArchetype(const Archetype& archetype)
    {
        OTR_ASSERT(m_ComponentsLock, "Component mask must be locked before creating entities.")

        return { this, CreateEntityInternal(), archetype };
    }

    void EntityManager::DestroyEntity(const Entity& entity)
    {
        OTR_ASSERT(m_ComponentsLock, "Component mask must be locked before destroying entities.")

        // Invalidate the entity
        UInt64 index;
        if (!m_EntityToIndex.TryGet(entity, &index))
            return;

        m_Entities[index].m_IsValid = false;

        // Add to a collection of entities to remove from an archetype
        ArchetypeFingerprint fingerprint;
        OTR_VALIDATE(m_EntityToFingerprint.TryGet(entity.GetId(), &fingerprint),
                     "Entity with id '{0}' must be mapped to an archetype.", entity.GetId())

        if (!m_FingerprintToEntitiesToRemove.ContainsKey(fingerprint))
            m_FingerprintToEntitiesToRemove.TryAdd(fingerprint, List<EntityId>());

        m_FingerprintToEntitiesToRemove[fingerprint]->Add(entity.GetId());
    }

    void EntityManager::RefreshManagerData()
    {
        OTR_ASSERT(m_ComponentsLock, "Component mask must be locked before refreshing entities.")

        RefreshArchetypes();
        RefreshEntities();
    }

    Entity EntityManager::CreateEntityInternal()
    {
        static EntityId id = 0;
        id++;

        auto entity = Entity{ id };
        while (m_EntityToIndex.ContainsKey(entity))
            entity.m_Id = ++id;

        return entity;
    }

    void EntityManager::RefreshArchetypes()
    {
        if (!m_EntityToComponentDataToAdd.IsEmpty())
        {
            for (const auto& [entityId, componentDataContainer]: m_EntityToComponentDataToAdd)
            {
                ArchetypeFingerprint fingerprint;
                for (const auto& [componentId, _a, _b]: componentDataContainer)
                {
                    UInt64 index;
                    if (!m_ComponentToFingerprintIndex.TryGet(componentId, &index))
                        continue;

                    fingerprint.Set(index, true);
                }

                if (!m_FingerprintToArchetype.ContainsKey(fingerprint))
                {
                    List<ComponentId> componentIds;
                    for (const auto& [componentId, _a, _b]: componentDataContainer)
                    {
                        OTR_ASSERT(m_ComponentToFingerprints.ContainsKey(componentId), "Component must be registered.")

                        componentIds.Add(componentId);

                        if (!m_ComponentToFingerprints[componentId]->Contains(fingerprint))
                            m_ComponentToFingerprints[componentId]->Add(fingerprint);
                    }

                    if (!m_FingerprintToArchetypeToAdd.ContainsKey(fingerprint))
                    {
                        Archetype archetype{ fingerprint, componentIds };
                        archetype.TryAddComponentDataUnsafe(entityId,
                                                            componentIds.GetData(),
                                                            componentDataContainer.GetComponentSizes(),
                                                            componentDataContainer.GetComponentData());

                        m_FingerprintToArchetypeToAdd.TryAdd(fingerprint, archetype);
                    }
                    else
                    {
                        m_FingerprintToArchetypeToAdd[fingerprint]->TryAddComponentDataUnsafe(
                            entityId,
                            componentIds.GetData(),
                            componentDataContainer.GetComponentSizes(),
                            componentDataContainer.GetComponentData());
                    }
                }

                if (!m_EntityToFingerprint.ContainsKey(entityId))
                    m_EntityToFingerprint.TryAdd(entityId, fingerprint);
                else
                    *m_EntityToFingerprint[entityId] = std::move(fingerprint);
            }

            m_EntityToComponentDataToAdd.ClearDestructive();
        }

        if (!m_FingerprintToArchetypeToAdd.IsEmpty())
        {
            for (const auto& [fingerprint, archetype]: m_FingerprintToArchetypeToAdd)
                m_FingerprintToArchetype.TryAdd(fingerprint, archetype);

            m_FingerprintToArchetypeToAdd.ClearDestructive();
        }

        if (!m_FingerprintToEntitiesToRemove.IsEmpty())
        {
            for (const auto& [fingerprint, entities]: m_FingerprintToEntitiesToRemove)
            {
                if (!m_FingerprintToArchetype.ContainsKey(fingerprint))
                    continue;

                for (const auto& entity: entities)
                    m_FingerprintToArchetype[fingerprint]->TryRemoveComponentData(entity);
            }

            m_FingerprintToEntitiesToRemove.ClearDestructive();
        }
    }

    void EntityManager::RefreshEntities()
    {
        if (!m_EntitiesToAdd.IsEmpty())
        {
            Entity e;
            while (m_EntitiesToAdd.TryPop(&e))
            {
                m_Entities.Add(e);

                auto index = m_Entities.GetCount() - 1;
                m_EntityToIndex.TryAdd(e, index);
            }

            m_EntitiesToAdd.Clear();
        }

        if (!m_Entities.IsEmpty())
        {
            List<UInt64> indicesToRemove;

            for (auto& entity: m_Entities)
            {
                if (entity.IsValid())
                    continue;

                UInt64 index;
                if (m_EntityToIndex.TryGet(entity, &index))
                {
                    indicesToRemove.Add(index);

                    OTR_VALIDATE(m_EntityToIndex.TryRemove(entity), "Unable to remove entity from index map.")
                    OTR_VALIDATE(m_EntityToIndex.TryAdd(m_Entities[index], index), "Unable to add entity to index map.")
                    OTR_VALIDATE(m_EntityToFingerprint.TryRemove(entity.GetId()),
                                 "Unable to remove entity from fingerprint map.")
                }
            }

            for (auto& index: indicesToRemove)
            {
                OTR_VALIDATE(m_Entities.TryRemoveAt(index), "Unable to remove entity from entity list.")
            }
        }
    }

    bool EntityManager::TryAddComponent(EntityId entityId,
                                        UInt64 componentId,
                                        UInt64 componentSize,
                                        const Byte* componentData)
    {
        if (m_EntityToComponentDataToAdd.ContainsKey(entityId))
        {
            m_EntityToComponentDataToAdd[entityId]->Add(componentId, componentSize, componentData);

            return true;
        }

        ArchetypeFingerprint fingerprint;
        if (!m_EntityToFingerprint.TryGet(entityId, &fingerprint))
            return false;

        OTR_ASSERT(m_FingerprintToArchetype.ContainsKey(fingerprint), "Fingerprint must be mapped to an archetype.")

        ComponentData componentDataContainer;
        m_FingerprintToArchetype[fingerprint]->GetComponentDataForEntityUnsafe(entityId, &componentDataContainer);
        componentDataContainer.Add(componentId, componentSize, componentData);

        m_EntityToComponentDataToAdd.TryAdd(entityId, componentDataContainer);

        if (!m_FingerprintToEntitiesToRemove.ContainsKey(fingerprint))
            m_FingerprintToEntitiesToRemove.TryAdd(fingerprint, List<EntityId>());

        m_FingerprintToEntitiesToRemove[fingerprint]->Add(entityId);

        return true;
    }

    bool EntityManager::TryRemoveComponent(EntityId entityId, ComponentId componentId)
    {
        ArchetypeFingerprint fingerprint;
        if (!m_EntityToFingerprint.TryGet(entityId, &fingerprint))
            return false;

        OTR_ASSERT(m_FingerprintToArchetype.ContainsKey(fingerprint), "Fingerprint must be mapped to an archetype.")

        if (!m_EntityToComponentDataToAdd.ContainsKey(entityId))
        {
            ComponentData componentDataContainer;
            m_FingerprintToArchetype[fingerprint]->GetComponentDataForEntityUnsafe(entityId, &componentDataContainer);
            componentDataContainer.Remove(componentId);

            m_EntityToComponentDataToAdd.TryAdd(entityId, componentDataContainer);

            if (!m_FingerprintToEntitiesToRemove.ContainsKey(fingerprint))
                m_FingerprintToEntitiesToRemove.TryAdd(fingerprint, List<EntityId>());

            m_FingerprintToEntitiesToRemove[fingerprint]->Add(entityId);
        }
        else
        {
            m_EntityToComponentDataToAdd[entityId]->Remove(componentId);
        }

        return true;
    }

    bool EntityManager::HasComponent(const EntityId entityId, const ComponentId componentId) const
    {
        UInt64 index;
        OTR_VALIDATE(m_ComponentToFingerprintIndex.TryGet(componentId, &index),
                     "Component with id '{0}'must be registered.", componentId)

        ArchetypeFingerprint fingerprint;
        OTR_VALIDATE(m_EntityToFingerprint.TryGet(entityId, &fingerprint),
                     "Entity with id '{0}' must be mapped to an archetype.", entityId)

        return fingerprint.Get(index);
    }

    EntityManager::ArchetypeBuilder::ArchetypeBuilder(EntityManager* entityManager)
        : m_EntityManager(entityManager), m_Fingerprint()
    {
    }

    EntityManager::ArchetypeBuilder::~ArchetypeBuilder()
    {
        m_EntityManager = nullptr;
        m_Fingerprint.ClearDestructive();
        m_ComponentIds.ClearDestructive();
    }

    Archetype EntityManager::ArchetypeBuilder::Build()
    {
        Archetype archetype(m_Fingerprint, m_ComponentIds);

        if (!m_EntityManager->m_FingerprintToArchetype.ContainsKey(m_Fingerprint))
        {
            if (!m_EntityManager->m_FingerprintToArchetypeToAdd.ContainsKey(m_Fingerprint))
                m_EntityManager->m_FingerprintToArchetypeToAdd.TryAdd(m_Fingerprint, archetype);
            else
                *m_EntityManager->m_FingerprintToArchetypeToAdd[m_Fingerprint] = archetype;
        }

        return archetype;
    }

    void EntityManager::ArchetypeBuilder::WithInternal(const UInt64 componentId)
    {
        OTR_ASSERT(m_EntityManager->m_ComponentsLock, "Component registration must be locked.")
        OTR_ASSERT(m_EntityManager->m_ComponentToFingerprintIndex.ContainsKey(componentId),
                   "Component must be registered.")

        UInt64 index = 0;
        m_EntityManager->m_ComponentToFingerprintIndex.TryGet(componentId, &index);

        m_Fingerprint.Set(index, true);
        m_ComponentIds.Add(componentId);
    }

    EntityManager::EntityBuilder::EntityBuilder(EntityManager* entityManager, Entity entity)
        : m_EntityManager(entityManager), m_Entity(std::move(entity)), m_Fingerprint(), m_ComponentData()
    {
        OTR_ASSERT(m_Entity.IsValid(), "Entity must be valid.")
    }

    EntityManager::EntityBuilder::~EntityBuilder()
    {
        m_EntityManager = nullptr;
        m_Fingerprint.ClearDestructive();
    }

    Entity EntityManager::EntityBuilder::Build()
    {
        m_EntityManager->m_EntitiesToAdd.Push(m_Entity);

        OTR_ASSERT(!m_EntityManager->m_EntityToComponentDataToAdd.ContainsKey(m_Entity.GetId()),
                   "Entity already exists. This should never happen at this stage!")

        OTR_VALIDATE(m_EntityManager->m_EntityToComponentDataToAdd.TryAdd(m_Entity.GetId(), m_ComponentData),
                     "Unable to add component data to entity.")

        return m_Entity;
    }

    void EntityManager::EntityBuilder::SetComponentDataInternal(const UInt64 componentId,
                                                                const UInt64 componentSize,
                                                                const Byte* const componentData)
    {
        OTR_ASSERT(m_EntityManager->m_ComponentsLock, "Component registration must be locked.")
        OTR_ASSERT(m_EntityManager->m_ComponentToFingerprintIndex.ContainsKey(componentId),
                   "Component must be registered.")

        UInt64 index = 0;
        OTR_VALIDATE(m_EntityManager->m_ComponentToFingerprintIndex.TryGet(componentId, &index),
                     "Component with id '{0}'must be registered.", componentId)

        OTR_ASSERT(!m_Fingerprint.Get(index), "Component already set.")

        m_Fingerprint.Set(index, true);
        m_ComponentData.Add(componentId, componentSize, componentData);
    }

    EntityManager::EntityBuilderFromArchetype::EntityBuilderFromArchetype(EntityManager* entityManager,
                                                                          Entity entity,
                                                                          const Archetype& archetype)
        : m_EntityManager(entityManager), m_Entity(std::move(entity)),
          m_FingerprintTrack(), m_ComponentData()
    {
        OTR_ASSERT(m_Entity.IsValid(), "Entity must be valid.")
        OTR_ASSERT(archetype.GetComponentCount() > 0, "Archetype must have components.")

        m_FingerprintTrack = archetype.GetFingerprint();
    }

    EntityManager::EntityBuilderFromArchetype::~EntityBuilderFromArchetype()
    {
        m_EntityManager = nullptr;
        m_FingerprintTrack.ClearDestructive();
    }

    Entity EntityManager::EntityBuilderFromArchetype::Build()
    {
        OTR_ASSERT(m_FingerprintTrack.GetTrueCount() == 0, "Not all components were set.")

        m_EntityManager->m_EntitiesToAdd.Push(m_Entity);

        OTR_ASSERT(!m_EntityManager->m_EntityToComponentDataToAdd.ContainsKey(m_Entity.GetId()),
                   "Entity already exists. This should never happen at this stage!")

        OTR_VALIDATE(m_EntityManager->m_EntityToComponentDataToAdd.TryAdd(m_Entity.GetId(), m_ComponentData),
                     "Unable to add component data to entity.")

        return m_Entity;
    }

    void EntityManager::EntityBuilderFromArchetype::SetComponentDataInternal(const UInt64 componentId,
                                                                             const UInt64 componentSize,
                                                                             const Byte* const componentData)
    {
        OTR_ASSERT(m_EntityManager->m_ComponentsLock, "Component registration must be locked.")
        OTR_ASSERT(m_EntityManager->m_ComponentToFingerprintIndex.ContainsKey(componentId),
                   "Component must be registered.")

        UInt64 index = 0;
        OTR_VALIDATE(m_EntityManager->m_ComponentToFingerprintIndex.TryGet(componentId, &index),
                     "Component with id '{0}'must be registered.", componentId)

        OTR_ASSERT(m_FingerprintTrack.Get(index), "Component already set.")

        m_FingerprintTrack.Set(index, false);
        m_ComponentData.Add(componentId, componentSize, componentData);
    }
}