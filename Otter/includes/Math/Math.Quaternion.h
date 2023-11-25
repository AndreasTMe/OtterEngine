#ifndef OTTERENGINE_MATH_QUATERNION_H
#define OTTERENGINE_MATH_QUATERNION_H

#include "Math/Quaternion.h"

namespace Otter::Math
{
    template<AnyNumber TNumber>
    OTR_INLINE bool IsApproximatelyZero(const Quaternion<TNumber>& quaternion)
    {
        if constexpr (IntegerNumber<TNumber>)
            return quaternion[0] == 0 && quaternion[1] == 0 && quaternion[2] == 0 && quaternion[3] == 0;

        return Math::IsApproximatelyZero(quaternion[0])
               && Math::IsApproximatelyZero(quaternion[1])
               && Math::IsApproximatelyZero(quaternion[2])
               && Math::IsApproximatelyZero(quaternion[3]);
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE bool AreApproximatelyEqual(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs)
    {
        if constexpr (IntegerNumber<Tx> && IntegerNumber<Ty>)
            return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2] && lhs[3] == rhs[3];

        return Math::AreApproximatelyEqual(lhs[0], rhs[0])
               && Math::AreApproximatelyEqual(lhs[1], rhs[1])
               && Math::AreApproximatelyEqual(lhs[1], rhs[2])
               && Math::AreApproximatelyEqual(lhs[2], rhs[3]);
    }

    template<AnyNumber TNumber>
    OTR_INLINE auto MagnitudeSquared(const Quaternion<TNumber>& quaternion)
    {
        return Square(quaternion[0]) + Square(quaternion[1]) + Square(quaternion[2]) + Square(quaternion[3]);
    }

    template<AnyNumber TNumber>
    OTR_INLINE auto
    Magnitude(const Quaternion<TNumber>& quaternion) { return SquareRoot(MagnitudeSquared(quaternion)); }

    template<AnyNumber TNumber>
    OTR_INLINE auto Normalise(const Quaternion<TNumber>& quaternion)
    {
        const auto magnitude = Magnitude(quaternion);
        if (magnitude < Epsilon<Double128>)
            return Quaternion<TNumber>::Zero();

        return quaternion / magnitude;
    }

    template<AnyNumber TNumber>
    OTR_INLINE auto Inverse(const Quaternion<TNumber>& quaternion)
    {
        const auto dot = Dot(quaternion, quaternion);
        if (dot > (TNumber) 0)
        {
            const auto invDot = (TNumber) 1 / dot;

            return Quaternion<TNumber>{
                -quaternion[0] * invDot,
                -quaternion[1] * invDot,
                -quaternion[2] * invDot,
                quaternion[3] * invDot
            };
        }

        return Quaternion<TNumber>::Zero();
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Dot(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs)
    {
        return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3];
    }

    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE auto Angle(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs)
    {
        // TODO: Quaternion::Angle
        return 0.0;
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto Lerp(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs, const Tz& t)
    {
        const auto dot = Dot(lhs, rhs);
        if (dot < 0.0)
        {
            return Quaternion<Tx>{
                lhs[0] - t * (lhs[0] + rhs[0]),
                lhs[1] - t * (lhs[1] + rhs[1]),
                lhs[2] - t * (lhs[2] + rhs[2]),
                lhs[3] - t * (lhs[3] + rhs[3])
            };
        }

        return Quaternion<Tx>{
            lhs[0] - t * (lhs[0] - rhs[0]),
            lhs[1] - t * (lhs[1] - rhs[1]),
            lhs[2] - t * (lhs[2] - rhs[2]),
            lhs[3] - t * (lhs[3] - rhs[3])
        };
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto LerpClamped(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs, const Tz& t)
    {
        return Lerp(lhs, rhs, Clamp(t, (Tz) 0.0, (Tz) 1.0));
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto Slerp(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs, const Tz& t)
    {
        // TODO: Quaternion::Slerp
        return Quaternion<Tx>();
    }

    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto SlerpClamped(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs, const Tz& t)
    {
        return Slerp(lhs, rhs, Clamp(t, (Tz) 0.0, (Tz) 1.0));
    }

    // TODO: Quaternion::MoveTowards
}

#endif //OTTERENGINE_MATH_QUATERNION_H
