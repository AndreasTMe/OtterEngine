#ifndef OTTERENGINE_MATH_VECTOR2_H
#define OTTERENGINE_MATH_VECTOR2_H

#include "Math/Vector.h"

namespace Otter::Math
{
    template<>
    OTR_INLINE constexpr Vector<2, Int32> Left() { return Vector<2, Int32>{ -1, 0 }; }

    template<>
    OTR_INLINE constexpr Vector<2, Int32> Right() { return Vector<2, Int32>{ 1, 0 }; }

    template<>
    OTR_INLINE constexpr Vector<2, Int32> Down() { return Vector<2, Int32>{ 0, -1 }; }

    template<>
    OTR_INLINE constexpr Vector<2, Int32> Up() { return Vector<2, Int32>{ 0, 1 }; }

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
    OTR_INLINE auto Magnitude(const Vector<2, TNumber>& vector) { return Sqrt(MagnitudeSquared(vector)); }

    template<AnyNumber TNumber, AnyNumber TMaxMagnitude>
    OTR_INLINE auto ClampMagnitude(const Vector<2, TNumber>& vector, const TMaxMagnitude& maxMagnitude)
    {
        const auto magnitudeSquared = MagnitudeSquared(vector);
        if (magnitudeSquared > Square(maxMagnitude))
            return vector * Min(maxMagnitude / Sqrt(magnitudeSquared), 1);

        return vector;
    }

    template<AnyNumber TNumber>
    OTR_INLINE auto Normalise(const Vector<2, TNumber>& vector)
    {
        const auto magnitude = Magnitude(vector);
        if (magnitude < Epsilon<Double128>)
            return VectorZero<2>;

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
    OTR_INLINE auto Lerp(const Vector<2, Tx>& lhs, const Vector<2, Ty>& rhs, const Tz& t)
    {
        return lhs + (rhs - lhs) * t;
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto LerpClamped(const Vector<2, Tx>& lhs, const Vector<2, Ty>& rhs, const Tz& t)
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

    // TODO: Vector2::SmoothStep
    // TODO: Vector2::SmoothDamp
    // TODO: Vector2::SmoothDampAngle
    // TODO: Vector2::MoveTowards
    // TODO: Vector2::RotateTowards
}

#endif //OTTERENGINE_MATH_VECTOR2_H
