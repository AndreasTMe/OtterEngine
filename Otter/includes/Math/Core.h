#ifndef OTTERENGINE_CORE_H
#define OTTERENGINE_CORE_H

#include <cmath>
#include <limits>

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Math/Math.Concepts.h"

// TODO: Add SIMD support.
namespace Otter::Math
{
    enum class AngleType
    {
        Radians,
        Degrees
    };

    enum class Axis
    {
        X,
        Y,
        Z
    };

    template<FloatingPointNumber TNumber>
    OTR_INLINE constexpr TNumber Pi = static_cast<TNumber>(3.141592653589793238462643383279502884L);

    template<FloatingPointNumber TNumber>
    OTR_INLINE constexpr TNumber Tau = static_cast<TNumber>(6.283185307179586476925286766559005768L);

    template<FloatingPointNumber TNumber>
    OTR_INLINE constexpr TNumber Epsilon = std::numeric_limits<TNumber>::epsilon();

    template<FloatingPointNumber TNumber>
    OTR_INLINE constexpr TNumber Sqrt2 = static_cast<TNumber>(1.414213562373095048801688724209698079L);

    template<FloatingPointNumber TNumber>
    OTR_INLINE constexpr TNumber Sqrt3 = static_cast<TNumber>(1.732050807568877293527446341505872366L);

    template<AnyNumber TNumber>
    OTR_INLINE constexpr TNumber PositiveInfinity = std::numeric_limits<TNumber>::infinity();

    template<AnyNumber TNumber>
    OTR_INLINE constexpr TNumber NegativeInfinity = -std::numeric_limits<TNumber>::infinity();

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE constexpr auto Max(Tx x, Ty y) { return x > y ? x : y; }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber... TRest>
    OTR_INLINE constexpr auto Max(Tx x, Ty y, TRest... rest) { return Max(Max(x, y), rest...); }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE constexpr auto Min(Tx x, Ty y) { return x < y ? x : y; }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber... TRest>
    OTR_INLINE constexpr auto Min(Tx x, Ty y, TRest... rest) { return Min(Min(x, y), rest...); }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Abs(TNumber x) { return x < 0 ? -x : x; }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr bool AreApproximatelyEqual(TNumber a, TNumber b) { return Abs(a - b) < Epsilon<Double128>; }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr bool IsApproximatelyZero(TNumber x)
    {
        if constexpr (IntegerNumber<TNumber>)
            return x == 0;

        return AreApproximatelyEqual(x, 0);
    }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr bool IsNaN(TNumber x) { return std::numeric_limits<TNumber>::has_quiet_NaN && std::isnan(x); }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr bool IsInfinity(TNumber x)
    {
        return std::numeric_limits<TNumber>::has_infinity && std::isinf(x);
    }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Sign(TNumber x) { return x < 0 ? -1 : 1; }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE constexpr auto FMod(Tx x, Ty y) { return std::fmod(x, y); }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE constexpr auto Power(Tx x, Ty y) { return (IsApproximatelyZero(y)) ? 1 : std::pow(x, y); }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Square(TNumber x) { return x * x; }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Cube(TNumber x) { return x * x * x; }

    template<IntegerNumber TNumber>
    OTR_INLINE constexpr bool IsPowerOfTwo(TNumber x) { return x > 0 && IsApproximatelyZero(x & (x - 1)); }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Exp(TNumber x) { return std::exp(x); }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto SquareRoot(TNumber x)
    {
        OTR_ASSERT_MSG(x >= 0, "Cannot take the square root of a negative number")
        return std::sqrt(x);
    }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Log2(TNumber x) { return std::log2(x); }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Log10(TNumber x) { return std::log10(x); }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Ln(TNumber x) { return std::log(x); }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto DegToRad(TNumber degrees)
    {
        return static_cast<TNumber>(degrees * (Pi<Double128> / 180));
    }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto RadToDeg(TNumber radians)
    {
        return static_cast<TNumber>(radians * (180 / Pi<Double128>));
    }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Sin(TNumber x) { return std::sin(x); }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Cos(TNumber x) { return std::cos(x); }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Tan(TNumber x) { return std::tan(x); }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Asin(TNumber x) { return std::asin(x); }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Acos(TNumber x) { return std::acos(x); }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Atan(TNumber x) { return std::atan(x); }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE constexpr auto Atan2(Tx y, Ty x) { return std::atan2(y, x); }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Round(TNumber x) { return std::round(x); }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Ceil(TNumber x) { return std::ceil(x); }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Floor(TNumber x) { return std::floor(x); }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE constexpr auto Clamp(Tx value, Ty min, Tz max)
    {
        return value < min ? min : value > max ? max : value;
    }

    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Clamp01(TNumber value) { return Clamp(value, (TNumber) 0.0, (TNumber) 1.0); }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE constexpr auto Lerp(Tx a, Ty b, Tz t) { return a + (b - a) * t; }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE constexpr auto LerpClamped(Tx a, Ty b, Tz t) { return Clamp(Lerp(a, b, t), a, b); }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE constexpr auto InverseLerp(Tx a, Ty b, Tz value)
    {
        if (AreApproximatelyEqual(a, b))
            return static_cast<decltype(a * b * value)>(0.0);

        return (value - a) / (b - a);
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE constexpr auto InverseLerpClamped(Tx a, Ty b, Tz value) { return Clamp01(InverseLerp(a, b, value)); }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE constexpr auto SmoothStep(Tx min, Ty max, Tz value)
    {
        auto t = Clamp01(InverseLerp(min, max, value));
        return t * t * (3 - 2 * t);
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE constexpr auto InverseSmoothStep(Tx min, Ty max, Tz smoothenedValue)
    {
        if (AreApproximatelyEqual(min, max))
            return min;

        smoothenedValue = Clamp((smoothenedValue - min) / (max - min), 0.0, 1.0);
        return min + smoothenedValue * smoothenedValue * (3 - 2 * smoothenedValue) * (max - min);
    }

    template<AnyNumber TNumber>
    OTR_INLINE auto NormaliseAngle(TNumber angle, AngleType angleType = AngleType::Radians)
    {
        const auto fullCircle = angleType == AngleType::Radians ? Tau<Double128> : 360.0;
        const auto halfCircle = angleType == AngleType::Radians ? Pi<Double128> : 180.0;

        while (angle > halfCircle)
            angle -= fullCircle;

        while (angle <= -halfCircle)
            angle += fullCircle;

        return angle;
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto LerpAngle(Tx angleA, Ty angleB, Tz t, AngleType angleType = AngleType::Radians)
    {
        const auto fullCircle = angleType == AngleType::Radians ? Tau<Double128> : 360.0;
        const auto halfCircle = angleType == AngleType::Radians ? Pi<Double128> : 180.0;

        auto angleAMod = FMod(angleA, fullCircle);
        auto angleBMod = FMod(angleB, fullCircle);

        auto angleDifference = angleBMod - angleAMod;

        if (angleDifference > halfCircle)
            angleDifference -= fullCircle;
        else if (angleDifference < -halfCircle)
            angleDifference += fullCircle;

        auto result = angleAMod + angleDifference * t;

        result = FMod(result, fullCircle);
        if (result < 0.0)
            result += fullCircle;

        return result;
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto InverseLerpAngle(Tx angleA, Ty angleB, Tz angleC, AngleType angleType = AngleType::Radians)
    {
        const auto fullCircle = angleType == AngleType::Radians ? Tau<Double128> : 360.0;
        const auto halfCircle = angleType == AngleType::Radians ? Pi<Double128> : 180.0;

        auto angleAMod = FMod(angleA, fullCircle);
        auto angleBMod = FMod(angleB, fullCircle);
        auto angleCMod = FMod(angleC, fullCircle);

        auto angleDifference = angleBMod - angleAMod;

        if (angleDifference > halfCircle)
            angleDifference -= fullCircle;
        else if (angleDifference < -halfCircle)
            angleDifference += fullCircle;

        if (IsApproximatelyZero(angleDifference))
            return static_cast<decltype(angleA * angleB * angleC)>(0.0);

        return (angleCMod - angleAMod) / angleDifference;
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto MoveTowards(Tx current, Ty target, Tz speed)
    {
        if (AreApproximatelyEqual(current, target))
            return target;

        const auto direction = (target - current > 0) ? 1.0 : -1.0;
        current += direction * speed;

        if ((direction > 0 && current > target) || (direction < 0 && current < target))
            return target;

        return current;
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto MoveTowardsAngle(Tx current, Ty target, Tz maxDeltaAngle, AngleType angleType = AngleType::Radians)
    {
        const auto fullCircle = angleType == AngleType::Radians ? Tau<Double128> : 360.0;
        const auto halfCircle = angleType == AngleType::Radians ? Pi<Double128> : 180.0;

        current = FMod(current, fullCircle);
        target  = FMod(target, fullCircle);

        const auto angleDifference = FMod(target - current + fullCircle, fullCircle) - halfCircle;
        const auto clampedAngle    = Clamp(angleDifference, -maxDeltaAngle, maxDeltaAngle);

        return FMod(current + clampedAngle + fullCircle, fullCircle);
    }
}

#endif //OTTERENGINE_CORE_H
