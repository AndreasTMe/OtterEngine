#ifndef OTTERENGINE_GLOBALACTIONS_H
#define OTTERENGINE_GLOBALACTIONS_H

#include "Core/Defines.h"
#include "Core/Delegates.h"

#include "Core/Events/WindowEvents.h"
#include "Core/Events/KeyboardEvents.h"
#include "Core/Events/MouseEvents.h"

namespace Otter
{
    /**
     * @brief Global actions for various events.
     */
    class GlobalActions final
    {
    public:
        /**
         * @brief Deleted copy constructor.
         */
        GlobalActions(GlobalActions&) = delete;

        /**
         * @brief Deleted copy constructor.
         */
        GlobalActions(const GlobalActions&) = delete;

        /**
         * @brief Deleted copy assignment operator.
         */
        GlobalActions& operator=(const GlobalActions&) = delete;

        /**
         * @brief Deleted move constructor.
         */
        GlobalActions(GlobalActions&&) = delete;

        /**
         * @brief Deleted move assignment operator.
         */
        GlobalActions& operator=(GlobalActions&&) = delete;

        /**
         * @brief Get the instance of the global actions.
         *
         * @return The instance of the global actions.
         */
        [[nodiscard]] OTR_INLINE static GlobalActions& GetInstance() { return s_GlobalActions; }

        /// @brief Action for when the window is closed.
        Func<bool, const WindowCloseEvent&> OnWindowClose;

        /// @brief Action for when the window is resized.
        Func<bool, const WindowResizeEvent&> OnWindowResize;

        /// @brief Action for when the window is minimized.
        Func<bool, const WindowMinimizedEvent&> OnWindowMinimized;

        /// @brief Action for when the window is maximized.
        Func<bool, const WindowMaximizedEvent&> OnWindowMaximized;

        /// @brief Action for when the window is restored.
        Func<bool, const WindowRestoredEvent&> OnWindowRestored;

        /// @brief Action for when a keyboard key is pressed.
        Func<bool, const KeyPressedEvent&> OnKeyPressed;

        /// @brief Action for when a keyboard key is released.
        Func<bool, const KeyReleasedEvent&> OnKeyReleased;

        /// @brief Action for when a keyboard key is repeated.
        Func<bool, const KeyRepeatEvent&> OnKeyRepeat;

        /// @brief Action for when a mouse button is pressed.
        Func<bool, const MouseButtonPressedEvent&> OnMouseButtonPressed;

        /// @brief Action for when a mouse button is released.
        Func<bool, const MouseButtonReleasedEvent&> OnMouseButtonReleased;

        /// @brief Action for when the mouse is scrolled.
        Func<bool, const MouseScrollEvent&> OnMouseScroll;

        /// @brief Action for when the mouse is moved.
        Func<bool, const MouseMovedEvent&> OnMouseMoved;

    private:
        /**
         * @brief Constructor.
         */
        GlobalActions() = default;

        /**
         * @brief Destructor.
         */
        ~GlobalActions() = default;

        static GlobalActions s_GlobalActions;
    };
}

#define OTR_GLOBAL_ACTIONS Otter::GlobalActions::GetInstance()

#endif //OTTERENGINE_GLOBALACTIONS_H

