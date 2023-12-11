#ifndef OTTERENGINE_WINDOWEVENTS_H
#define OTTERENGINE_WINDOWEVENTS_H

#include "Core/Events/Event.h"

namespace Otter
{
    /**
     * @brief Event that is fired when the window is closed.
     */
    class WindowCloseEvent final : public Event
    {
    public:
        /**
         * @brief Constructor.
         */
        WindowCloseEvent()
            : Event(static_cast<EventCategory>(EventCategory::Window | EventCategory::Blocking),
                    EventType::WindowClose)
        {
        }
    };

    /**
     * @brief Event that is fired when the window is resized.
     */
    class WindowResizeEvent final : public Event
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param width The new width of the window.
         * @param height The new height of the window.
         * @param isInitiatedByUser Whether the resize was initiated by the user.
         */
        WindowResizeEvent(const UInt16 width, const UInt16 height, const bool isInitiatedByUser)
            : Event(static_cast<EventCategory>(EventCategory::Window | EventCategory::Blocking),
                    EventType::WindowResize)
        {
            Capture(width, 8);
            Capture(height, 10);
            Capture(isInitiatedByUser, 5);
        }

        /**
         * @brief Gets the new width of the window.
         *
         * @return The new width of the window.
         */
        [[nodiscard]] OTR_INLINE UInt16 GetWidth() const { return Get<UInt16>(8); }

        /**
         * @brief Gets the new height of the window.
         *
         * @return The new height of the window.
         */
        [[nodiscard]] OTR_INLINE UInt16 GetHeight() const { return Get<UInt16>(10); }

        /**
         * @brief Gets whether the resize was initiated by the user.
         *
         * @return True if the resize was initiated by the user, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsInitiatedByUser() const { return Get<bool>(5); }
    };

    /**
     * @brief Event that is fired when the window is minimized.
     */
    class WindowMinimizedEvent final : public Event
    {
    public:
        /**
         * @brief Constructor.
         */
        WindowMinimizedEvent()
            : Event(static_cast<EventCategory>(EventCategory::Window | EventCategory::Blocking),
                    EventType::WindowMinimized)
        {
        }
    };

    /**
     * @brief Event that is fired when the window is maximized.
     */
    class WindowMaximizedEvent final : public Event
    {
    public:
        /**
         * @brief Constructor.
         */
        WindowMaximizedEvent()
            : Event(static_cast<EventCategory>(EventCategory::Window | EventCategory::Blocking),
                    EventType::WindowMaximized)
        {
        }
    };

    /**
     * @brief Event that is fired when the window is restored.
     */
    class WindowRestoredEvent final : public Event
    {
    public:
        /**
         * @brief Constructor.
         */
        WindowRestoredEvent()
            : Event(static_cast<EventCategory>(EventCategory::Window | EventCategory::Blocking),
                    EventType::WindowRestored)
        {
        }
    };
}

#endif //OTTERENGINE_WINDOWEVENTS_H
