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
         * @param componentMask The component mask.
         * @param componentIds The component ids.
         */
        Archetype(const BitSet& componentMask, const List<ComponentId>& componentIds)
        {
            OTR_ASSERT_MSG(componentMask.GetTrueCount() == componentIds.GetCount(),
                           "Component mask and component ids must have the same count.")

            m_ComponentMask = componentMask;

            for (UInt64 i = 0; i < componentIds.GetCount(); ++i)
                m_ComponentIdToData.TryAdd(componentIds[i], UnsafeList());
        }

        /**
         * @brief Destructor.
         */
        ~Archetype()
        {
            m_EntityIds.ClearDestructive();
            m_ComponentIdToData.ClearDestructive();
            m_EntityIdToBufferPosition.ClearDestructive();
        }

        /**
         * @brief Copy constructor.
         */
        Archetype(const Archetype& other)
        {
            m_EntityIds                = other.m_EntityIds;
            m_ComponentIdToData        = other.m_ComponentIdToData;
            m_EntityIdToBufferPosition = other.m_EntityIdToBufferPosition;
        }

        /**
         * @brief Move constructor.
         */
        Archetype(Archetype&& other) noexcept
        {
            m_EntityIds                = std::move(other.m_EntityIds);
            m_ComponentIdToData        = std::move(other.m_ComponentIdToData);
            m_EntityIdToBufferPosition = std::move(other.m_EntityIdToBufferPosition);

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

            m_EntityIds                = std::move(other.m_EntityIds);
            m_ComponentIdToData        = std::move(other.m_ComponentIdToData);
            m_EntityIdToBufferPosition = std::move(other.m_EntityIdToBufferPosition);

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
            return m_EntityIds == other.m_EntityIds
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
         * @brief Adds component data to the archetype.
         *
         * @param entityId The entity id.
         * @param componentData The component data.
         *
         * @return True if the component data was added, false otherwise.
         */
        bool TryAddComponentData(const EntityId entityId, const List<ComponentData>& componentData)
        {
            OTR_ASSERT_MSG(componentData.GetCount() == m_ComponentIdToData.GetCount(),
                           "Passed component data count must be equal to archetype's component count.")
            OTR_DEBUG_BLOCK(
                for (UInt64 i = 0; i < componentData.GetCount(); ++i)
                {
                    OTR_ASSERT_MSG(m_ComponentIdToData.ContainsKey(componentData[i].Id),
                                   "Archetype does not contain component id.")
                }
            )

            if (m_EntityIdToBufferPosition.ContainsKey(entityId))
                return false;

            OTR_VALIDATE(m_EntityIdToBufferPosition.TryAdd(entityId, m_EntityIds.GetCount()))

            m_EntityIds.Add(entityId);

            for (UInt64 i = 0; i < componentData.GetCount(); ++i)
                m_ComponentIdToData[componentData[i].Id]->Add(componentData[i].Data, componentData[i].Size);

            return true;
        }

        /**
         * @brief Removes component data from the archetype.
         *
         * @param entityId The entity id.
         *
         * @return True if the component data was removed, false otherwise.
         */
        bool TryRemoveComponentData(const EntityId entityId)
        {
            UInt64 index;
            if (!m_EntityIdToBufferPosition.TryGet(entityId, &index))
                return false;

            OTR_VALIDATE(m_EntityIds.TryRemoveAt(index))

            for (auto& [componentId, componentData]: m_ComponentIdToData)
            {
                OTR_VALIDATE(m_ComponentIdToData[componentId]->TryRemoveAt(index))
            }

            OTR_VALIDATE(m_EntityIdToBufferPosition.TryRemove(entityId))

            return true;
        }

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
            OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component Id must be greater than 0.")
            OTR_ASSERT_MSG(HasComponent<TComponent>(), "Archetype must have component.")

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
            OTR_STATIC_ASSERT_MSG(TComponent::Id > 0, "Component Id must be greater than 0.")
            OTR_ASSERT_MSG(HasComponent<TComponent>(), "Archetype must have component.")
            OTR_ASSERT_MSG(m_EntityIdToBufferPosition.ContainsKey(entityId), "Entity id must belong to the archetype.")

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
        BitSet                              m_ComponentMask;
        List<EntityId>               m_EntityIds;
        Dictionary<ComponentId, UnsafeList> m_ComponentIdToData;
        Dictionary<EntityId, UInt64> m_EntityIdToBufferPosition;

        friend class EntityManager;
    };
}

#endif //OTTERENGINE_ARCHETYPE_H
