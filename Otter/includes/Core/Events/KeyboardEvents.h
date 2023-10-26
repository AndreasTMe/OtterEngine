#ifndef OTTERENGINE_KEYBOARDEVENTS_H
#define OTTERENGINE_KEYBOARDEVENTS_H

#include "Core/Types.h"
#include "Core/Events/Event.h"

namespace Otter::Internal
{
    class KeyPressedEvent final : public Event
    {
    public:
        explicit KeyPressedEvent(const KeyCode keyCode)
            : Event(EventType::KeyPressed,
                    static_cast<EventCategory>(EventCategory::Input | EventCategory::Keyboard)),
              m_KeyCode(keyCode)
        {
        }
        ~KeyPressedEvent() final = default;

        [[nodiscard]] OTR_INLINE KeyCode GetKeyCode() const { return m_KeyCode; }

    private:
        KeyCode m_KeyCode = KeyCode::None;
    };

    class KeyReleasedEvent final : public Event
    {
    public:
        explicit KeyReleasedEvent(const KeyCode keyCode)
            : Event(EventType::KeyReleased,
                    static_cast<EventCategory>(EventCategory::Input | EventCategory::Keyboard)),
              m_KeyCode(keyCode)
        {
        }
        ~KeyReleasedEvent() final = default;

        [[nodiscard]] OTR_INLINE KeyCode GetKeyCode() const { return m_KeyCode; }

    private:
        KeyCode m_KeyCode = KeyCode::None;
    };

    class KeyHoldEvent final : public Event
    {
    public:
        explicit KeyHoldEvent(const KeyCode keyCode, const Float32 holdTime)
            : Event(EventType::KeyHold,
                    static_cast<EventCategory>(EventCategory::Input | EventCategory::Keyboard)),
              m_KeyCode(keyCode), m_HoldTime(holdTime)
        {
        }
        ~KeyHoldEvent() final = default;

        [[nodiscard]] OTR_INLINE KeyCode GetKeyCode() const { return m_KeyCode; }
        [[nodiscard]] OTR_INLINE Float32 GetHoldTime() const { return m_HoldTime; }

    private:
        KeyCode m_KeyCode  = KeyCode::None;
        Float32 m_HoldTime = 0.0f; // TODO: Is there a way to capture the time between KeyPressed and KeyReleased?
    };
}

OTR_WITH_TYPENAME(Otter::Internal::KeyPressedEvent)

OTR_WITH_TYPENAME(Otter::Internal::KeyReleasedEvent)

OTR_WITH_TYPENAME(Otter::Internal::KeyHoldEvent)

#endif //OTTERENGINE_KEYBOARDEVENTS_H
