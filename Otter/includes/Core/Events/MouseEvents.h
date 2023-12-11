#ifndef OTTERENGINE_MOUSEEVENTS_H
#define OTTERENGINE_MOUSEEVENTS_H

#include "Core/Defines.h"
#include "Core/Input/MouseButton.h"
#include "Core/Events/Event.h"

namespace Otter
{
    /**
     * @brief Event that is fired when a mouse button is pressed.
     */
    class MouseButtonPressedEvent final : public Event
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param button The button that was pressed.
         */
        explicit MouseButtonPressedEvent(const MouseButton button)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse),
                    EventType::MouseButtonPressed)
        {
            Capture(button, 5);
        }

        /**
         * @brief Gets the button that was pressed.
         *
         * @return The button that was pressed.
         */
        [[nodiscard]] OTR_INLINE MouseButton GetMouseButton() const { return Get<MouseButton>(5); }
    };

    /**
     * @brief Event that is fired when a mouse button is released.
     */
    class MouseButtonReleasedEvent final : public Event
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param button The button that was released.
         */
        explicit MouseButtonReleasedEvent(const MouseButton button)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse),
                    EventType::MouseButtonReleased)
        {
            Capture(button, 5);
        }

        /**
         * @brief Gets the button that was released.
         *
         * @return The button that was released.
         */
        [[nodiscard]] OTR_INLINE MouseButton GetMouseButton() const { return Get<MouseButton>(5); }
    };

    /**
     * @brief Event that is fired when the mouse is scrolled.
     */
    class MouseScrollEvent final : public Event
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param isPositive Whether the scroll was positive or negative.
         */
        explicit MouseScrollEvent(const bool isPositive)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse),
                    EventType::MouseScroll)
        {
            Capture(isPositive, 7);
        }

        /**
         * @brief Gets whether the scroll was positive or negative.
         *
         * @return True if the scroll was positive, false if negative.
         */
        [[nodiscard]] OTR_INLINE bool IsPositive() const { return Get<bool>(7); }
    };

    /**
     * @brief Event that is fired when the mouse is moved.
     */
    class MouseMovedEvent final : public Event
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param x The x position of the mouse.
         * @param y The y position of the mouse.
         */
        explicit MouseMovedEvent(const UInt16 x, const UInt16 y)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse),
                    EventType::MouseMoved)
        {
            Capture(x, 8);
            Capture(y, 10);
        }

        /**
         * @brief Gets the x position of the mouse.
         *
         * @return The x position of the mouse.
         */
        [[nodiscard]] OTR_INLINE UInt16 GetX() const { return Get<UInt16>(8); }

        /**
         * @brief Gets the y position of the mouse.
         *
         * @return The y position of the mouse.
         */
        [[nodiscard]] OTR_INLINE UInt16 GetY() const { return Get<UInt16>(10); }
    };
}

#endif //OTTERENGINE_MOUSEEVENTS_H
