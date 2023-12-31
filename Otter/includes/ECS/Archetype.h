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
            m_EntityIdToIndex.ClearDestructive();
            m_ComponentIdToData.ClearDestructive();
        }

        /**
         * @brief Copy constructor.
         */
        Archetype(const Archetype& other)
        {
            m_EntityIdToIndex   = other.m_EntityIdToIndex;
            m_ComponentIdToData = other.m_ComponentIdToData;
        }

        /**
         * @brief Move constructor.
         */
        Archetype(Archetype&& other) noexcept
        {
            m_EntityIdToIndex   = std::move(other.m_EntityIdToIndex);
            m_ComponentIdToData = std::move(other.m_ComponentIdToData);

            other.m_EntityIdToIndex.ClearDestructive();
            other.m_ComponentIdToData.ClearDestructive();
        }

        /**
         * @brief Copy assignment operator.
         */
        Archetype& operator=(const Archetype& other)
        {
            if (this == &other)
                return *this;

            m_EntityIdToIndex   = other.m_EntityIdToIndex;
            m_ComponentIdToData = other.m_ComponentIdToData;

            return *this;
        }

        /**
         * @brief Move assignment operator.
         */
        Archetype& operator=(Archetype&& other) noexcept
        {
            if (this == &other)
                return *this;

            m_EntityIdToIndex   = std::move(other.m_EntityIdToIndex);
            m_ComponentIdToData = std::move(other.m_ComponentIdToData);

            other.m_EntityIdToIndex.ClearDestructive();
            other.m_ComponentIdToData.ClearDestructive();

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
            return m_EntityIdToIndex == other.m_EntityIdToIndex && m_ComponentIdToData == other.m_ComponentIdToData;
        }

        /**
         * @brief Inequality operator.
         *
         * @param other The other archetype.
         *
         * @return True if the archetypes are not equal, false otherwise.
         */
        bool operator!=(const Archetype& other) const noexcept { return !(*this == other); }

        bool TryPushComponentData(const EntityId entityId, const List<ComponentData>& componentData)
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

            if (m_EntityIdToIndex.ContainsKey(entityId))
                return false;

            const auto index = m_EntityIdToIndex.GetCount();
            m_EntityIdToIndex.TryAdd(entityId, index);

            for (UInt64 i = 0; i < componentData.GetCount(); ++i)
                m_ComponentIdToData[componentData[i].Id]->Add(componentData[i].Data, componentData[i].Size);

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
         * @brief Gets the entity count of the archetype.
         *
         * @return The entity count.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetEntityCount() const { return m_EntityIdToIndex.GetCount(); }

        /**
         * @brief Gets the component count of the archetype.
         *
         * @return The component count.
         */
        [[nodiscard]] OTR_INLINE UInt64 GetComponentCount() const { return m_ComponentIdToData.GetCount(); }

    private:
        BitSet                              m_ComponentMask;
        Dictionary<EntityId, UInt64>        m_EntityIdToIndex;
        Dictionary<ComponentId, UnsafeList> m_ComponentIdToData;

        friend class EntityManager;
    };
}

#endif //OTTERENGINE_ARCHETYPE_H
