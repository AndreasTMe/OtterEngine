#ifndef OTTERENGINE_ICOMPONENT_H
#define OTTERENGINE_ICOMPONENT_H

#include "Core/BaseTypes.h"

namespace Otter
{
    /// @brief Type alias for component Ids.
    using ComponentId = UInt16;

    /**
     * @brief Marker interface for all components to inherit from.
     *
     * @note IComponent has a protected constructor, deleted copy/move constructors and assignment operators, and no
     * virtual methods.
     * @note Should not be used for any other reason than marking a class as a component.
     */
    class IComponent
    {
    public:
        /**
         * @brief Destructor.
         */
        ~IComponent() = default;

        /**
         * @brief Deleted copy constructor.
         */
        IComponent(const IComponent& other) = delete;

        /**
         * @brief Deleted move constructor.
         */
        IComponent(IComponent&& other) noexcept = delete;

        /**
         * @brief Deleted copy assignment operator.
         */
        IComponent& operator=(const IComponent& other) = delete;

        /**
         * @brief Deleted move assignment operator.
         */
        IComponent& operator=(IComponent&& other) noexcept = delete;

    protected:
        /**
         * @brief Constructor.
         */
        IComponent() = default;
    };

    /**
     * @brief Concept for checking if a type is derived from IComponent and has a static Id.
     *
     * @tparam T Type to check.
     */
    template<typename T>
    concept IsComponent = IsDerivedFrom<T, IComponent> && requires { T::Id; };

    /**
     * @brief Concept for checking if some types are all derived from IComponent and have a static Id.
     *
     * @tparam TArgs Types to check.
     */
    template<typename... TArgs>
    concept AreComponents = (VariadicArgs<TArgs...>::template AllDerivedFrom<IComponent>())
                            && (requires { TArgs::Id; } && ...);

    /**
     * @brief Structure for storing component data.
     */
    struct ComponentData final
    {
    public:
        /// @brief The component data.
        Byte* Data = nullptr;

        /// @brief The component size.
        UInt64 Size = 0;

        /// @brief The component id.
        ComponentId Id = 0;

        /**
         * @brief Constructor.
         */
        ComponentData() = default;

        /**
         * @brief Constructor.
         *
         * @param id The component id.
         * @param data The component data.
         * @param size The component size.
         */
        ComponentData(ComponentId id, Byte* data, UInt64 size)
            : Id(id), Data(data), Size(size)
        {
        }

        /**
         * @brief Destructor.
         */
        ~ComponentData() = default;

        /**
         * @brief Copy constructor.
         */
        ComponentData(const ComponentData& other) = default;

        /**
         * @brief Move constructor.
         */
        ComponentData(ComponentData&& other) noexcept = default;

        /**
         * @brief Copy assignment operator.
         */
        ComponentData& operator=(const ComponentData& other) = default;

        /**
         * @brief Move assignment operator.
         */
        ComponentData& operator=(ComponentData&& other) noexcept = default;

        /**
         * @brief Equality operator.
         *
         * @param other The other data.
         *
         * @return True if the data are equal, false otherwise.
         */
        bool operator==(const ComponentData& other) const noexcept
        {
            return Id == other.Id && Data == other.Data && Size == other.Size;
        }

        /**
         * @brief Inequality operator.
         *
         * @param other The other data.
         *
         * @return True if the data are not equal, false otherwise.
         */
        bool operator!=(const ComponentData& other) const noexcept { return !(*this == other); }
    };
}

#endif //OTTERENGINE_ICOMPONENT_H
