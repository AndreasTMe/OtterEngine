#ifndef OTTERENGINE_KEYBOARDEVENTS_H
#define OTTERENGINE_KEYBOARDEVENTS_H

#include "Core/Types.h"
#include "Core/Events/Event.h"

namespace Otter
{
    /**
     * @brief Event that is fired when a keyboard key is pressed.
     */
    class KeyPressedEvent final : public Event
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param keyCode The key code of the key that was pressed.
         */
        explicit KeyPressedEvent(const KeyCode keyCode)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Keyboard),
                    EventType::KeyPressed)
        {
            Capture(keyCode, 5);
        }

        /**
         * @brief Gets the key code of the key that was pressed.
         *
         * @return The key code of the key that was pressed.
         */
        [[nodiscard]] OTR_INLINE KeyCode GetKeyCode() const { return Get<KeyCode>(5); }
    };

    /**
     * @brief Event that is fired when a keyboard key is released.
     */
    class KeyReleasedEvent final : public Event
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param keyCode The key code of the key that was released.
         */
        explicit KeyReleasedEvent(const KeyCode keyCode)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Keyboard),
                    EventType::KeyReleased)
        {
            Capture(keyCode, 5);
        }

        /**
         * @brief Gets the key code of the key that was released.
         *
         * @return The key code of the key that was released.
         */
        [[nodiscard]] OTR_INLINE KeyCode GetKeyCode() const { return Get<KeyCode>(5); }
    };

    /**
     * @brief Event that is fired when a keyboard key is repeated.
     */
    class KeyRepeatEvent final : public Event
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param keyCode The key code of the key that was repeated.
         * @param repeatCount The number of times the key has been repeated.
         */
        explicit KeyRepeatEvent(const KeyCode keyCode, const UInt16 repeatCount)
            : Event(static_cast<EventCategory>(EventCategory::Input | EventCategory::Keyboard),
                    EventType::KeyRepeat)
        {
            Capture(keyCode, 5);
            Capture(repeatCount, 12);
        }

        /**
         * @brief Gets the key code of the key that was repeated.
         *
         * @return The key code of the key that was repeated.
         */
        [[nodiscard]] OTR_INLINE KeyCode GetKeyCode() const { return Get<KeyCode>(5); }

        /**
         * @brief Gets the number of times the key has been repeated.
         *
         * @return The number of times the key has been repeated.
         */
        [[nodiscard]] OTR_INLINE UInt16 GetRepeatCount() const { return Get<UInt16>(12); }
    };
}

#endif //OTTERENGINE_KEYBOARDEVENTS_H
