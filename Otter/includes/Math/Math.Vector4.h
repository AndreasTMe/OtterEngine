#ifndef OTTERENGINE_MATH_VECTOR4_H
#define OTTERENGINE_MATH_VECTOR4_H

#include "Math/Vector.h"

namespace Otter::Math
{
    template<AnyNumber TNumber>
    OTR_INLINE bool IsApproximatelyZero(const Vector<4, TNumber>& vector)
    {
        if constexpr (IntegerNumber<TNumber>)
            return vector[0] == 0 && vector[1] == 0 && vector[2] == 0 && vector[3] == 0;

        return Math::IsApproximatelyZero(vector[0])
               && Math::IsApproximatelyZero(vector[1])
               && Math::IsApproximatelyZero(vector[2])
               && Math::IsApproximatelyZero(vector[3]);
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE bool AreApproximatelyEqual(const Vector<4, Tx>& lhs, const Vector<4, Ty>& rhs)
    {
        if constexpr (IntegerNumber<Tx> && IntegerNumber<Ty>)
            return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2] && lhs[3] == rhs[3];

        return Math::AreApproximatelyEqual(lhs[0], rhs[0])
               && Math::AreApproximatelyEqual(lhs[1], rhs[1])
               && Math::AreApproximatelyEqual(lhs[2], rhs[2])
               && Math::AreApproximatelyEqual(lhs[3], rhs[3]);
    }

    template<AnyNumber TNumber>
    OTR_INLINE auto MagnitudeSquared(const Vector<4, TNumber>& vector)
    {
        return Square(vector[0]) + Square(vector[1]) + Square(vector[2]);
    }

    template<AnyNumber TNumber>
    OTR_INLINE auto Magnitude(const Vector<4, TNumber>& vector) { return SquareRoot(MagnitudeSquared(vector)); }

    template<AnyNumber TNumber, AnyNumber TMaxMagnitude>
    OTR_INLINE auto ClampMagnitude(const Vector<4, TNumber>& vector, const TMaxMagnitude& maxMagnitude)
    {
        const auto magnitudeSquared = MagnitudeSquared(vector);
        if (magnitudeSquared > Square(maxMagnitude))
            return vector * Min(maxMagnitude / SquareRoot(magnitudeSquared), 1);

        return vector;
    }

    template<AnyNumber TNumber>
    OTR_INLINE auto Normalise(const Vector<4, TNumber>& vector)
    {
        const auto magnitude = Magnitude(vector);
        if (magnitude < Epsilon<Double128>)
            return Vector<4, TNumber>::Zero();

        return vector / magnitude;
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Dot(const Vector<4, Tx>& lhs, const Vector<4, Ty>& rhs)
    {
        return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3];
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Cross(const Vector<4, Tx>& lhs, const Vector<4, Ty>& rhs)
    {
        return Vector<4, decltype(lhs[0] * rhs[0] - lhs[0] * rhs[0] + lhs[0] * rhs[0])>{
            lhs[1] * rhs[2] - lhs[2] * rhs[1],
            lhs[2] * rhs[0] - lhs[0] * rhs[2],
            lhs[0] * rhs[1] - lhs[1] * rhs[0]
        };
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto DistanceSquared(const Vector<4, Tx>& lhs, const Vector<4, Ty>& rhs)
    {
        return MagnitudeSquared(lhs - rhs);
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Distance(const Vector<4, Tx>& lhs, const Vector<4, Ty>& rhs)
    {
        return Magnitude(lhs - rhs);
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto Lerp(const Vector<4, Tx>& lhs, const Vector<4, Ty>& rhs, const Tz& t)
    {
        return lhs + (rhs - lhs) * t;
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto LerpClamped(const Vector<4, Tx>& lhs, const Vector<4, Ty>& rhs, const Tz& t)
    {
        return Lerp(lhs, rhs, Clamp(t, (Tz) 0.0, (Tz) 1.0));
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Max(const Vector<4, Tx>& lhs, const Vector<4, Ty>& rhs)
    {
        return Vector<4, decltype(Math::Max(lhs[0], rhs[0]))>(Math::Max(lhs[0], rhs[0]),
                                                              Math::Max(lhs[1], rhs[1]),
                                                              Math::Max(lhs[2], rhs[2]),
                                                              Math::Max(lhs[3], rhs[3]));
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Min(const Vector<4, Tx>& lhs, const Vector<4, Ty>& rhs)
    {
        return Vector<4, decltype(Math::Min(lhs[0], rhs[0]))>(Math::Min(lhs[0], rhs[0]),
                                                              Math::Min(lhs[1], rhs[1]),
                                                              Math::Min(lhs[2], rhs[2]),
                                                              Math::Min(lhs[3], rhs[3]));
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Project(const Vector<4, Tx>& vector, const Vector<4, Ty>& normal)
    {
        const auto magnitudeSquared = MagnitudeSquared(normal);
        if (magnitudeSquared < Epsilon<Double128>)
            return Vector<4, decltype(vector[0] * normal[0])>::Zero();

        return (Dot(vector, normal) / magnitudeSquared) * normal;
    }

    // TODO: Vector4::MoveTowards
}

#endif //OTTERENGINE_MATH_VECTOR4_H
