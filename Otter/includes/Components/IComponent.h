#ifndef OTTERENGINE_ICOMPONENT_H
#define OTTERENGINE_ICOMPONENT_H

namespace Otter
{
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
}

#endif //OTTERENGINE_ICOMPONENT_H
