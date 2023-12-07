#include "Otter.PCH.h"

#include "Core/Time.h"

namespace Otter
{
    void Time::Start()
    {
        OTR_INTERNAL_ASSERT_MSG(!m_GetTimeCallback.IsEmpty(), "Callback has not been set.")

        m_LastFrameTime = m_GetTimeCallback();
    }

    void Time::Refresh()
    {
        const auto currentFrameTime = m_GetTimeCallback();
        m_DeltaTime     = currentFrameTime - m_LastFrameTime;
        m_LastFrameTime = currentFrameTime;

        m_Accumulator += m_DeltaTime;

        if (k_InverseFrameRateMin > 0.0 && m_Accumulator < k_InverseFrameRateMin)
            m_Accumulator = k_InverseFrameRateMin;
        else if (k_InverseFrameRateMax > 0.0 && m_Accumulator > k_InverseFrameRateMax)
            m_Accumulator = k_InverseFrameRateMax;
    }

    bool Time::HasFixedStepsLeft()
    {
        if (k_FixedDeltaTime <= 0.0 || m_Accumulator <= 0.0)
            return false;

        if (m_Accumulator >= k_FixedDeltaTime)
        {
            m_Accumulator -= k_FixedDeltaTime;

            return true;
        }

        return false;
    }
}