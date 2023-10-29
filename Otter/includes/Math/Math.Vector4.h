#ifndef OTTERENGINE_MATH_VECTOR4_H
#define OTTERENGINE_MATH_VECTOR4_H

#include "Math/Core.h"
#include "Math/Vector.h"

namespace Otter::Math
{
    template<>
    OTR_INLINE Vector<4, Int32> VectorOne() { return Vector<4, Int32>(1, 1, 1, 1); }

    template<>
    OTR_INLINE Vector<4, Int32> VectorZero() { return Vector<4, Int32>(0, 0, 0, 0); }

    template<AnyNumber TNumber>
    OTR_INLINE Vector<4, TNumber> VectorPositiveInfinity()
    {
        return Vector<4, TNumber>(PositiveInfinity<TNumber>(),
                                  PositiveInfinity<TNumber>(),
                                  PositiveInfinity<TNumber>(),
                                  PositiveInfinity<TNumber>());
    }

    template<AnyNumber TNumber>
    OTR_INLINE Vector<4, TNumber> VectorNegativeInfinity()
    {
        return Vector<4, TNumber>(NegativeInfinity<TNumber>(),
                                  NegativeInfinity<TNumber>(),
                                  NegativeInfinity<TNumber>(),
                                  NegativeInfinity<TNumber>());
    }

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
               && Math::AreApproximatelyEqual(lhs[1], rhs[2])
               && Math::AreApproximatelyEqual(lhs[2], rhs[3]);
    }

    template<AnyNumber TNumber>
    OTR_INLINE auto MagnitudeSquared(const Vector<4, TNumber>& vector)
    {
        return Square(vector[0]) + Square(vector[1]) + Square(vector[2]);
    }

    template<AnyNumber TNumber>
    OTR_INLINE auto Magnitude(const Vector<4, TNumber>& vector) { return Sqrt(MagnitudeSquared(vector)); }

    template<AnyNumber TNumber, AnyNumber TMaxMagnitude>
    OTR_INLINE auto ClampMagnitude(const Vector<4, TNumber>& vector, const TMaxMagnitude& maxMagnitude)
    {
        return vector * Min(maxMagnitude / Magnitude(vector), 1);
    }

    template<AnyNumber TNumber>
    OTR_INLINE auto Normalise(const Vector<4, TNumber>& vector) { return vector / Magnitude(vector); }

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
        return Lerp(lhs, rhs, Clamp(t, 0.0, 1.0));
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
        auto magnitudeSquared = MagnitudeSquared(normal);
        if (magnitudeSquared < Epsilon<Double128>)
            return VectorZero<4>;

        return (Dot(vector, normal) / magnitudeSquared) * normal;
    }

    // TODO: Vector4::MoveTowards
}

#endif //OTTERENGINE_MATH_VECTOR4_H
