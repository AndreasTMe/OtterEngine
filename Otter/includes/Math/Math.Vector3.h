#ifndef OTTERENGINE_MATH_VECTOR3_H
#define OTTERENGINE_MATH_VECTOR3_H

#include "Math/Vector.h"

namespace Otter::Math
{
    template<AnyNumber TNumber>
    OTR_INLINE bool IsApproximatelyZero(const Vector<3, TNumber>& vector)
    {
        if constexpr (IntegerNumber<TNumber>)
            return vector[0] == 0 && vector[1] == 0 && vector[2] == 0;

        return Math::IsApproximatelyZero(vector[0])
               && Math::IsApproximatelyZero(vector[1])
               && Math::IsApproximatelyZero(vector[2]);
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE bool AreApproximatelyEqual(const Vector<3, Tx>& lhs, const Vector<3, Ty>& rhs)
    {
        if constexpr (IntegerNumber<Tx> && IntegerNumber<Ty>)
            return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2];

        return Math::AreApproximatelyEqual(lhs[0], rhs[0])
               && Math::AreApproximatelyEqual(lhs[1], rhs[1])
               && Math::AreApproximatelyEqual(lhs[2], rhs[2]);
    }

    template<AnyNumber TNumber>
    OTR_INLINE auto MagnitudeSquared(const Vector<3, TNumber>& vector)
    {
        return Square(vector[0]) + Square(vector[1]) + Square(vector[2]);
    }

    template<AnyNumber TNumber>
    OTR_INLINE auto Magnitude(const Vector<3, TNumber>& vector) { return SquareRoot(MagnitudeSquared(vector)); }

    template<AnyNumber TNumber, AnyNumber TMaxMagnitude>
    OTR_INLINE auto ClampMagnitude(const Vector<3, TNumber>& vector, const TMaxMagnitude& maxMagnitude)
    {
        const auto magnitudeSquared = MagnitudeSquared(vector);
        if (magnitudeSquared > Square(maxMagnitude))
            return vector * Min(maxMagnitude / SquareRoot(magnitudeSquared), 1);

        return vector;
    }

    template<AnyNumber TNumber>
    OTR_INLINE auto Normalise(const Vector<3, TNumber>& vector)
    {
        const auto magnitude = Magnitude(vector);
        if (magnitude < Epsilon<Double128>)
            return Vector<3, TNumber>::Zero();

        return vector / magnitude;
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Dot(const Vector<3, Tx>& lhs, const Vector<3, Ty>& rhs)
    {
        return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Cross(const Vector<3, Tx>& lhs, const Vector<3, Ty>& rhs)
    {
        return Vector<3, decltype(lhs[0] * rhs[0] - lhs[0] * rhs[0] + lhs[0] * rhs[0])>{
            lhs[1] * rhs[2] - lhs[2] * rhs[1],
            lhs[2] * rhs[0] - lhs[0] * rhs[2],
            lhs[0] * rhs[1] - lhs[1] * rhs[0]
        };
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto DistanceSquared(const Vector<3, Tx>& lhs, const Vector<3, Ty>& rhs)
    {
        return MagnitudeSquared(lhs - rhs);
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Distance(const Vector<3, Tx>& lhs, const Vector<3, Ty>& rhs)
    {
        return Magnitude(lhs - rhs);
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto Lerp(const Vector<3, Tx>& lhs, const Vector<3, Ty>& rhs, Tz t)
    {
        return lhs + (rhs - lhs) * t;
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto LerpClamped(const Vector<3, Tx>& lhs, const Vector<3, Ty>& rhs, Tz t)
    {
        return Lerp(lhs, rhs, Clamp(t, (Tz) 0.0, (Tz) 1.0));
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto Slerp(const Vector<3, Tx>& lhs, const Vector<3, Ty>& rhs, Tz t)
    {
        auto dotClamped = Clamp(Dot(lhs, rhs), -1.0, 1.0);
        auto theta      = Acos(dotClamped) * t;
        auto relative   = Normalise(rhs - lhs * dotClamped);

        return ((lhs * Cos(theta)) + (relative * Sin(theta)));
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto SlerpClamped(const Vector<3, Tx>& lhs, const Vector<3, Ty>& rhs, Tz t)
    {
        return Slerp(lhs, rhs, Clamp(t, (Tz) 0.0, (Tz) 1.0));
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Max(const Vector<3, Tx>& lhs, const Vector<3, Ty>& rhs)
    {
        return Vector<3, decltype(Math::Max(lhs[0], rhs[0]))>(Math::Max(lhs[0], rhs[0]),
                                                              Math::Max(lhs[1], rhs[1]),
                                                              Math::Max(lhs[2], rhs[2]));
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Min(const Vector<3, Tx>& lhs, const Vector<3, Ty>& rhs)
    {
        return Vector<3, decltype(Math::Min(lhs[0], rhs[0]))>(Math::Min(lhs[0], rhs[0]),
                                                              Math::Min(lhs[1], rhs[1]),
                                                              Math::Min(lhs[2], rhs[2]));
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto Clamp(const Vector<3, Tx>& value, const Vector<3, Ty>& min, const Vector<3, Tz>& max)
    {
        return Vector<3, decltype(Math::Clamp(value[0], min[0], max[0]))>(Math::Clamp(value[0], min[0], max[0]),
                                                                          Math::Clamp(value[1], min[1], max[1]),
                                                                          Math::Clamp(value[2], min[2], max[2]));
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Reflect(const Vector<3, Tx>& vector, const Vector<3, Ty>& normal)
    {
        return vector - 2 * Dot(vector, normal) * normal;
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Project(const Vector<3, Tx>& vector, const Vector<3, Ty>& normal)
    {
        const auto magnitudeSquared = MagnitudeSquared(normal);
        if (magnitudeSquared < Epsilon<Double128>)
            return Vector<3, decltype(vector[0] * normal[0])>::Zero();

        return (Dot(vector, normal) / magnitudeSquared) * normal;
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Angle(const Vector<3, Tx>& lhs, const Vector<3, Ty>& rhs)
    {
        const auto magnitudeProduct = Magnitude(lhs) * Magnitude(rhs);
        if (magnitudeProduct < Epsilon<Double128>)
            return 0;

        return Acos(Dot(lhs, rhs) / magnitudeProduct);
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto AngleSigned(const Vector<3, Tx>& from, const Vector<3, Ty>& to, const Vector<3, Ty>& axis)
    {
        return Sign(Dot(axis, Cross(from, to))) * Angle(from, to);
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto SmoothStep(const Vector<3, Tx>& min, const Vector<3, Ty>& max, const Vector<3, Tz>& value)
    {
        return Vector<3, decltype(Math::SmoothStep(min[0], max[0], value[0]))>{
            Math::SmoothStep(min[0], max[0], value[0]),
            Math::SmoothStep(min[1], max[1], value[1]),
            Math::SmoothStep(min[2], max[2], value[2])
        };
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE constexpr auto InverseSmoothStep(const Vector<3, Tx>& min,
                                                const Vector<3, Ty>& max,
                                                const Vector<3, Tz>& smoothenedValue)
    {
        return Vector<3, decltype(Math::InverseSmoothStep(min, max, smoothenedValue[0]))>{
            Math::InverseSmoothStep(min[0], max[0], smoothenedValue[0]),
            Math::InverseSmoothStep(min[1], max[1], smoothenedValue[1]),
            Math::InverseSmoothStep(min[2], max[2], smoothenedValue[2])
        };
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto MoveTowards(const Vector<3, Tx>& current, const Vector<3, Ty>& target, Tz maxDistanceDelta)
    {
        if (AreApproximatelyEqual(current, target))
            return target;

        const auto difference       = { target[0] - current[0],
                                        target[1] - current[1],
                                        target[2] - current[2] };
        const auto magnitudeSquared = MagnitudeSquared(difference);

        if (IsApproximatelyZero(magnitudeSquared))
            return target;

        const auto distance = SquareRoot(magnitudeSquared);
        if (distance <= maxDistanceDelta)
            return target;

        const auto factor = maxDistanceDelta / distance;

        return Vector<3, decltype(current[0] + difference[0] * factor)>{
            current[0] + difference[0] * factor,
            current[1] + difference[1] * factor,
            current[2] + difference[2] * factor
        };
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto RotateTowards(const Vector<3, Tx>& current,
                                  const Vector<3, Ty>& target,
                                  Tz maxDeltaAngle,
                                  AngleType angleType = AngleType::Radians)
    {
        if (AreApproximatelyEqual(current, target))
            return target;

        if (angleType == AngleType::Degrees)
            maxDeltaAngle = DegToRad(maxDeltaAngle);

        const auto axis  = Cross(current, target);
        const auto angle = Acos(Clamp(Dot(current, target), -1.0, 1.0));
        maxDeltaAngle = Clamp(maxDeltaAngle, -angle, maxDeltaAngle);

        const auto sinHalfAngle = std::sin(maxDeltaAngle / 2.0);
        const auto cosHalfAngle = std::cos(maxDeltaAngle / 2.0);

        return Vector<3, decltype(current[0] * cosHalfAngle + axis[0] * sinHalfAngle)>{
            cosHalfAngle * current[0] + sinHalfAngle * axis[0],
            cosHalfAngle * current[1] + sinHalfAngle * axis[1],
            cosHalfAngle * current[2] + sinHalfAngle * axis[2]
        };
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto ProjectOnPlane(const Vector<3, Tx>& vector, const Vector<3, Ty>& planeNormal)
    {
        if (IsApproximatelyZero(vector) || IsApproximatelyZero(planeNormal))
            return vector;

        if (AreApproximatelyEqual(vector, planeNormal))
            return Vector<3, decltype(vector[0] - planeNormal[0])>::Zero();

        planeNormal = Normalize(planeNormal);
        const auto dot = Dot(vector, planeNormal);

        return Vector<3, decltype(vector[0] - dot * planeNormal[0])>{
            vector[0] - dot * planeNormal[0],
            vector[1] - dot * planeNormal[1],
            vector[2] - dot * planeNormal[2]
        };
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE void OrthoNormalize(Vector<3, Tx>* normal, Vector<3, Ty>* tangent, Vector<3, Tz>* biNormal)
    {
        // Gram-Schmidt orthogonalization process
        auto normalDeref   = *normal;
        auto tangentDeref  = *tangent;
        auto biNormalDeref = *biNormal;

        // Step 1: Normal
        normalDeref = Normalise(normalDeref);

        // Step 1: Tangent
        tangentDeref = tangentDeref - (normalDeref * (normalDeref[0] * tangentDeref[0]
                                                      + normalDeref[1] * tangentDeref[1]
                                                      + normalDeref[2] * tangentDeref[2]));
        tangentDeref = Normalise(tangentDeref);

        // Step 1: BiNormal
        const auto cross = Cross(normalDeref, tangentDeref);

        biNormalDeref = biNormalDeref - (normalDeref * (normalDeref[0] * biNormalDeref[0]
                                                        + normalDeref[1] * biNormalDeref[1]
                                                        + normalDeref[2] * biNormalDeref[2]));
        biNormalDeref = biNormalDeref - (tangentDeref * (tangentDeref[0] * biNormalDeref[0]
                                                         + tangentDeref[1] * biNormalDeref[1]
                                                         + tangentDeref[2] * biNormalDeref[2]));
        biNormalDeref = biNormalDeref - (cross * (cross[0] * biNormalDeref[0]
                                                  + cross[1] * biNormalDeref[1]
                                                  + cross[2] * biNormalDeref[2]));
        biNormalDeref = Normalize(biNormalDeref);

        *normal   = normalDeref;
        *tangent  = tangentDeref;
        *biNormal = biNormalDeref;
    }
}

#endif //OTTERENGINE_MATH_VECTOR3_H
