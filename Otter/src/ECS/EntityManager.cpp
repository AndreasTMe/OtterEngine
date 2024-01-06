#include "ECS/EntityManager.h"

namespace Otter
{
    EntityManager::~EntityManager()
    {
        m_Entities.ClearDestructive();
        m_EntityToIndex.ClearDestructive();
        m_EntitiesToAdd.ClearDestructive();

        m_ComponentToFingerprintIndex.ClearDestructive();
        m_ComponentToFingerprints.ClearDestructive();

        m_FingerprintToArchetype.ClearDestructive();
    }

    EntityManager::EntityBuilder EntityManager::CreateEntity()
    {
        OTR_ASSERT_MSG(m_ComponentsLock, "Component mask must be locked before creating entities.")

        return { this, CreateEntityInternal() };
    }

    EntityManager::EntityBuilderFromArchetype EntityManager::CreateEntityFromArchetype(const Archetype& archetype)
    {
        OTR_ASSERT_MSG(m_ComponentsLock, "Component mask must be locked before creating entities.")

        return { this, CreateEntityInternal(), archetype };
    }

    void EntityManager::DestroyEntity(const Entity& entity)
    {
        OTR_ASSERT_MSG(m_ComponentsLock, "Component mask must be locked before destroying entities.")

        UInt64 index;
        if (!m_EntityToIndex.TryGet(entity, &index))
            return;

        m_Entities[index].m_IsValid = false;
    }

    void EntityManager::RefreshManagedData()
    {
        OTR_ASSERT_MSG(m_ComponentsLock, "Component mask must be locked before refreshing entities.")

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

        OTR_ASSERT_MSG(m_EntitiesToAdd.GetCount() == m_FingerprintToComponentDataToAdd.GetCount(),
                       "Entity count must match component data count.")

        if (m_FingerprintToComponentDataToAdd.IsEmpty())
            return;

        UInt64 index = 0;
        m_FingerprintToComponentDataToAdd.ForEach(
            [&](auto& fingerprint, auto& componentData)
            {
                if (!m_FingerprintToArchetype.ContainsKey(fingerprint))
                {
                    List<ComponentId> componentIds;
                    for (UInt64       i = 0; i < componentData.GetCount(); i++)
                        componentIds.Add(componentData[i].Id);

                    m_FingerprintToArchetype.TryAdd(fingerprint, Archetype(fingerprint, componentIds));
                }

                m_FingerprintToArchetype[fingerprint]
                    ->TryAddComponentData(m_EntitiesToAdd[index].GetId(), componentData);
                index++;
            });

        m_FingerprintToComponentDataToAdd.Clear();
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

                OTR_VALIDATE(m_EntityToIndex.TryRemove(entity))
                OTR_VALIDATE(m_EntityToIndex.TryAdd(m_Entities[index], index))
            }
        }

        for (auto& index: indicesToRemove)
        {
            OTR_VALIDATE(m_Entities.TryRemoveAt(index))
        }
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
        OTR_ASSERT_MSG(m_EntityManager->m_ComponentsLock, "Component registration must be locked.")
        OTR_ASSERT_MSG(m_EntityManager->m_ComponentToFingerprintIndex.ContainsKey(componentId),
                       "Component must be registered.")

        UInt64 index = 0;
        m_EntityManager->m_ComponentToFingerprintIndex.TryGet(componentId, &index);

        m_Fingerprint.Set(index, true);
        m_ComponentIds.Add(componentId);
    }

    EntityManager::EntityBuilder::EntityBuilder(EntityManager* entityManager, Entity entity)
        : m_EntityManager(entityManager), m_Entity(std::move(entity)), m_Fingerprint(), m_ComponentData()
    {
        OTR_ASSERT_MSG(m_Entity.IsValid(), "Entity must be valid.")
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
            m_EntityManager->m_FingerprintToComponentDataToAdd.TryAdd(std::move(m_Fingerprint),
                                                                      std::move(m_ComponentData));
        else
            for (auto& componentData: m_ComponentData)
                m_EntityManager->m_FingerprintToComponentDataToAdd[m_Fingerprint]->Add(componentData);

        return m_Entity;
    }

    void EntityManager::EntityBuilder::SetComponentDataInternal(UInt64 componentId,
                                                                ComponentData&& componentData)
    {
        OTR_ASSERT_MSG(m_EntityManager->m_ComponentsLock, "Component registration must be locked.")
        OTR_ASSERT_MSG(m_EntityManager->m_ComponentToFingerprintIndex.ContainsKey(componentId),
                       "Component must be registered.")

        UInt64 index = 0;
        OTR_VALIDATE(m_EntityManager->m_ComponentToFingerprintIndex.TryGet(componentId, &index))

        OTR_ASSERT_MSG(!m_Fingerprint.Get(index), "Component already set.")

        m_Fingerprint.Set(index, true);
        m_ComponentData.Add(componentData);
    }

    EntityManager::EntityBuilderFromArchetype::EntityBuilderFromArchetype(EntityManager* entityManager,
                                                                          Entity entity,
                                                                          Archetype archetype)
        : m_EntityManager(entityManager), m_Entity(std::move(entity)),
          m_ComponentData(), m_Archetype(std::move(archetype))
    {
        OTR_ASSERT_MSG(m_Entity.IsValid(), "Entity must be valid.")
        OTR_ASSERT_MSG(m_Archetype.GetComponentCount() > 0, "Archetype must have components.")

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
        OTR_ASSERT_MSG(m_FingerprintTrack.GetTrueCount() == 0, "Not all components were set.")

        m_EntityManager->m_EntitiesToAdd.Push(m_Entity);

        const auto fingerprint = m_Archetype.GetFingerprint();
        if (!m_EntityManager->m_FingerprintToComponentDataToAdd.ContainsKey(fingerprint))
            m_EntityManager->m_FingerprintToComponentDataToAdd.TryAdd(fingerprint, m_ComponentData);
        else
            for (auto& componentData: m_ComponentData)
                m_EntityManager->m_FingerprintToComponentDataToAdd[fingerprint]->Add(componentData);

        return m_Entity;
    }

    void EntityManager::EntityBuilderFromArchetype::SetComponentDataInternal(UInt64 componentId,
                                                                             ComponentData&& componentData)
    {
        OTR_ASSERT_MSG(m_EntityManager->m_ComponentsLock, "Component registration must be locked.")
        OTR_ASSERT_MSG(m_EntityManager->m_ComponentToFingerprintIndex.ContainsKey(componentId),
                       "Component must be registered.")

        UInt64 index = 0;
        OTR_VALIDATE(m_EntityManager->m_ComponentToFingerprintIndex.TryGet(componentId, &index))

        m_FingerprintTrack.Set(index, false);
        m_ComponentData.Add(componentData);
    }
}