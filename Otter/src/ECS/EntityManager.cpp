#include "ECS/EntityManager.h"

namespace Otter
{
    EntityManager::~EntityManager()
    {
        m_Entities.ClearDestructive();
        m_EntityToIndex.ClearDestructive();
        m_EntitiesToAdd.ClearDestructive();
        m_EntityToFingerprint.ClearDestructive();

        m_ComponentToFingerprintIndex.ClearDestructive();
        m_ComponentToFingerprints.ClearDestructive();

        m_ArchetypesToAdd.ClearDestructive();
        m_FingerprintToArchetype.ClearDestructive();
        m_FingerprintToComponentDataToAdd.ClearDestructive();
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

        PopulateNewArchetypes();
        PopulateNewEntities();
        CleanUpEntities();
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

    void EntityManager::PopulateNewArchetypes()
    {
        if (!m_ArchetypesToAdd.IsEmpty())
        {
            Archetype archetype;
            while (m_ArchetypesToAdd.TryPop(&archetype))
                m_FingerprintToArchetype.TryAdd(archetype.GetFingerprint(), archetype);

            m_ArchetypesToAdd.Clear();
        }

        if (!m_FingerprintToComponentDataToAdd.IsEmpty())
        {
            for (const auto& [fingerprint, entityComponentData]: m_FingerprintToComponentDataToAdd)
            {
                if (!m_FingerprintToArchetype.ContainsKey(fingerprint))
                {
                    List<ComponentId> componentIds;

                    for (const auto& [entity, componentData]: entityComponentData)
                        for (const auto& data: componentData)
                            componentIds.Add(data.Id);

                    m_FingerprintToArchetype.TryAdd(fingerprint, Archetype(fingerprint, componentIds));
                }

                for (const auto& [entity, componentData]: entityComponentData)
                {
                    m_FingerprintToArchetype[fingerprint]->TryAddComponentData(entity, componentData);

                    if (!m_EntityToFingerprint.ContainsKey(entity))
                        m_EntityToFingerprint.TryAdd(entity, fingerprint);
                    else
                        *m_EntityToFingerprint[entity] = fingerprint;
                }
            }

            m_FingerprintToComponentDataToAdd.Clear();
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

            m_FingerprintToEntitiesToRemove.Clear();
        }
    }

    void EntityManager::PopulateNewEntities()
    {
        if (m_EntitiesToAdd.IsEmpty())
            return;

        Entity e;
        while (m_EntitiesToAdd.TryPop(&e))
        {
            m_Entities.Add(e);

            auto index = m_Entities.GetCount() - 1;
            m_EntityToIndex.TryAdd(e, index);
        }

        m_EntitiesToAdd.Clear();
    }

    void EntityManager::CleanUpEntities()
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

    bool EntityManager::TryAddComponent(const EntityId entityId, const ComponentData& componentData)
    {
        ArchetypeFingerprint oldFingerprint;
        if (!m_EntityToFingerprint.TryGet(entityId, &oldFingerprint))
            return false;

        OTR_ASSERT(m_FingerprintToArchetype.ContainsKey(oldFingerprint), "Fingerprint must be mapped to an archetype.")

        UInt64 fingerprintIndex;
        if (!m_ComponentToFingerprintIndex.TryGet(componentData.Id, &fingerprintIndex))
            return false;

        OTR_ASSERT(!oldFingerprint.Get(fingerprintIndex), "Entity already has component with id {0}.", componentData.Id)

        ArchetypeFingerprint newFingerprint = oldFingerprint;
        newFingerprint.Set(fingerprintIndex, true);

        if (!m_FingerprintToComponentDataToAdd.ContainsKey(newFingerprint))
            m_FingerprintToComponentDataToAdd.TryAdd(newFingerprint, Dictionary<EntityId, List<ComponentData>>());

        if (!m_FingerprintToComponentDataToAdd[newFingerprint]->ContainsKey(entityId))
        {
            m_FingerprintToComponentDataToAdd[newFingerprint]->TryAdd(entityId, List<ComponentData>());

            UInt64      totalComponentsInArchetype = oldFingerprint.GetTrueCount();
            ComponentId componentIds[totalComponentsInArchetype];
            UInt64      componentSizes[totalComponentsInArchetype];

            m_FingerprintToArchetype[oldFingerprint]->GetComponentDataForEntityUnsafe(entityId,
                                                                                      componentIds,
                                                                                      componentSizes,
                                                                                      nullptr);

            UInt64 allComponentsDataBufferSize = 0;

            for (UInt64 i = 0; i < totalComponentsInArchetype; ++i)
                allComponentsDataBufferSize += componentSizes[i];

            Byte allComponentsDataBuffer[allComponentsDataBufferSize];

            m_FingerprintToArchetype[oldFingerprint]->GetComponentDataForEntityUnsafe(entityId,
                                                                                      componentIds,
                                                                                      componentSizes,
                                                                                      allComponentsDataBuffer);

            UInt64 dataIndex = 0;
            UInt64 sizeIndex = 0;

            while (sizeIndex < totalComponentsInArchetype)
            {
                Byte currentComponentData[componentSizes[sizeIndex]];
                MemorySystem::MemoryCopy(currentComponentData,
                                         &allComponentsDataBuffer[dataIndex],
                                         componentSizes[sizeIndex]);

                (*m_FingerprintToComponentDataToAdd[newFingerprint])[entityId]->Add({
                                                                                        componentIds[sizeIndex],
                                                                                        currentComponentData,
                                                                                        componentSizes[sizeIndex]
                                                                                    });

                dataIndex += componentSizes[sizeIndex];
                ++sizeIndex;
            }
        }

        (*m_FingerprintToComponentDataToAdd[newFingerprint])[entityId]->Add(componentData);

        if (!m_FingerprintToEntitiesToRemove.ContainsKey(oldFingerprint))
            m_FingerprintToEntitiesToRemove.TryAdd(oldFingerprint, List<EntityId>());

        m_FingerprintToEntitiesToRemove[oldFingerprint]->Add(entityId);

        return true;
    }

    bool EntityManager::TryRemoveComponent(const EntityId entityId, const ComponentId componentId)
    {
        ArchetypeFingerprint oldFingerprint;
        if (!m_EntityToFingerprint.TryGet(entityId, &oldFingerprint))
            return false;

        OTR_ASSERT(m_FingerprintToArchetype.ContainsKey(oldFingerprint), "Fingerprint must be mapped to an archetype.")

        UInt64 fingerprintIndex;
        if (!m_ComponentToFingerprintIndex.TryGet(componentId, &fingerprintIndex))
            return false;

        OTR_ASSERT(oldFingerprint.Get(fingerprintIndex), "Entity does not have component with id {0}.", componentId)

        ArchetypeFingerprint newFingerprint = oldFingerprint;
        newFingerprint.Set(fingerprintIndex, false);

        if (!m_FingerprintToComponentDataToAdd.ContainsKey(newFingerprint))
            m_FingerprintToComponentDataToAdd.TryAdd(newFingerprint, Dictionary<EntityId, List<ComponentData>>());

        if (!m_FingerprintToComponentDataToAdd[newFingerprint]->ContainsKey(entityId))
        {
            m_FingerprintToComponentDataToAdd[newFingerprint]->TryAdd(entityId, List<ComponentData>());

            UInt64      totalComponentsInArchetype = oldFingerprint.GetTrueCount();
            ComponentId componentIds[totalComponentsInArchetype];
            UInt64      componentSizes[totalComponentsInArchetype];

            m_FingerprintToArchetype[oldFingerprint]->GetComponentDataForEntityUnsafe(entityId,
                                                                                      componentIds,
                                                                                      componentSizes,
                                                                                      nullptr);

            UInt64 allComponentsDataBufferSize = 0;

            for (UInt64 i = 0; i < totalComponentsInArchetype; ++i)
                allComponentsDataBufferSize += componentSizes[i];

            Byte allComponentsDataBuffer[allComponentsDataBufferSize];

            m_FingerprintToArchetype[oldFingerprint]->GetComponentDataForEntityUnsafe(entityId,
                                                                                      componentIds,
                                                                                      componentSizes,
                                                                                      allComponentsDataBuffer);

            UInt64 dataIndex = 0;
            UInt64 sizeIndex = 0;

            while (sizeIndex < totalComponentsInArchetype)
            {
                if (componentIds[sizeIndex] != componentId)
                {
                    Byte currentComponentData[componentSizes[sizeIndex]];
                    MemorySystem::MemoryCopy(currentComponentData,
                                             &allComponentsDataBuffer[dataIndex],
                                             componentSizes[sizeIndex]);

                    (*m_FingerprintToComponentDataToAdd[newFingerprint])[entityId]->Add({
                                                                                            componentIds[sizeIndex],
                                                                                            currentComponentData,
                                                                                            componentSizes[sizeIndex]
                                                                                        });
                }

                dataIndex += componentSizes[sizeIndex];
                ++sizeIndex;
            }
        }

        if (!m_FingerprintToEntitiesToRemove.ContainsKey(oldFingerprint))
            m_FingerprintToEntitiesToRemove.TryAdd(oldFingerprint, List<EntityId>());

        m_FingerprintToEntitiesToRemove[oldFingerprint]->Add(entityId);

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
            m_EntityManager->m_ArchetypesToAdd.Push(archetype);

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
        m_ComponentData.ClearDestructive();
    }

    Entity EntityManager::EntityBuilder::Build()
    {
        m_EntityManager->m_EntitiesToAdd.Push(m_Entity);

        if (!m_EntityManager->m_FingerprintToComponentDataToAdd.ContainsKey(m_Fingerprint))
            m_EntityManager->m_FingerprintToComponentDataToAdd
                .TryAdd(m_Fingerprint, Dictionary<EntityId, List<ComponentData>>());

        if (!m_EntityManager->m_FingerprintToComponentDataToAdd[m_Fingerprint]->ContainsKey(m_Entity.GetId()))
            m_EntityManager->m_FingerprintToComponentDataToAdd[m_Fingerprint]
                ->TryAdd(m_Entity.GetId(), List<ComponentData>());

        for (auto& componentData: m_ComponentData)
            (*m_EntityManager->m_FingerprintToComponentDataToAdd[m_Fingerprint])[m_Entity.GetId()]->Add(componentData);

        m_EntityManager->m_EntityToFingerprint.TryAdd(m_Entity.GetId(), m_Fingerprint);

        return m_Entity;
    }

    void EntityManager::EntityBuilder::SetComponentDataInternal(UInt64 componentId,
                                                                ComponentData&& componentData)
    {
        OTR_ASSERT(m_EntityManager->m_ComponentsLock, "Component registration must be locked.")
        OTR_ASSERT(m_EntityManager->m_ComponentToFingerprintIndex.ContainsKey(componentId),
                   "Component must be registered.")

        UInt64 index = 0;
        OTR_VALIDATE(m_EntityManager->m_ComponentToFingerprintIndex.TryGet(componentId, &index),
                     "Component with id '{0}'must be registered.", componentId)

        OTR_ASSERT(!m_Fingerprint.Get(index), "Component already set.")

        m_Fingerprint.Set(index, true);
        m_ComponentData.Add(componentData);
    }

    EntityManager::EntityBuilderFromArchetype::EntityBuilderFromArchetype(EntityManager* entityManager,
                                                                          Entity entity,
                                                                          Archetype archetype)
        : m_EntityManager(entityManager), m_Entity(std::move(entity)),
          m_ComponentData(), m_Archetype(std::move(archetype))
    {
        OTR_ASSERT(m_Entity.IsValid(), "Entity must be valid.")
        OTR_ASSERT(m_Archetype.GetComponentCount() > 0, "Archetype must have components.")

        m_FingerprintTrack = m_Archetype.GetFingerprint();
    }

    EntityManager::EntityBuilderFromArchetype::~EntityBuilderFromArchetype()
    {
        m_EntityManager = nullptr;
        m_ComponentData.ClearDestructive();
        m_FingerprintTrack.ClearDestructive();
    }

    Entity EntityManager::EntityBuilderFromArchetype::Build()
    {
        OTR_ASSERT(m_FingerprintTrack.GetTrueCount() == 0, "Not all components were set.")

        m_EntityManager->m_EntitiesToAdd.Push(m_Entity);

        const auto fingerprint = m_Archetype.GetFingerprint();

        if (!m_EntityManager->m_FingerprintToComponentDataToAdd.ContainsKey(fingerprint))
            m_EntityManager->m_FingerprintToComponentDataToAdd
                .TryAdd(fingerprint, Dictionary<EntityId, List<ComponentData>>());

        if (!m_EntityManager->m_FingerprintToComponentDataToAdd[fingerprint]->ContainsKey(m_Entity.GetId()))
            m_EntityManager->m_FingerprintToComponentDataToAdd[fingerprint]
                ->TryAdd(m_Entity.GetId(), List<ComponentData>());

        for (auto& componentData: m_ComponentData)
            (*m_EntityManager->m_FingerprintToComponentDataToAdd[fingerprint])[m_Entity.GetId()]->Add(componentData);

        m_EntityManager->m_EntityToFingerprint.TryAdd(m_Entity.GetId(), fingerprint);

        return m_Entity;
    }

    void EntityManager::EntityBuilderFromArchetype::SetComponentDataInternal(UInt64 componentId,
                                                                             ComponentData&& componentData)
    {
        OTR_ASSERT(m_EntityManager->m_ComponentsLock, "Component registration must be locked.")
        OTR_ASSERT(m_EntityManager->m_ComponentToFingerprintIndex.ContainsKey(componentId),
                   "Component must be registered.")

        UInt64 index = 0;
        OTR_VALIDATE(m_EntityManager->m_ComponentToFingerprintIndex.TryGet(componentId, &index),
                     "Component with id '{0}'must be registered.", componentId)

        m_FingerprintTrack.Set(index, false);
        m_ComponentData.Add(componentData);
    }
}