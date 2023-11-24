#ifndef OTTERENGINE_KEYBOARDEVENTS_H
#define OTTERENGINE_KEYBOARDEVENTS_H

#include "Core/Types.h"
#include "Core/Events/Event.h"

namespace Otter
{
    class KeyPressedEvent final : public Event
    {
    public:
        explicit KeyPressedEvent(const KeyCode keyCode)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Keyboard),
                    EventType::KeyPressed)
        {
            Capture(keyCode, 5);
        }

        [[nodiscard]] OTR_INLINE KeyCode GetKeyCode() const { return Get<KeyCode>(5); }
    };

    class KeyReleasedEvent final : public Event
    {
    public:
        explicit KeyReleasedEvent(const KeyCode keyCode)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Keyboard),
                    EventType::KeyReleased)
        {
            Capture(keyCode, 5);
        }

        [[nodiscard]] OTR_INLINE KeyCode GetKeyCode() const { return Get<KeyCode>(5); }
    };

    class KeyHoldEvent final : public Event
    {
    public:
        explicit KeyHoldEvent(const KeyCode keyCode, const Float32 holdTime)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Keyboard),
                    EventType::KeyHold)
        {
            Capture(keyCode, 5);
            Capture(holdTime, 3);
        }

        [[nodiscard]] OTR_INLINE KeyCode GetKeyCode() const { return Get<KeyCode>(5); }
        [[nodiscard]] OTR_INLINE Float32 GetHoldTime() const { return Get<Float32>(3); }
    };
}

#endif //OTTERENGINE_KEYBOARDEVENTS_H
