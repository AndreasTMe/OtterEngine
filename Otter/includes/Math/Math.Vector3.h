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
    OTR_INLINE auto Lerp(const Vector<3, Tx>& lhs, const Vector<3, Ty>& rhs, const Tz& t)
    {
        return lhs + (rhs - lhs) * t;
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto LerpClamped(const Vector<3, Tx>& lhs, const Vector<3, Ty>& rhs, const Tz& t)
    {
        return Lerp(lhs, rhs, Clamp(t, (Tz) 0.0, (Tz) 1.0));
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto Slerp(const Vector<3, Tx>& lhs, const Vector<3, Ty>& rhs, const Tz& t)
    {
        auto dotClamped = Clamp(Dot(lhs, rhs), -1.0, 1.0);
        auto theta      = Acos(dotClamped) * t;
        auto relative   = Normalise(rhs - lhs * dotClamped);

        return ((lhs * Cos(theta)) + (relative * Sin(theta)));
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto SlerpClamped(const Vector<3, Tx>& lhs, const Vector<3, Ty>& rhs, const Tz& t)
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

    // TODO: Vector3::MoveTowards
    // TODO: Vector3::RotateTowards
    // TODO: Vector3::ProjectOnPlane
    // TODO: Vector3::OrthoNormalize
}

#endif //OTTERENGINE_MATH_VECTOR3_H
