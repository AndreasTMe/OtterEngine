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

    class KeyRepeatEvent final : public Event
    {
    public:
        explicit KeyRepeatEvent(const KeyCode keyCode, const UInt16 repeatCount)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Keyboard),
                    EventType::KeyHold)
        {
            Capture(keyCode, 5);
            Capture(repeatCount, 12);
        }

        [[nodiscard]] OTR_INLINE KeyCode GetKeyCode() const { return Get < KeyCode > (5); }
        [[nodiscard]] OTR_INLINE UInt16 GetRepeatCount() const { return Get < UInt16 > (12); }
    };
}

#endif //OTTERENGINE_KEYBOARDEVENTS_H
