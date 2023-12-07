#ifndef OTTERENGINE_TIME_H
#define OTTERENGINE_TIME_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Core/Function.h"

namespace Otter
{
    struct TimeConfiguration final
    {
    public:
        Double64 FrameRateMin   = 0.0;
        Double64 FrameRateMax   = 0.0;
        Double64 FixedDeltaTime = 0.0;
    };

    using TimeStep = Double64;

    struct Time final
    {
    public:
        explicit Time(const TimeConfiguration& config, Function<Double64()> getTimeCallback)
            : k_InverseFrameRateMin(config.FrameRateMin == 0.0 ? 0.0 : 1.0 / config.FrameRateMin),
              k_InverseFrameRateMax(config.FrameRateMax == 0.0 ? 0.0 : 1.0 / config.FrameRateMax),
              k_FixedDeltaTime(config.FixedDeltaTime)
        {
            OTR_INTERNAL_ASSERT_MSG(k_InverseFrameRateMax > 0.0
                                    ? k_InverseFrameRateMin <= k_InverseFrameRateMax
                                    : true,
                                    "Minimum frame rate cannot be greater than maximum frame rate.")
            OTR_INTERNAL_ASSERT_MSG(k_FixedDeltaTime >= 0.0, "Fixed delta time cannot be negative.")

            OTR_INTERNAL_ASSERT_MSG(getTimeCallback, "Callback cannot be null.")

            m_GetTimeCallback = getTimeCallback;
        }
        ~Time() = default;

        void Start();
        void Refresh();

        [[nodiscard]] bool HasFixedStepsLeft();

        [[nodiscard]] OTR_INLINE TimeStep GetDeltaTime() const { return m_DeltaTime; }
        [[nodiscard]] OTR_INLINE TimeStep GetFixedDeltaTime() const { return k_FixedDeltaTime; }

    private:
        const Double64 k_InverseFrameRateMin;
        const Double64 k_InverseFrameRateMax;
        const Double64 k_FixedDeltaTime;

        Function<Double64()> m_GetTimeCallback{ };

        Double64 m_DeltaTime     = 0.0;
        Double64 m_LastFrameTime = 0.0;
        Double64 m_Accumulator   = 0.0;
    };
}

#endif //OTTERENGINE_TIME_H
