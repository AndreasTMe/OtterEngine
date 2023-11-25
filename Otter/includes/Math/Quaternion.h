#ifndef OTTERENGINE_QUATERNION_H
#define OTTERENGINE_QUATERNION_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Math/Math.Concepts.h"
#include "Math/Core.h"

namespace Otter
{
    namespace Math
    {
        template<AnyNumber TNumber>
        struct Quaternion;

        class QuaternionUtils;
    }

    template<AnyNumber TNumber>
    using Quaternion = Math::Quaternion<TNumber>;
    using Quat = Math::QuaternionUtils;
}

namespace Otter::Math
{
    template<AnyNumber TNumber>
    struct Quaternion final
    {
        // HELP: I usually add the member variables at the bottom of the class,
        //      but it's the only member and the class is too large.
    private:
        TNumber m_Values[4];

    public:
        constexpr Quaternion()
        {
            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = static_cast<TNumber>(0.0);
        }

        constexpr explicit Quaternion(TNumber scalar)
        {
            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = scalar;
        }

        constexpr Quaternion(InitialiserList<TNumber> list)
        {
            OTR_ASSERT_MSG(list.size() == 4, "Initialiser list size does not match Quaternion size")

            UInt64 i = 0;
            for (const TNumber& value: list)
                m_Values[i++] = value;
        }

        Quaternion(const Quaternion<TNumber>& other)
        {
            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = other.m_Values[i];
        }

        Quaternion(Quaternion<TNumber>&& other) noexcept
        {
            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = other.m_Values[i];
        }

        Quaternion<TNumber>& operator=(const Quaternion<TNumber>& other)
        {
            if (this == &other)
                return *this;

            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = other.m_Values[i];

            return *this;
        }

        Quaternion<TNumber>& operator=(Quaternion<TNumber>&& other) noexcept
        {
            if (this == &other)
                return *this;

            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = other.m_Values[i];

            return *this;
        }

        TNumber& operator[](UInt8 index)
        {
            OTR_ASSERT_MSG(index < 4, "Index {0} is out of range", index)
            return m_Values[index];
        }

        const TNumber& operator[](UInt8 index) const
        {
            OTR_ASSERT_MSG(index < 4, "Index {0} is out of range", index)
            return m_Values[index];
        }

        template<AnyNumber TOtherNumber>
        explicit operator Quaternion<TOtherNumber>() const
        {
            Quaternion<TOtherNumber> result;

            for (UInt8 i = 0; i < 4; ++i)
                result[i] = static_cast<TOtherNumber>(m_Values[i]);

            return result;
        }

        template<AnyNumber TOtherNumber>
        Quaternion<TNumber>& operator*=(const Quaternion<TOtherNumber>& other)
        {
            *this = Quaternion<TNumber>{
                m_Values[3] * other.m_Values[0]
                + m_Values[0] * other.m_Values[3]
                + m_Values[1] * other.m_Values[2]
                - m_Values[2] * other.m_Values[1],

                m_Values[3] * other.m_Values[1]
                + m_Values[1] * other.m_Values[3]
                + m_Values[2] * other.m_Values[0]
                - m_Values[0] * other.m_Values[2],

                m_Values[3] * other.m_Values[2]
                + m_Values[2] * other.m_Values[3]
                + m_Values[0] * other.m_Values[1]
                - m_Values[1] * other.m_Values[0],

                m_Values[3] * other.m_Values[3]
                - m_Values[0] * other.m_Values[0]
                - m_Values[1] * other.m_Values[1]
                - m_Values[2] * other.m_Values[2]
            };

            return *this;
        }

        template<AnyNumber TOtherNumber>
        Quaternion<TNumber>& operator*=(TOtherNumber scalar) noexcept
        {
            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] *= scalar;

            return *this;
        }

        template<AnyNumber TOtherNumber>
        Quaternion<TNumber>& operator/=(TOtherNumber scalar) noexcept
        {
            OTR_ASSERT_MSG(scalar != 0, "Division by zero")

            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] /= scalar;

            return *this;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator*(Quaternion<TNumber> lhs, const Quaternion<TOtherNumber>& rhs)
        {
            lhs *= rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator*(Quaternion<TNumber> lhs, TOtherNumber rhs)
        {
            lhs *= rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator/(Quaternion<TNumber> lhs, TOtherNumber rhs)
        {
            OTR_ASSERT_MSG(rhs != 0, "Division by zero")

            lhs /= rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        bool operator==(const Quaternion<TOtherNumber>& other) const noexcept
        {
            if constexpr (IntegerNumber<TNumber> && IntegerNumber<TOtherNumber>)
            {
                for (UInt8 i = 0; i < 4; ++i)
                    if (m_Values[i] != other.m_Values[i])
                        return false;

                return true;
            }

            for (UInt8 i = 0; i < 4; ++i)
                if (!Math::AreApproximatelyEqual(m_Values[i], other.m_Values[i]))
                    return false;

            return true;
        }

        template<AnyNumber TOtherNumber>
        bool operator!=(const Quaternion<TOtherNumber>& other) const noexcept
        {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "Simplify"

            return !(*this == other);

#pragma clang diagnostic pop
        }

        [[nodiscard]] OTR_INLINE TNumber GetX() const noexcept { return m_Values[0]; }
        OTR_INLINE void SetX(TNumber x) noexcept { m_Values[0] = x; }

        [[nodiscard]] OTR_INLINE TNumber GetY() const noexcept { return m_Values[1]; }
        OTR_INLINE void SetY(TNumber y) noexcept { m_Values[1] = y; }

        [[nodiscard]] OTR_INLINE TNumber GetZ() const noexcept { return m_Values[2]; }
        OTR_INLINE void SetZ(TNumber z) noexcept { m_Values[2] = z; }

        [[nodiscard]] OTR_INLINE TNumber GetW() const noexcept { return m_Values[3]; }
        OTR_INLINE void SetW(TNumber w) noexcept { m_Values[3] = w; }

        OTR_INLINE static constexpr Quaternion<TNumber> Zero() noexcept
        {
            return Quaternion<TNumber>(static_cast<TNumber>(0.0));
        }

        OTR_INLINE static constexpr Quaternion<TNumber> One() noexcept
        {
            return Quaternion<TNumber>(static_cast<TNumber>(1.0));
        }

        OTR_INLINE static constexpr Quaternion<TNumber> Identity() noexcept
        {
            return Quaternion<TNumber>{ static_cast<TNumber>(0.0),
                                        static_cast<TNumber>(0.0),
                                        static_cast<TNumber>(0.0),
                                        static_cast<TNumber>(1.0) };
        }
    };

    class QuaternionUtils final
    {
    public:
        template<AnyNumber TNumber>
        OTR_INLINE static bool IsApproximatelyZero(const Quaternion<TNumber>& quaternion)
        {
            if constexpr (IntegerNumber<TNumber>)
                return quaternion[0] == 0 && quaternion[1] == 0 && quaternion[2] == 0 && quaternion[3] == 0;

            return Math::IsApproximatelyZero(quaternion[0])
                   && Math::IsApproximatelyZero(quaternion[1])
                   && Math::IsApproximatelyZero(quaternion[2])
                   && Math::IsApproximatelyZero(quaternion[3]);
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static bool AreApproximatelyEqual(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs)
        {
            if constexpr (IntegerNumber<Tx> && IntegerNumber<Ty>)
                return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2] && lhs[3] == rhs[3];

            return Math::AreApproximatelyEqual(lhs[0], rhs[0])
                   && Math::AreApproximatelyEqual(lhs[1], rhs[1])
                   && Math::AreApproximatelyEqual(lhs[1], rhs[2])
                   && Math::AreApproximatelyEqual(lhs[2], rhs[3]);
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto MagnitudeSquared(const Quaternion<TNumber>& quaternion)
        {
            return Math::Square(quaternion[0]) + Math::Square(quaternion[1])
                   + Math::Square(quaternion[2]) + Math::Square(quaternion[3]);
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Magnitude(const Quaternion<TNumber>& quaternion)
        {
            return Math::SquareRoot(MagnitudeSquared(quaternion));
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Normalise(const Quaternion<TNumber>& quaternion)
        {
            const auto magnitude = Magnitude(quaternion);
            if (magnitude < Epsilon<Double128>)
                return Quaternion<TNumber>::Zero();

            return quaternion / magnitude;
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Inverse(const Quaternion<TNumber>& quaternion)
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
        OTR_INLINE static auto Dot(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs)
        {
            return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3];
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Angle(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs)
        {
            const auto lhsNormalised = Normalise(lhs);
            const auto rhsNormalised = Normalise(rhs);

            const auto dotProduct = Math::Clamp(Dot(lhsNormalised, rhsNormalised), -1.0, 1.0);

            return Math::Acos(dotProduct);
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto Lerp(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs, Tz t)
        {
            const auto dot = Dot(lhs, rhs);
            if (dot < 0.0)
            {
                return Quaternion<decltype(lhs[0] - t * (lhs[0] + rhs[0]))>{
                    lhs[0] - t * (lhs[0] + rhs[0]),
                    lhs[1] - t * (lhs[1] + rhs[1]),
                    lhs[2] - t * (lhs[2] + rhs[2]),
                    lhs[3] - t * (lhs[3] + rhs[3])
                };
            }

            return Quaternion<decltype(lhs[0] - t * (lhs[0] - rhs[0]))>{
                lhs[0] - t * (lhs[0] - rhs[0]),
                lhs[1] - t * (lhs[1] - rhs[1]),
                lhs[2] - t * (lhs[2] - rhs[2]),
                lhs[3] - t * (lhs[3] - rhs[3])
            };
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto LerpClamped(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs, Tz t)
        {
            return Lerp(lhs, rhs, Math::Clamp(t, (Tz) 0.0, (Tz) 1.0));
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto Slerp(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs, Tz t)
        {
            const auto lhsNormalised = Normalise(lhs);
            const auto rhsNormalised = Normalise(rhs);

            const auto dotProduct = Math::Clamp(Dot(lhsNormalised, rhsNormalised), -1.0, 1.0);
            const auto sign       = Math::Sign(dotProduct);

            if (Math::IsApproximatelyZero(dotProduct - static_cast<decltype(dotProduct)>(1.0)))
            {
                return Quaternion<decltype(lhsNormalised[0] + t * sign * (rhsNormalised[0] - lhsNormalised[0]))>{
                    lhsNormalised[0] + t * sign * (rhsNormalised[0] - lhsNormalised[0]),
                    lhsNormalised[1] + t * sign * (rhsNormalised[1] - lhsNormalised[1]),
                    lhsNormalised[2] + t * sign * (rhsNormalised[2] - lhsNormalised[2]),
                    lhsNormalised[3] + t * sign * (rhsNormalised[3] - lhsNormalised[3])
                };
            }

            const auto angle  = Math::Acos(dotProduct);
            const auto scale1 = Math::Sin((1.0f - t) * angle) / Math::Sin(angle);
            const auto scale2 = Math::Sin(t * angle) / Math::Sin(angle * sign);

            return Quaternion<decltype(scale1 * lhsNormalised[0] + scale2 * rhsNormalised[0])>{
                scale1 * lhsNormalised[0] + scale2 * rhsNormalised[0],
                scale1 * lhsNormalised[1] + scale2 * rhsNormalised[1],
                scale1 * lhsNormalised[2] + scale2 * rhsNormalised[2],
                scale1 * lhsNormalised[3] + scale2 * rhsNormalised[3]
            };
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto SlerpClamped(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs, Tz t)
        {
            return Slerp(lhs, rhs, Math::Clamp(t, (Tz) 0.0, (Tz) 1.0));
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto MoveTowards(const Quaternion<Tx>& current,
                                           const Quaternion<Ty>& target,
                                           Tz maxAngleDelta,
                                           AngleType angleType = AngleType::Radians)
        {
            if (AreApproximatelyEqual(current, target))
                return target;

            if (angleType == AngleType::Degrees)
                maxAngleDelta = Math::DegToRad(maxAngleDelta);

            const auto memberWiseMul = current[0] * target[0]
                                       + current[1] * target[1]
                                       + current[2] * target[2]
                                       + current[3] * target[3];
            const auto acos          = Math::Acos(memberWiseMul);

            if (Math::IsApproximatelyZero(acos))
                return target;

            return Slerp(current, target, Math::Min(1.0f, maxAngleDelta / acos));
        }
    };
}

#endif //OTTERENGINE_QUATERNION_H
