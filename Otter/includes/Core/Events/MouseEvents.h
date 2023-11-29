#ifndef OTTERENGINE_MOUSEEVENTS_H
#define OTTERENGINE_MOUSEEVENTS_H

#include "Core/Types.h"
#include "Core/Events/Event.h"

namespace Otter
{
    class MouseButtonPressedEvent final : public Event
    {
    public:
        explicit MouseButtonPressedEvent(const MouseButton button)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse),
                    EventType::MouseButtonPressed)
        {
            Capture(button, 5);
        }

        [[nodiscard]] OTR_INLINE MouseButton GetMouseButton() const { return Get<MouseButton>(5); }
    };

    class MouseButtonReleasedEvent final : public Event
    {
    public:
        explicit MouseButtonReleasedEvent(const MouseButton button)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse),
                    EventType::MouseButtonReleased)
        {
            Capture(button, 5);
        }

        [[nodiscard]] OTR_INLINE MouseButton GetMouseButton() const { return Get<MouseButton>(5); }
    };

    class MouseScrollEvent final : public Event
    {
    public:
        explicit MouseScrollEvent(const bool isPositive)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse),
                    EventType::MouseScroll)
        {
            Capture(isPositive, 7);
        }

        [[nodiscard]] OTR_INLINE bool IsPositive() const { return Get<bool>(7); }
    };

    class MouseMovedEvent final : public Event
    {
    public:
        explicit MouseMovedEvent(const UInt16 x, const UInt16 y)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse),
                    EventType::MouseMoved)
        {
            Capture(x, 8);
            Capture(y, 10);
        }

        [[nodiscard]] OTR_INLINE UInt16 GetX() const { return Get<UInt16>(8); }
        [[nodiscard]] OTR_INLINE UInt16 GetY() const { return Get<UInt16>(10); }
    };
}

#endif //OTTERENGINE_MOUSEEVENTS_H
