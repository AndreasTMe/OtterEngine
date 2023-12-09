#ifndef OTTERENGINE_LAYERS_H
#define OTTERENGINE_LAYERS_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Memory.h"
#include "Core/Time.h"

namespace Otter
{
    /**
     * @brief The Layer class is an abstraction that represents a layer in an application.
     */
    class Layer
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~Layer() = default;

        /**
         * @brief Enables the layer.
         *
         * @note This function is virtual, so it can be overridden by derived classes if necessary.
         */
        virtual void OnEnable() { m_IsEnabled = true; }

        /**
         * @brief Disables the layer.
         *
         * @note This function is virtual, so it can be overridden by derived classes if necessary.
         */
        virtual void OnDisable() { m_IsEnabled = false; }

        /**
         * @brief Called on each update of the layer. Derived classes must implement this function
         * to define their specific behavior during an update.
         *
         * @param step The time step between updates.
         */
        virtual void OnUpdate(const TimeStep& step) = 0;

        /**
         * @brief Check if the layer is enabled.
         *
         * @return True if the layer is enabled, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsEnabled() const { return m_IsEnabled; }

    protected:
        /**
         * @brief Explicit default constructor. The layer is enabled by default, so if it must be disabled on creation,
         * the user must pass false as argument. Otherwise, it can be left empty.
         */
        explicit Layer(bool isEnabled = true)
            : m_IsEnabled(isEnabled)
        {
        }

        bool m_IsEnabled = false;
    };
}

#endif //OTTERENGINE_LAYERS_H
