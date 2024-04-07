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
        m_FingerprintToEntitiesToRemove.ClearDestructive();
        m_ArchetypesToAdd.ClearDestructive();
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
                        componentIds.Add(componentId);

//                    Archetype archetype{ fingerprint, componentIds };
//                    archetype.TryAddComponentDataUnsafe(entityId,
//                                                        componentIds.GetData(),
//                                                        componentDataContainer.GetComponentSizes(),
//                                                        componentDataContainer.GetComponentData());
//
//                    m_ArchetypesToAdd.Push(std::move(archetype));

                    m_ArchetypesToAdd.Push({ fingerprint, componentIds });
                }

                if (!m_EntityToFingerprint.ContainsKey(entityId))
                    m_EntityToFingerprint.TryAdd(entityId, fingerprint);
                else
                    *m_EntityToFingerprint[entityId] = std::move(fingerprint);
            }

            m_EntityToComponentDataToAdd.Clear();
        }

        if (!m_ArchetypesToAdd.IsEmpty())
        {
            Archetype archetype;
            while (m_ArchetypesToAdd.TryPop(&archetype))
                m_FingerprintToArchetype.TryAdd(archetype.GetFingerprint(), archetype);

            m_ArchetypesToAdd.Clear();
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

    void EntityManager::AddComponent(EntityId entityId,
                                     UInt64 componentId,
                                     UInt64 componentSize,
                                     const Byte* componentData)
    {
        if (m_EntityToComponentDataToAdd.ContainsKey(entityId))
        {
            m_EntityToComponentDataToAdd[entityId]->Add(componentId, componentSize, componentData);

            return;
        }

        ArchetypeFingerprint fingerprint;
        OTR_VALIDATE(m_EntityToFingerprint.TryGet(entityId, &fingerprint), "Entity must be mapped to a fingerprint.")
        OTR_ASSERT(m_FingerprintToArchetype.ContainsKey(fingerprint), "Fingerprint must be mapped to an archetype.")

        ComponentData componentDataContainer;
        m_FingerprintToArchetype[fingerprint]->GetComponentDataForEntityUnsafe(entityId, &componentDataContainer);
        componentDataContainer.Add(componentId, componentSize, componentData);

        m_EntityToComponentDataToAdd.TryAdd(entityId, componentDataContainer);

        if (!m_FingerprintToEntitiesToRemove.ContainsKey(fingerprint))
            m_FingerprintToEntitiesToRemove.TryAdd(fingerprint, List<EntityId>());

        m_FingerprintToEntitiesToRemove[fingerprint]->Add(entityId);
    }

    bool EntityManager::TryRemoveComponent(const EntityId entityId, const ComponentId componentId)
    {
//        ArchetypeFingerprint oldFingerprint;
//        if (!m_EntityToFingerprint.TryGet(entityId, &oldFingerprint))
//            return false;
//
//        OTR_ASSERT(m_FingerprintToArchetype.ContainsKey(oldFingerprint), "Fingerprint must be mapped to an archetype.")
//
//        UInt64 fingerprintIndex;
//        if (!m_ComponentToFingerprintIndex.TryGet(componentId, &fingerprintIndex))
//            return false;
//
//        OTR_ASSERT(oldFingerprint.Get(fingerprintIndex), "Entity does not have component with id {0}.", componentId)
//
//        ArchetypeFingerprint newFingerprint = oldFingerprint;
//        newFingerprint.Set(fingerprintIndex, false);
//
//        if (!m_FingerprintToComponentDataToAdd.ContainsKey(newFingerprint))
//            m_FingerprintToComponentDataToAdd.TryAdd(newFingerprint, Dictionary<EntityId, List<ComponentData>>());
//
//        if (!m_FingerprintToComponentDataToAdd[newFingerprint]->ContainsKey(entityId))
//        {
//            m_FingerprintToComponentDataToAdd[newFingerprint]->TryAdd(entityId, List<ComponentData>());
//
//            UInt64      totalComponentsInArchetype = oldFingerprint.GetTrueCount();
//            ComponentId componentIds[totalComponentsInArchetype];
//            UInt64      componentSizes[totalComponentsInArchetype];
//
//            m_FingerprintToArchetype[oldFingerprint]->GetComponentDataForEntityUnsafe(entityId,
//                                                                                      componentIds,
//                                                                                      componentSizes,
//                                                                                      nullptr);
//
//            UInt64 allComponentsDataBufferSize = 0;
//
//            for (UInt64 i = 0; i < totalComponentsInArchetype; ++i)
//                allComponentsDataBufferSize += componentSizes[i];
//
//            Byte allComponentsDataBuffer[allComponentsDataBufferSize];
//
//            m_FingerprintToArchetype[oldFingerprint]->GetComponentDataForEntityUnsafe(entityId,
//                                                                                      componentIds,
//                                                                                      componentSizes,
//                                                                                      allComponentsDataBuffer);
//
//            UInt64 dataIndex = 0;
//            UInt64 sizeIndex = 0;
//
//            while (sizeIndex < totalComponentsInArchetype)
//            {
//                if (componentIds[sizeIndex] != componentId)
//                {
//                    Byte currentComponentData[componentSizes[sizeIndex]];
//                    MemorySystem::MemoryCopy(currentComponentData,
//                                             &allComponentsDataBuffer[dataIndex],
//                                             componentSizes[sizeIndex]);
//
//                    (*m_FingerprintToComponentDataToAdd[newFingerprint])[entityId]->Add({
//                                                                                            componentIds[sizeIndex],
//                                                                                            currentComponentData,
//                                                                                            componentSizes[sizeIndex]
//                                                                                        });
//                }
//
//                dataIndex += componentSizes[sizeIndex];
//                ++sizeIndex;
//            }
//        }
//
//        if (!m_FingerprintToEntitiesToRemove.ContainsKey(oldFingerprint))
//            m_FingerprintToEntitiesToRemove.TryAdd(oldFingerprint, List<EntityId>());
//
//        m_FingerprintToEntitiesToRemove[oldFingerprint]->Add(entityId);

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

        for (const auto& componentId: m_ComponentIds)
        {
            OTR_ASSERT(m_EntityManager->m_ComponentToFingerprints.ContainsKey(componentId),
                       "Component must be registered.")

            if (!m_EntityManager->m_ComponentToFingerprints[componentId]->Contains(m_Fingerprint))
                m_EntityManager->m_ComponentToFingerprints[componentId]->Add(m_Fingerprint);
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