#ifndef OTTERENGINE_ARCHETYPE_H
#define OTTERENGINE_ARCHETYPE_H

#include "Core/Collections/List.h"
#include "Core/Collections/UnsafeList.h"
#include "Core/Collections/Dictionary.h"
#include "ECS/Entity.h"
#include "ECS/ComponentData.h"

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
        friend class EntityManager;

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
            m_Fingerprint              = other.m_Fingerprint;
            m_EntityIds                = other.m_EntityIds;
            m_ComponentIdToData        = other.m_ComponentIdToData;
            m_EntityIdToBufferPosition = other.m_EntityIdToBufferPosition;
        }

        /**
         * @brief Move constructor.
         */
        Archetype(Archetype&& other) noexcept
        {
            m_Fingerprint              = std::move(other.m_Fingerprint);
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

            m_Fingerprint              = other.m_Fingerprint;
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

            m_Fingerprint              = std::move(other.m_Fingerprint);
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
        ArchetypeFingerprint                m_Fingerprint;
        List<EntityId>                      m_EntityIds;
        Dictionary<ComponentId, UnsafeList> m_ComponentIdToData;
        Dictionary<EntityId, UInt64>        m_EntityIdToBufferPosition;

    OTR_TEST_VISIBLE:
        /**
         * @brief Adds component data to the archetype.
         *
         * @param entityId The entity id.
         * @param componentIds An array of the component ids of the archetype.
         * @param componentSizes An array of each of the archetype's components' sizes.
         * @param componentData The archetype's component data of the entity in a continuous buffer.
         *
         * @return True if the component data was added, false otherwise.
         *
         * @note The `componentIds` and `componentSizes` arrays must be pre-allocated and their sizes must be equal to
         * the archetype's component count.
         * @note The `componentData` buffer must be pre-allocated and its size must be equal to the sum of the
         * archetype's component sizes.
         */
        bool TryAddComponentDataUnsafe(const EntityId entityId,
                                       const ComponentId* const componentIds,
                                       const UInt64* const componentSizes,
                                       const Byte* const componentData)
        {
            OTR_ASSERT(componentIds && componentSizes && componentData,
                       "Component ids, sizes and data must not be null.")

            if (m_EntityIdToBufferPosition.ContainsKey(entityId))
                return false;

            OTR_VALIDATE(m_EntityIdToBufferPosition.TryAdd(entityId, m_EntityIds.GetCount()),
                         "Failed to add entity with id '{0}' to buffer position dictionary.", entityId)

            m_EntityIds.Add(entityId);

            for (UInt64 i = 0, j = 0; i < m_ComponentIdToData.GetCount(); ++i)
            {
                OTR_ASSERT(m_ComponentIdToData.ContainsKey(componentIds[i]), "Archetype does not contain component id.")

                m_ComponentIdToData[componentIds[i]]->Add(&componentData[j], componentSizes[i]);
                j += componentSizes[i];
            }

            return true;
        }

        /**
         * @brief Gets the component data for an entity.
         *
         * @param entityId The entity id.
         * @param componentData The container to store the component data.
         */
        void GetComponentDataForEntityUnsafe(const EntityId entityId, ComponentData* componentData) const
        {
            OTR_ASSERT(m_EntityIdToBufferPosition.ContainsKey(entityId), "Entity id must belong to the archetype.")
            OTR_ASSERT(componentData, "Component data container should not be null.")

            const auto bufferPosition = *m_EntityIdToBufferPosition[entityId];

            for (const auto& [componentId, storedComponentData]: m_ComponentIdToData)
                componentData->Add(componentId,
                                   storedComponentData.GetOffset(),
                                   storedComponentData.operator[]<Byte>(bufferPosition));
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
        [[nodiscard]] decltype(auto) GetComponentsForEntityUnsafe(const EntityId entityId) const
        {
            OTR_STATIC_ASSERT(TComponent::Id > 0, "Component Id must be greater than 0.")
            OTR_ASSERT(HasComponent<TComponent>(), "Archetype must have component.")
            OTR_ASSERT(m_EntityIdToBufferPosition.ContainsKey(entityId), "Entity id must belong to the archetype.")

            if constexpr (VariadicArgs<TComponents...>::GetSize() == 0)
                return m_ComponentIdToData[TComponent::Id]
                    ->template operator[]<TComponent>(*m_EntityIdToBufferPosition[entityId]);
            else
            {
                auto entityIndex = *m_EntityIdToBufferPosition[entityId];

                std::tuple<TComponent*, TComponents* ...> components;
                std::get<TComponent*>(components) = m_ComponentIdToData[TComponent::Id]
                    ->template operator[]<TComponent>(entityIndex);

                ([&]
                {
                    std::get<TComponents*>(components) = m_ComponentIdToData[TComponents::Id]
                        ->template operator[]<TComponents>(entityIndex);
                }(), ...);

                return components;
            }
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
         * @brief Iterates over all entities of the archetype.
         *
         * @tparam TComponent The component type.
         * @tparam TComponents The rest of the components.
         *
         * @param callback The function to call for entity.
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

            auto* component = m_ComponentIdToData[TComponent::Id]->template GetData<TComponent>();

            if constexpr (VariadicArgs<TComponents...>::GetSize() == 0)
            {
                for (UInt64 i = 0; i < m_EntityIds.GetCount(); ++i)
                    callback(&component[i]);
            }
            else
            {
                std::tuple<TComponents* ...> components;
                ([&]
                {
                    std::get<TComponents*>(components) = m_ComponentIdToData[TComponents::Id]
                        ->template GetData<TComponents>();
                }(), ...);

                std::apply([&](TComponents* ... rest)
                           {
                               for (UInt64 i = 0; i < m_EntityIds.GetCount(); ++i)
                                   callback(&component[i], &rest[i]...);
                           }, components);
            }
        }
    };
}

#endif //OTTERENGINE_ARCHETYPE_H
