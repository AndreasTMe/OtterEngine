#ifndef OTTERENGINE_MOUSEEVENTS_H
#define OTTERENGINE_MOUSEEVENTS_H

#include "Core/Types.h"
#include "Core/Events/Event.h"

namespace Otter::Internal
{
    class MouseButtonPressedEvent final : public Event
    {
    public:
        explicit MouseButtonPressedEvent(const MouseButton button, const UInt8 count = 1)
            : Event(EventType::MouseButtonPressed,
                    static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse)),
              m_Button(button), m_Count(count)
        {
        }
        ~MouseButtonPressedEvent() final = default;

        [[nodiscard]] OTR_INLINE MouseButton GetButton() const { return m_Button; }
        [[nodiscard]] OTR_INLINE UInt8 GetClickCount() const { return m_Count; }

    private:
        MouseButton m_Button = MouseButton::None;
        UInt8       m_Count  = 0;
    };

    class MouseButtonReleasedEvent final : public Event
    {
    public:
        explicit MouseButtonReleasedEvent(const MouseButton button)
            : Event(EventType::MouseButtonReleased,
                    static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse)),
              m_Button(button)
        {
        }
        ~MouseButtonReleasedEvent() final = default;

        [[nodiscard]] OTR_INLINE MouseButton GetButton() const { return m_Button; }

    private:
        MouseButton m_Button = MouseButton::None;
    };

    class MouseScrollEvent final : public Event
    {
    public:
        explicit MouseScrollEvent(const bool isPositive)
            : Event(EventType::MouseScroll,
                    static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse)),
              m_IsPositive(isPositive)
        {
        }
        ~MouseScrollEvent() final = default;

        [[nodiscard]] OTR_INLINE bool IsPositive() const { return m_IsPositive; }

    private:
        bool m_IsPositive = false;
    };

    class MouseMovedEvent final : public Event
    {
    public:
        explicit MouseMovedEvent(const UInt32 x, const UInt32 y)
            : Event(EventType::MouseMoved,
                    static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse)),
              m_X(x), m_Y(y)
        {
        }
        ~MouseMovedEvent() final = default;

        [[nodiscard]] OTR_INLINE UInt16 GetX() const { return m_X; }
        [[nodiscard]] OTR_INLINE UInt16 GetY() const { return m_Y; }

    private:
        UInt16 m_X = 0;
        UInt16 m_Y = 0;
    };

    class MouseDraggedEvent final : public Event
    {
    public:
        explicit MouseDraggedEvent(const MouseButton button,
                                   const UInt16 x,
                                   const UInt16 y,
                                   const Float32 dragTime = 0,
                                   const bool stopped = false)
            : Event(EventType::MouseDragEnded,
                    static_cast<EventCategory>(EventCategory::Input | EventCategory::Mouse)),
              m_Button(button), m_X(x), m_Y(y), m_DragTime(dragTime), m_Stopped(stopped)
        {
        }
        ~MouseDraggedEvent() final = default;

        // TODO: Implement DragStarted/Ended events.
//        [[nodiscard]] OTR_INLINE EventType GetEventType() const final
//        {
//            if (m_Stopped)
//                return EventType::MouseDragEnded;
//            else if (m_DragTime == 0.0)
//                return EventType::MouseDragStarted;
//            else
//                return EventType::MouseDragged;
//        }

        [[nodiscard]] OTR_INLINE MouseButton GetButton() const { return m_Button; }
        [[nodiscard]] OTR_INLINE UInt16 GetX() const { return m_X; }
        [[nodiscard]] OTR_INLINE UInt16 GetY() const { return m_Y; }
        [[nodiscard]] OTR_INLINE Float32 GetDragTime() const { return m_DragTime; }

    private:
        MouseButton m_Button = MouseButton::None;

        UInt16  m_X        = 0;
        UInt16  m_Y        = 0;
        Float32 m_DragTime = 0;
        bool    m_Stopped  = false;
    };
}

#endif //OTTERENGINE_MOUSEEVENTS_H
