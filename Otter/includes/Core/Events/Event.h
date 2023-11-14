#ifndef OTTERENGINE_EVENT_H
#define OTTERENGINE_EVENT_H

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Otter::Internal
{
    class Event
    {
    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR_AND_VIRTUAL_DESTRUCTOR(Event)

        [[nodiscard]] OTR_INLINE constexpr EventType GetEventType() const { return m_EventType; }
        [[nodiscard]] OTR_INLINE constexpr EventCategory GetEventCategory() const { return m_EventCategory; }
        [[nodiscard]] OTR_INLINE constexpr bool IsBlocking() const
        {
            return (m_EventCategory & EventCategory::Blocking) == EventCategory::Blocking;
        }

    protected:
        Event(const EventType& type, const EventCategory& category)
            : m_EventType(type), m_EventCategory(category)
        {
        }

        EventType     m_EventType     = EventType::None;
        EventCategory m_EventCategory = EventCategory::None;
    };
}

#endif //OTTERENGINE_EVENT_H
