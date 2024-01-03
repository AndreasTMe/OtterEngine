#include "ECS/EntityManager.h"

namespace Otter
{
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
        if (!m_EntityToIndex.TryGetIndex(entity, &index))
            return;

        m_Entities[index].m_Id = 0;
    }

    void EntityManager::RefreshEntities()
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

        m_EntitiesToAdd.Push(entity);

        return entity;
    }

    void EntityManager::PopulateNewArchetypes()
    {
        if (m_ArchetypesToAdd.IsEmpty())
            return;

        Archetype archetype;
        while (m_ArchetypesToAdd.TryPop(&archetype))
            m_FingerprintToArchetype.TryAdd(archetype.GetFingerprint(), archetype);

        m_ArchetypesToAdd.Clear();
    }

    void EntityManager::PopulateNewEntities()
    {
        if (m_EntitiesToAdd.IsEmpty())
            return;

        Entity e;
        while (m_EntitiesToAdd.TryPop(&e))
        {
            m_Entities.Add(e);
            m_EntityToIndex.TryAdd(e, m_Entities.GetCount() - 1);
        }

        m_EntitiesToAdd.Clear();
    }

    void EntityManager::CleanUpEntities()
    {
        for (auto& entity: m_Entities)
        {
            if (entity.IsValid())
                continue;

            UInt64 index;
            if (m_EntityToIndex.TryGetIndex(entity, &index))
            {
                m_Entities.TryRemoveAt(index);

                m_EntityToIndex.TryRemove(entity);
                m_EntityToIndex.TryAdd(m_Entities[index], index);
            }
        }
    }
}