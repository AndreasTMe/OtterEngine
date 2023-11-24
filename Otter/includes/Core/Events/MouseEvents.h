#ifndef OTTERENGINE_MOUSEEVENTS_H
#define OTTERENGINE_MOUSEEVENTS_H

#include "Core/Types.h"
#include "Core/Events/Event.h"

namespace Otter
{
    class MouseButtonPressedEvent final : public Event
    {
    public:
        explicit MouseButtonPressedEvent(const MouseButton button, const UInt8 count = 1)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse),
                    EventType::MouseButtonPressed)
        {
            Capture(button, 5);
            Capture(count, 6);
        }

        [[nodiscard]] OTR_INLINE MouseButton GetMouseButton() const { return Get<MouseButton>(5); }
        [[nodiscard]] OTR_INLINE UInt8 GetCount() const { return Get<UInt8>(6); }
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
        explicit MouseMovedEvent(const UInt32 x, const UInt32 y)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse),
                    EventType::MouseMoved)
        {
            Capture(x, 8);
            Capture(y, 10);
        }

        [[nodiscard]] OTR_INLINE UInt32 GetX() const { return Get<UInt32>(8); }
        [[nodiscard]] OTR_INLINE UInt32 GetY() const { return Get<UInt32>(10); }
    };

    class MouseDraggedEvent final : public Event
    {
    public:
        explicit MouseDraggedEvent(const MouseButton button,
                                   const UInt16 x,
                                   const UInt16 y,
                                   const Float32 dragTime)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse),
                    EventType::MouseDragged)
        {
            Capture(button, 5);
            Capture(x, 8);
            Capture(y, 10);
            Capture(dragTime, 12);
        }

        [[nodiscard]] OTR_INLINE MouseButton GetMouseButton() const { return Get<MouseButton>(5); }
        [[nodiscard]] OTR_INLINE UInt16 GetX() const { return Get<UInt16>(8); }
        [[nodiscard]] OTR_INLINE UInt16 GetY() const { return Get<UInt16>(10); }
        [[nodiscard]] OTR_INLINE Float32 GetDragTime() const { return Get<Float32>(12); }
    };

    class MouseDragStartedEvent final : public Event
    {
    public:
        explicit MouseDragStartedEvent(const MouseButton button,
                                       const UInt16 x,
                                       const UInt16 y)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse),
                    EventType::MouseDragStarted)
        {
            Capture(button, 5);
            Capture(x, 8);
            Capture(y, 10);
        }

        [[nodiscard]] OTR_INLINE MouseButton GetMouseButton() const { return Get<MouseButton>(5); }
        [[nodiscard]] OTR_INLINE UInt16 GetX() const { return Get<UInt16>(8); }
        [[nodiscard]] OTR_INLINE UInt16 GetY() const { return Get<UInt16>(10); }
    };

    class MouseDragEndedEvent final : public Event
    {
    public:
        explicit MouseDragEndedEvent(const MouseButton button,
                                     const UInt16 x,
                                     const UInt16 y,
                                     const Float32 dragTime)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse),
                    EventType::MouseDragEnded)
        {
            Capture(button, 5);
            Capture(x, 8);
            Capture(y, 10);
            Capture(dragTime, 12);
        }

        [[nodiscard]] OTR_INLINE MouseButton GetMouseButton() const { return Get<MouseButton>(5); }
        [[nodiscard]] OTR_INLINE UInt16 GetX() const { return Get<UInt16>(8); }
        [[nodiscard]] OTR_INLINE UInt16 GetY() const { return Get<UInt16>(10); }
        [[nodiscard]] OTR_INLINE Float32 GetDragTime() const { return Get<Float32>(12); }
    };
}

#endif //OTTERENGINE_MOUSEEVENTS_H
