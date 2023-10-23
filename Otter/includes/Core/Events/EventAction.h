#ifndef OTTERENGINE_EVENTACTION_H
#define OTTERENGINE_EVENTACTION_H

#include <functional>

#include "Core/Defines.h"
#include "Core/Events/Event.h"

namespace Otter::Internal
{
    class EventAction final
    {
    public:
        using Function = std::function<bool(const Event&)>;

    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR(EventAction)

        OTR_INLINE EventAction(const EventAction& other) noexcept { m_Functions = other.m_Functions; }
        OTR_INLINE EventAction(EventAction&& other) noexcept { m_Functions = std::move(other.m_Functions); }
        OTR_INLINE explicit EventAction(EventAction* other)
            : EventAction(*other)
        {
        }

        OTR_INLINE EventAction& operator=(const EventAction& other)
        {
            m_Functions = other.m_Functions;
            return *this;
        }

        OTR_INLINE EventAction& operator=(EventAction&& other) noexcept
        {
            m_Functions = std::move(other.m_Functions);
            return *this;
        }

        OTR_INLINE void operator+=(const Function& function)
        {
            if (!function)
                return;

            m_Functions.push_back(function);
        }

//        OTR_INLINE void operator-=(Function& function)
//        {
//            if (!function)
//                return;
//
//            /*
//             * BUG: no match for 'operator=='
//             * (operand types are 'std::function<bool(const Otter::Internal::Event&)* >' and
//             * 'const std::function<bool(const Otter::Internal::Event&)>')
//             */
//            m_Functions.erase(std::remove(m_Functions.begin(), m_Functions.end(), function), m_Functions.end());
//        }

        OTR_INLINE bool operator==(const EventAction& other) const
        {
            if (m_Functions.size() != other.m_Functions.size())
                return false;

            for (auto i = 0; i < m_Functions.size(); i++)
                if (m_Functions[i].target_type() != other.m_Functions[i].target_type())
                    return false;

            return true;
        }

        OTR_INLINE bool operator!=(const EventAction& other) const { return !(*this == other); }
        OTR_INLINE virtual bool operator()(const Event& event) { return Invoke(event); }

        OTR_INLINE bool Invoke(const Event& event)
        {
            if (m_Functions.empty())
                return false;

            for (auto it = m_Functions.begin(); it != m_Functions.end(); it++)
                if ((*it)(event))
                    return true;

            return false;
        }

        OTR_INLINE bool ReverseInvoke(const Event& event)
        {
            if (m_Functions.empty())
                return false;

            for (auto itReverse = m_Functions.rbegin(); itReverse != m_Functions.rend(); itReverse++)
                if ((*itReverse)(event))
                    return true;

            return false;
        }

        OTR_INLINE void Clear() { m_Functions.clear(); }

    private:
        // TODO: Replace with a custom list.
        std::vector<Function> m_Functions;
    };
}

#endif //OTTERENGINE_EVENTACTION_H
