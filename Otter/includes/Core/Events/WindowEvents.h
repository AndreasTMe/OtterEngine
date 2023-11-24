#ifndef OTTERENGINE_WINDOWEVENTS_H
#define OTTERENGINE_WINDOWEVENTS_H

#include "Core/Events/Event.h"

namespace Otter
{
#define OTR_WINDOW_EVENT_CLASS(Type)                                                            \
    class Window##Type##Event final : public Event                                              \
    {                                                                                           \
    public:                                                                                     \
        Window##Type##Event()                                                                   \
            : Event(static_cast<EventCategory>(EventCategory::Window | EventCategory::Blocking),\
                    EventType::Window##Type)                                                    \
            {                                                                                   \
            }                                                                                   \
    };

    OTR_WINDOW_EVENT_CLASS(Close)

    class WindowResizeEvent final : public Event
    {
    public:
        WindowResizeEvent(const UInt16 width, const UInt16 height, const bool isInitiatedByUser)
            : Event(static_cast<EventCategory>(EventCategory::Window | EventCategory::Blocking),
                    EventType::WindowResize)
        {
            Capture(width, 8);
            Capture(height, 10);
            Capture(isInitiatedByUser, 5);
        }

        [[nodiscard]] OTR_INLINE UInt16 GetWidth() const { return Get<UInt16>(8); }
        [[nodiscard]] OTR_INLINE UInt16 GetHeight() const { return Get<UInt16>(10); }
        [[nodiscard]] OTR_INLINE bool IsInitiatedByUser() const { return Get<bool>(5); }
    };

    OTR_WINDOW_EVENT_CLASS(Minimized)

    OTR_WINDOW_EVENT_CLASS(Maximized)

    OTR_WINDOW_EVENT_CLASS(Restored)

    OTR_WINDOW_EVENT_CLASS(Refresh)

#undef OTR_WINDOW_EVENT_CLASS
}

#endif //OTTERENGINE_WINDOWEVENTS_H
