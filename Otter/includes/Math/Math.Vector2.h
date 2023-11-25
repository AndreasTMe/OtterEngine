#ifndef OTTERENGINE_MATH_VECTOR2_H
#define OTTERENGINE_MATH_VECTOR2_H

#include "Math/Vector.h"

namespace Otter::Math
{
    template<AnyNumber TNumber>
    OTR_INLINE bool IsApproximatelyZero(const Vector<2, TNumber>& vector)
    {
        if constexpr (IntegerNumber<TNumber>)
            return vector[0] == 0 && vector[1] == 0;

        return Math::IsApproximatelyZero(vector[0]) && Math::IsApproximatelyZero(vector[1]);
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE bool AreApproximatelyEqual(const Vector<2, Tx>& lhs, const Vector<2, Ty>& rhs)
    {
        if constexpr (IntegerNumber<Tx> && IntegerNumber<Ty>)
            return lhs[0] == rhs[0] && lhs[1] == rhs[1];

        return Math::AreApproximatelyEqual(lhs[0], rhs[0]) && Math::AreApproximatelyEqual(lhs[1], rhs[1]);
    }

    template<AnyNumber TNumber>
    OTR_INLINE auto MagnitudeSquared(const Vector<2, TNumber>& vector)
    {
        return Square(vector[0]) + Square(vector[1]);
    }

    template<AnyNumber TNumber>
    OTR_INLINE auto Magnitude(const Vector<2, TNumber>& vector) { return SquareRoot(MagnitudeSquared(vector)); }

    template<AnyNumber TNumber, AnyNumber TMaxMagnitude>
    OTR_INLINE auto ClampMagnitude(const Vector<2, TNumber>& vector, const TMaxMagnitude& maxMagnitude)
    {
        const auto magnitudeSquared = MagnitudeSquared(vector);
        if (magnitudeSquared > Square(maxMagnitude))
            return vector * Min(maxMagnitude / SquareRoot(magnitudeSquared), 1);

        return vector;
    }

    template<AnyNumber TNumber>
    OTR_INLINE auto Normalise(const Vector<2, TNumber>& vector)
    {
        const auto magnitude = Magnitude(vector);
        if (magnitude < Epsilon<Double128>)
            return Vector<2, TNumber>::Zero();

        return vector / magnitude;
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Dot(const Vector<2, Tx>& lhs, const Vector<2, Ty>& rhs)
    {
        return lhs[0] * rhs[0] + lhs[1] * rhs[1];
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto DistanceSquared(const Vector<2, Tx>& lhs, const Vector<2, Ty>& rhs)
    {
        return MagnitudeSquared(lhs - rhs);
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Distance(const Vector<2, Tx>& lhs, const Vector<2, Ty>& rhs)
    {
        return Magnitude(lhs - rhs);
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto Lerp(const Vector<2, Tx>& lhs, const Vector<2, Ty>& rhs, Tz t)
    {
        return lhs + (rhs - lhs) * t;
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto LerpClamped(const Vector<2, Tx>& lhs, const Vector<2, Ty>& rhs, Tz t)
    {
        return Lerp(lhs, rhs, Clamp(t, (Tz) 0.0, (Tz) 1.0));
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Max(const Vector<2, Tx>& lhs, const Vector<2, Ty>& rhs)
    {
        return Vector<2, decltype(Math::Max(lhs[0], rhs[0]))>(Math::Max(lhs[0], rhs[0]),
                                                              Math::Max(lhs[1], rhs[1]));
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Min(const Vector<2, Tx>& lhs, const Vector<2, Ty>& rhs)
    {
        return Vector<2, decltype(Math::Min(lhs[0], rhs[0]))>(Math::Min(lhs[0], rhs[0]),
                                                              Math::Min(lhs[1], rhs[1]));
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto Clamp(const Vector<2, Tx>& value, const Vector<2, Ty>& min, const Vector<2, Tz>& max)
    {
        return Vector<2, decltype(Math::Clamp(value[0], min[0], max[0]))>(Math::Clamp(value[0], min[0], max[0]),
                                                                          Math::Clamp(value[1], min[1], max[1]));
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Reflect(const Vector<2, Tx>& vector, const Vector<2, Ty>& normal)
    {
        return vector - 2 * Dot(vector, normal) * normal;
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Angle(const Vector<2, Tx>& lhs, const Vector<2, Ty>& rhs)
    {
        const auto magnitudeProduct = Magnitude(lhs) * Magnitude(rhs);
        if (magnitudeProduct < Epsilon<Double128>)
            return 0;

        return Acos(Dot(lhs, rhs) / magnitudeProduct);
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto AngleSigned(const Vector<2, Tx>& from, const Vector<2, Ty>& to)
    {
        return Sign(from[0] * to[1] - from[1] * to[0]) * Angle(from, to);
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto SmoothStep(const Vector<2, Tx>& min, const Vector<2, Ty>& max, const Vector<2, Tz>& value)
    {
        return Vector<2, decltype(Math::SmoothStep(min[0], max[0], value[0]))>{
            Math::SmoothStep(min[0], max[0], value[0]),
            Math::SmoothStep(min[1], max[1], value[1])
        };
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto InverseSmoothStep(const Vector<2, Tx>& min,
                                      const Vector<2, Ty>& max,
                                      const Vector<2, Tz>& smoothenedValue)
    {
        return Vector<2, decltype(Math::InverseSmoothStep(min, max, smoothenedValue[0]))>{
            Math::InverseSmoothStep(min[0], max[0], smoothenedValue[0]),
            Math::InverseSmoothStep(min[1], max[1], smoothenedValue[1])
        };
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto MoveTowards(const Vector<2, Tx>& current, const Vector<2, Ty>& target, Tz maxDistanceDelta)
    {
        if (AreApproximatelyEqual(current, target))
            return target;

        const auto difference       = { target[0] - current[0],
                                        target[1] - current[1] };
        const auto magnitudeSquared = MagnitudeSquared(difference);

        if (IsApproximatelyZero(magnitudeSquared))
            return target;

        const auto distance = SquareRoot(magnitudeSquared);
        if (distance <= maxDistanceDelta)
            return target;

        const auto factor = maxDistanceDelta / distance;

        return Vector<2, decltype(current[0] + difference[0] * factor)>{
            current[0] + difference[0] * factor,
            current[1] + difference[1] * factor
        };
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto RotateTowards(const Vector<2, Tx>& current,
                                  const Vector<2, Ty>& target,
                                  Tz maxDeltaAngle,
                                  AngleType angleType = AngleType::Radians)
    {
        if (AreApproximatelyEqual(current, target))
            return target;

        if (angleType == AngleType::Degrees)
            maxDeltaAngle = DegToRad(maxDeltaAngle);

        const auto currentAngle = Atan2(current.y, current.x);
        const auto targetAngle  = Atan2(target.y, target.x);
        const auto deltaAngle   = Clamp(NormalizeAngle(targetAngle - currentAngle),
                                        -maxDeltaAngle,
                                        maxDeltaAngle);

        const auto rotatedAngle = currentAngle + deltaAngle;
        const auto length       = Magnitude(current);

        return Vector<2, decltype(length * Cos(rotatedAngle))>{
            length * Cos(rotatedAngle),
            length * Sin(rotatedAngle)
        };
    }
}

#endif //OTTERENGINE_MATH_VECTOR2_H
