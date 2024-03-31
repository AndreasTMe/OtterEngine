#ifndef OTTERENGINE_ARCHETYPE_H
#define OTTERENGINE_ARCHETYPE_H

#include "Core/Collections/List.h"
#include "Core/Collections/UnsafeList.h"
#include "Core/Collections/Dictionary.h"
#include "ECS/Entity.h"
#include "Components/IComponent.h"

namespace Otter
{
    class EntityManager;

    /**
     * @brief Alias for the archetype's fingerprint.
     */
    using ArchetypeFingerprint = BitSet;

    /**
     * @brief The archetype class for the entity-component system.
     */
    class Archetype final
    {
    public:
        /**
         * @brief Constructor.
         */
        Archetype() = default;

        /**
         * @brief Constructor.
         *
         * @param fingerprint The component mask.
         * @param componentIds The component ids.
         */
        Archetype(const ArchetypeFingerprint& fingerprint, const List<ComponentId>& componentIds)
        {
            m_Fingerprint = fingerprint;

            for (UInt64 i = 0; i < componentIds.GetCount(); ++i)
                m_ComponentIdToData.TryAdd(componentIds[i], UnsafeList());

            OTR_ASSERT(m_Fingerprint.GetTrueCount() == m_ComponentIdToData.GetCount(),
                       "Archetype fingerprint must have the same amount of true bits as component ids.")
        }

        /**
         * @brief Destructor.
         */
        ~Archetype()
        {
            m_Fingerprint.ClearDestructive();
            m_EntityIds.ClearDestructive();
            m_ComponentIdToData.ClearDestructive();
            m_EntityIdToBufferPosition.ClearDestructive();
        }

        /**
         * @brief Copy constructor.
         */
        Archetype(const Archetype& other)
        {
            m_Fingerprint = other.m_Fingerprint;
            m_EntityIds                = other.m_EntityIds;
            m_ComponentIdToData        = other.m_ComponentIdToData;
            m_EntityIdToBufferPosition = other.m_EntityIdToBufferPosition;
        }

        /**
         * @brief Move constructor.
         */
        Archetype(Archetype&& other) noexcept
        {
            m_Fingerprint = std::move(other.m_Fingerprint);
            m_EntityIds                = std::move(other.m_EntityIds);
            m_ComponentIdToData        = std::move(other.m_ComponentIdToData);
            m_EntityIdToBufferPosition = std::move(other.m_EntityIdToBufferPosition);

            other.m_Fingerprint.ClearDestructive();
            other.m_EntityIds.ClearDestructive();
            other.m_ComponentIdToData.ClearDestructive();
            other.m_EntityIdToBufferPosition.ClearDestructive();
        }

        /**
         * @brief Copy assignment operator.
         */
        Archetype& operator=(const Archetype& other)
        {
            if (this == &other)
                return *this;

            m_Fingerprint = other.m_Fingerprint;
            m_EntityIds                = other.m_EntityIds;
            m_ComponentIdToData        = other.m_ComponentIdToData;
            m_EntityIdToBufferPosition = other.m_EntityIdToBufferPosition;

            return *this;
        }

        /**
         * @brief Move assignment operator.
         */
        Archetype& operator=(Archetype&& other) noexcept
        {
            if (this == &other)
                return *this;

            m_Fingerprint = std::move(other.m_Fingerprint);
            m_EntityIds                = std::move(other.m_EntityIds);
            m_ComponentIdToData        = std::move(other.m_ComponentIdToData);
            m_EntityIdToBufferPosition = std::move(other.m_EntityIdToBufferPosition);

            other.m_Fingerprint.ClearDestructive();
            other.m_EntityIds.ClearDestructive();
            other.m_ComponentIdToData.ClearDestructive();
            other.m_EntityIdToBufferPosition.ClearDestructive();

            return *this;
        }

        /**
         * @brief Equality operator.
         *
         * @param other The other archetype.
         *
         * @return True if the archetypes are equal, false otherwise.
         */
        bool operator==(const Archetype& other) const noexcept
        {
            return m_Fingerprint == other.m_Fingerprint
                   && m_EntityIds == other.m_EntityIds
                   && m_ComponentIdToData == other.m_ComponentIdToData
                   && m_EntityIdToBufferPosition == other.m_EntityIdToBufferPosition;
        }

        /**
         * @brief Inequality operator.
         *
         * @param other The other archetype.
         *
         * @return True if the archetypes are not equal, false otherwise.
         */
        bool operator!=(const Archetype& other) const noexcept { return !(*this == other); }

        /**
         * @brief Gets the requested components of the archetype.
         *
         * @tparam TComponent The type of the component.
         * @tparam TComponents The types of the rest of the components.
         *
         * @return The requested components as a single pointer or a tuple of pointers, depending on the amount of
         * components requested.
         */
        template<typename TComponent, typename... TComponents>
        requires IsComponent<TComponent> && AreComponents<TComponents...>
        [[nodiscard]] decltype(auto) GetComponents() const
        {
            OTR_STATIC_ASSERT(TComponent::Id > 0, "Component Id must be greater than 0.")
            OTR_ASSERT(HasComponent<TComponent>(), "Archetype must have component.")

            if constexpr (VariadicArgs<TComponents...>::GetSize() == 0)
                return m_ComponentIdToData[TComponent::Id]->template GetData<TComponent>();
            else
            {
                std::tuple<TComponent*, TComponents* ...> componentData;
                std::get<0>(componentData) = m_ComponentIdToData[TComponent::Id]->template GetData<TComponent>();

                if constexpr (VariadicArgs<TComponents...>::GetSize() == 1)
                    std::get<1>(componentData) = GetComponents<TComponents...>();
                else
                    componentData = std::tuple_cat(componentData, GetComponents<TComponents...>());

                return componentData;
            }
        }

        /**
         * @brief Gets the requested components of the archetype for an entity.
         *
         * @tparam TComponent The type of the component.
         * @tparam TComponents The types of the rest of the components.
         *
         * @param entityId The entity id.
         *
         * @return The requested components as a single pointer or a tuple of pointers, depending on the amount of
         * components requested.
         */
        template<typename TComponent, typename... TComponents>
        requires IsComponent<TComponent> && AreComponents<TComponents...>
        [[nodiscard]] decltype(auto) GetComponentsForEntity(const EntityId entityId) const
        {
            OTR_STATIC_ASSERT(TComponent::Id > 0, "Component Id must be greater than 0.")
            OTR_ASSERT(HasComponent<TComponent>(), "Archetype must have component.")
            OTR_ASSERT(m_EntityIdToBufferPosition.ContainsKey(entityId), "Entity id must belong to the archetype.")

            if constexpr (VariadicArgs<TComponents...>::GetSize() == 0)
                return &m_ComponentIdToData[TComponent::Id]
                    ->template GetData<TComponent>()[*m_EntityIdToBufferPosition[entityId]];
            else
            {
                std::tuple<TComponent*, TComponents * ...> componentData;
                std::get<0>(componentData) = &m_ComponentIdToData[TComponent::Id]
                    ->template GetData<TComponent>()[*m_EntityIdToBufferPosition[entityId]];

                if constexpr (VariadicArgs<TComponents...>::GetSize() == 1)
                    std::get<1>(componentData) = GetComponentsForEntity<TComponents...>(entityId);
                else
                    componentData = std::tuple_cat(componentData, GetComponentsForEntity<TComponents...>(entityId));

                return componentData;
            }
        }

        /**
         * @brief Adds component data to the archetype.
         *
         * @param entityId The entity id.
         * @param componentData The component data.
         *
         * @return True if the component data was added, false otherwise.
         */
        bool TryAddComponentData(const EntityId entityId, const List<ComponentData>& componentData)
        {
            OTR_ASSERT(componentData.GetCount() == m_ComponentIdToData.GetCount(),
                       "Passed component data count must be equal to archetype's component count.")
            OTR_DEBUG_BLOCK(
                for (UInt64 i = 0; i < componentData.GetCount(); ++i)
                {
                    OTR_ASSERT(m_ComponentIdToData.ContainsKey(componentData[i].Id),
                               "Archetype does not contain component id.")
                }
            )

            if (m_EntityIdToBufferPosition.ContainsKey(entityId))
                return false;

            OTR_VALIDATE(m_EntityIdToBufferPosition.TryAdd(entityId, m_EntityIds.GetCount()),
                         "Failed to add entity with id '{0}' to buffer position dictionary.", entityId)

            m_EntityIds.Add(entityId);

            for (UInt64 i = 0; i < componentData.GetCount(); ++i)
                m_ComponentIdToData[componentData[i].Id]->Add(componentData[i].Data, componentData[i].Size);

            return true;
        }

        /**
         * @brief Removes an entity and all its component data from the archetype.
         *
         * @param entityId The entity id.
         *
         * @return True if the entity-component data were removed, false otherwise.
         */
        bool TryRemoveComponentData(const EntityId entityId)
        {
            if (!m_EntityIdToBufferPosition.ContainsKey(entityId))
                return false;

            UInt64 index     = *m_EntityIdToBufferPosition[entityId];
            UInt64 lastIndex = m_EntityIds.GetCount() - 1;

            OTR_VALIDATE(m_EntityIds.TryRemoveAt(index), "Failed to remove entity with id '{0}'.", entityId)
            OTR_VALIDATE(m_EntityIdToBufferPosition.TryRemove(entityId),
                         "Failed to remove entity with id '{0}' from buffer position dictionary.", entityId)

            if (index != lastIndex)
            {
                OTR_VALIDATE(m_EntityIdToBufferPosition.TryAdd(m_EntityIds[index], index),
                             "Failed to move entity with id '{0}' to buffer position dictionary.", m_EntityIds[index])
            }

            for (auto& [componentId, storedComponentData]: m_ComponentIdToData)
            {
                OTR_VALIDATE(storedComponentData.TryRemoveAt(index),
                             "Failed to remove component data for entity with id '{0}'.", entityId)
            }

            return true;
        }

        /**
         * @brief Checks if the archetype has a component.
         *
         * @tparam TComponent The type of the component.
         *
         * @return True if the archetype has the component, false otherwise.
         */
        template<typename TComponent>
        requires IsComponent<TComponent>
        [[nodiscard]] OTR_INLINE bool HasComponent() const { return m_ComponentIdToData.ContainsKey(TComponent::Id); }

        /**
         * @brief Gets the archetype's fingerprint.
         *
         * @return The archetype's fingerprint.
         */
        [[nodiscard]] OTR_INLINE const ArchetypeFingerprint& GetFingerprint() const { return m_Fingerprint; }

        /**
         * @brief Gets the entity count of the archetype.
         *
         * @return The entity count.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetEntityCount() const { return m_EntityIds.GetCount(); }

        /**
         * @brief Gets the component count of the archetype.
         *
         * @return The component count.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetComponentCount() const { return m_ComponentIdToData.GetCount(); }

    private:
        ArchetypeFingerprint         m_Fingerprint;
        List<EntityId>               m_EntityIds;
        Dictionary<ComponentId, UnsafeList> m_ComponentIdToData;
        Dictionary<EntityId, UInt64> m_EntityIdToBufferPosition;

        friend class EntityManager;
    };
}

#endif //OTTERENGINE_ARCHETYPE_H
