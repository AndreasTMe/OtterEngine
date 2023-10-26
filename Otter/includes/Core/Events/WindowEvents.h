#ifndef OTTERENGINE_WINDOWEVENTS_H
#define OTTERENGINE_WINDOWEVENTS_H

#include "Core/Events/Event.h"

namespace Otter::Internal
{
#define OTR_WINDOW_EVENT_CLASS(Type)                                                        \
    class Window##Type##Event final : public Event                                          \
    {                                                                                       \
    public:                                                                                 \
        Window##Type##Event() : Event(EventType::Window##Type, EventCategory::Window) { }   \
        ~Window##Type##Event() final = default;                                             \
    };

    OTR_WINDOW_EVENT_CLASS(Close)

    class WindowResizeEvent final : public Event
    {
    public:
        WindowResizeEvent(UInt16 width, UInt16 height, bool isInitiatedByUser)
            : Event(EventType::WindowResize, EventCategory::Window),
              m_Width(width), m_Height(height), m_IsInitiatedByUser(isInitiatedByUser)
        {
        }
        ~WindowResizeEvent() final = default;

        [[nodiscard]] OTR_INLINE UInt16 GetWidth() const { return m_Width; }
        [[nodiscard]] OTR_INLINE UInt16 GetHeight() const { return m_Height; }
        [[nodiscard]] OTR_INLINE bool IsInitiatedByUser() const { return m_IsInitiatedByUser; }

    private:
        UInt16 m_Width;
        UInt16 m_Height;
        bool   m_IsInitiatedByUser;
    };

    OTR_WINDOW_EVENT_CLASS(Minimized)

    OTR_WINDOW_EVENT_CLASS(Maximized)

    OTR_WINDOW_EVENT_CLASS(Restored)

    OTR_WINDOW_EVENT_CLASS(Refresh)

#undef OTR_WINDOW_EVENT_CLASS
}

OTR_WITH_TYPENAME(Otter::Internal::WindowCloseEvent)

OTR_WITH_TYPENAME(Otter::Internal::WindowResizeEvent)

OTR_WITH_TYPENAME(Otter::Internal::WindowMinimizedEvent)

OTR_WITH_TYPENAME(Otter::Internal::WindowMaximizedEvent)

OTR_WITH_TYPENAME(Otter::Internal::WindowRestoredEvent)

OTR_WITH_TYPENAME(Otter::Internal::WindowRefreshEvent)

#endif //OTTERENGINE_WINDOWEVENTS_H
