#ifndef OTTERENGINE_ARCHETYPE_H
#define OTTERENGINE_ARCHETYPE_H

#include "Core/BaseTypes.h"
#include "Core/Collections/Dictionary.h"
#include "Core/Collections/UnsafeList.h"
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

    private:
        Dictionary<EntityId, UInt64>        m_EntityIdToIndex;
        Dictionary<ComponentId, UnsafeList> m_ComponentIdToData;

        void PushComponentData(const Entity& entity,
                               const List<ComponentId>& componentIds,
                               const List<UnsafeList>& componentData)
        {
            OTR_ASSERT_MSG(entity.IsValid(), "Entity must be valid.")
            OTR_DEBUG_BLOCK(
                if (m_ComponentIdToData.IsCreated() && !m_ComponentIdToData.IsEmpty())
                {
                    OTR_ASSERT_MSG(componentIds.GetCount() == m_ComponentIdToData.GetCount(),
                                   "Component Ids must be the same size as the component Ids in the archetype.")
                    OTR_ASSERT_MSG(componentData.GetCount() == m_ComponentIdToData.GetCount(),
                                   "Component Data must be the same size as the component Ids in the archetype.")
                }
                else
                {
                    OTR_ASSERT_MSG(componentIds.GetCount() == componentData.GetCount(),
                                   "Component Ids must be the same size as the component Data.")
                }
            )

            for (UInt64 i = 0; i < componentIds.GetCount(); ++i)
            {
                const auto& componentId = componentIds[i];
                const auto& component   = componentData[i];

                if (!m_ComponentIdToData.ContainsKey(componentId))
                    m_ComponentIdToData.TryAdd(componentId, component);
                else
                    m_ComponentIdToData[componentId]->Add(component);
            }

            m_EntityIdToIndex.TryAdd(entity.GetId(), m_EntityIdToIndex.GetCount());
        }

        friend class EntityManager;
    };
}

#endif //OTTERENGINE_ARCHETYPE_H
