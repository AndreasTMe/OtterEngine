#ifndef OTTERENGINE_TIME_H
#define OTTERENGINE_TIME_H

#include "Core/Defines.h"
#include "Core/BaseTypes.h"
#include "Core/Assert.h"
#include "Core/Function.h"

namespace Otter
{
    /**
     * @brief Configuration structure, used to pass time configuration to the Time class.
     */
    struct TimeConfiguration final
    {
    public:
        Double64 FrameRateMin   = 0.0;
        Double64 FrameRateMax   = 0.0;
        Double64 FixedDeltaTime = 0.0;
    };

    /// @brief Time step alias, used to represent time in seconds.
    using TimeStep = Double64;

    /**
     * @brief Time class, used to manage time in the engine.
     */
    struct Time final
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param config Time configuration (frame rate limits, fixed delta time).
         * @param getTimeCallback Callback used to get the current time.
         */
        Time(const TimeConfiguration& config, const Function<Double64()>& getTimeCallback)
            : k_InverseFrameRateMin(config.FrameRateMin == 0.0 ? 0.0 : 1.0 / config.FrameRateMin),
              k_InverseFrameRateMax(config.FrameRateMax == 0.0 ? 0.0 : 1.0 / config.FrameRateMax),
              k_FixedDeltaTime(config.FixedDeltaTime)
        {
            OTR_INTERNAL_ASSERT_MSG(config.FrameRateMin > 0.0 && config.FrameRateMax > 0.0
                                    ? config.FrameRateMin <= config.FrameRateMax
                                    : true,
                                    "Minimum frame rate cannot be greater than maximum frame rate.")
            OTR_INTERNAL_ASSERT_MSG(config.FixedDeltaTime >= 0.0, "Fixed delta time cannot be negative.")

            OTR_INTERNAL_ASSERT_MSG(!getTimeCallback.IsEmpty(), "Callback cannot be null.")

            m_GetTimeCallback = getTimeCallback;
        }

        /**
         * @brief Destructor.
         */
        ~Time() = default;

        /**
         * @brief Starts the time by setting the last frame time.
         */
        void Start();

        /**
         * @brief Refreshes the time by calculating the delta time.
         */
        void Refresh();

        /**
         * @brief Checks if there are fixed steps left in the current frame.
         *
         * @return True if the time has fixed steps left, false otherwise.
         */
        [[nodiscard]] bool HasFixedStepsLeft();

        /**
         * @brief Getter for the delta time.
         *
         * @return The delta time.
         */
        [[nodiscard]] OTR_INLINE TimeStep GetDeltaTime() const { return m_DeltaTime; }

        /**
         * @brief Getter for the fixed delta time.
         *
         * @return The fixed delta time.
         */
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
