#ifndef OTTERENGINE_VECTOR_H
#define OTTERENGINE_VECTOR_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Math/Math.Concepts.h"
#include "Math/Core.h"

namespace Otter
{
    namespace Math
    {
        template<UInt8 TDimension, AnyNumber TNumber> requires Dimension<TDimension>
        struct Vector;

        class Vector2DUtils;

        class Vector3DUtils;

        class Vector4DUtils;
    }

    template<AnyNumber TNumber>
    using Vector2D = Math::Vector<2, TNumber>;
    using Vec2D = Math::Vector2DUtils;

    template<AnyNumber TNumber>
    using Vector3D = Math::Vector<3, TNumber>;
    using Vec3D = Math::Vector3DUtils;

    template<AnyNumber TNumber>
    using Vector4D = Math::Vector<4, TNumber>;
    using Vec4D = Math::Vector4DUtils;
}

namespace Otter::Math
{
    template<UInt8 TDimension, AnyNumber TNumber> requires Dimension<TDimension>
    struct Vector final
    {
        // HELP: I usually add the member variables at the bottom of the class,
        //      but it's the only member and the class is too large.
    private:
        TNumber m_Values[TDimension];

    public:
        constexpr Vector()
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = static_cast<TNumber>(0.0);
        }

        constexpr explicit Vector(TNumber scalar)
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = scalar;
        }

        constexpr Vector(InitialiserList<TNumber> list)
        {
            OTR_ASSERT_MSG(list.size() == TDimension, "Initialiser list size does not match Vector size")

            UInt64 i = 0;
            for (const TNumber& value: list)
                m_Values[i++] = value;
        }

        Vector(const Vector<TDimension, TNumber>& other)
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = other.m_Values[i];
        }

        Vector(Vector<TDimension, TNumber>&& other) noexcept
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = other.m_Values[i];
        }

        Vector<TDimension, TNumber>& operator=(const Vector<TDimension, TNumber>& other)
        {
            if (this == &other)
                return *this;

            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = other.m_Values[i];

            return *this;
        }

        Vector<TDimension, TNumber>& operator=(Vector<TDimension, TNumber>&& other) noexcept
        {
            if (this == &other)
                return *this;

            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = other.m_Values[i];

            return *this;
        }

        TNumber& operator[](UInt8 index)
        {
            OTR_ASSERT_MSG(index < TDimension, "Index {0} is out of range", index)
            return m_Values[index];
        }

        const TNumber& operator[](UInt8 index) const
        {
            OTR_ASSERT_MSG(index < TDimension, "Index {0} is out of range", index)
            return m_Values[index];
        }

        template<AnyNumber TOtherNumber>
        explicit operator Vector<TDimension, TOtherNumber>() const
        {
            Vector<TDimension, TOtherNumber> result;

            for (UInt8 i = 0; i < TDimension; ++i)
                result.m_Values[i] = static_cast<TOtherNumber>(m_Values[i]);

            return result;
        }

        template<AnyNumber TOtherNumber>
        Vector<TDimension, TNumber>& operator+=(const Vector<TDimension, TOtherNumber>& other)
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] += static_cast<TNumber>(other.m_Values[i]);

            return *this;
        }

        Vector<TDimension, TNumber>& operator+=(const Vector<TDimension, TNumber>& other) noexcept
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] += other.m_Values[i];

            return *this;
        }

        template<AnyNumber TOtherNumber>
        Vector<TDimension, TNumber>& operator-=(const Vector<TDimension, TOtherNumber>& other)
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] -= static_cast<TNumber>(other.m_Values[i]);

            return *this;
        }

        Vector<TDimension, TNumber>& operator-=(const Vector<TDimension, TNumber>& other) noexcept
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] -= other.m_Values[i];

            return *this;
        }

        template<AnyNumber TOtherNumber>
        Vector<TDimension, TNumber>& operator*=(TOtherNumber scalar) noexcept
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] *= scalar;

            return *this;
        }

        template<AnyNumber TOtherNumber>
        Vector<TDimension, TNumber>& operator/=(TOtherNumber scalar) noexcept
        {
            OTR_ASSERT_MSG(scalar != 0, "Division by zero")

            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] /= scalar;

            return *this;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator+(Vector<TDimension, TNumber> lhs,
                                        const Vector<TDimension, TOtherNumber>& rhs)
        {
            lhs += rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator-(Vector<TDimension, TNumber> lhs,
                                        const Vector<TDimension, TOtherNumber>& rhs)
        {
            lhs -= rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator*(Vector<TDimension, TNumber> lhs, TOtherNumber rhs)
        {
            lhs *= rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator/(Vector<TDimension, TNumber> lhs, TOtherNumber rhs)
        {
            OTR_ASSERT_MSG(rhs != 0, "Division by zero")

            lhs /= rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        bool operator==(const Vector<TDimension, TOtherNumber>& other) const noexcept
        {
            if constexpr (IntegerNumber<TNumber> && IntegerNumber<TOtherNumber>)
            {
                for (UInt8 i = 0; i < TDimension; ++i)
                    if (m_Values[i] != other.m_Values[i])
                        return false;

                return true;
            }

            for (UInt8 i = 0; i < TDimension; ++i)
                if (!Math::AreApproximatelyEqual(m_Values[i], other.m_Values[i]))
                    return false;

            return true;
        }

        template<AnyNumber TOtherNumber>
        bool operator!=(const Vector<TDimension, TOtherNumber>& other) const noexcept
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

        [[nodiscard]] OTR_INLINE TNumber GetZ() const noexcept requires (TDimension >= 3) { return m_Values[2]; }
        OTR_INLINE void SetZ(TNumber z) noexcept requires (TDimension >= 3) { m_Values[2] = z; }

        [[nodiscard]] OTR_INLINE TNumber GetW() const noexcept requires (TDimension == 4) { return m_Values[3]; }
        OTR_INLINE void SetW(TNumber w) noexcept requires (TDimension == 4) { m_Values[3] = w; }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> Left() requires (TDimension < 4)
        {
            if constexpr (TDimension == 2)
                return Vector<2, TNumber>(static_cast<TNumber>(-1.0),
                                          static_cast<TNumber>(0.0));
            else
                return Vector<3, TNumber>(static_cast<TNumber>(-1.0),
                                          static_cast<TNumber>(0.0),
                                          static_cast<TNumber>(0.0));
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> Right() requires (TDimension < 4)
        {
            if constexpr (TDimension == 2)
                return Vector<2, TNumber>(static_cast<TNumber>(1.0), static_cast<TNumber>(0.0));
            else
                return Vector<3, TNumber>(static_cast<TNumber>(1.0),
                                          static_cast<TNumber>(0.0),
                                          static_cast<TNumber>(0.0));
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> Down() requires (TDimension < 4)
        {
            if constexpr (TDimension == 2)
                return Vector<2, TNumber>(static_cast<TNumber>(0.0), static_cast<TNumber>(-1.0));
            else
                return Vector<3, TNumber>(static_cast<TNumber>(0.0),
                                          static_cast<TNumber>(-1.0),
                                          static_cast<TNumber>(0.0));
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> Up() requires (TDimension < 4)
        {
            if constexpr (TDimension == 2)
                return Vector<2, TNumber>(static_cast<TNumber>(0.0), static_cast<TNumber>(1.0));
            else
                return Vector<3, TNumber>(static_cast<TNumber>(0.0),
                                          static_cast<TNumber>(1.0),
                                          static_cast<TNumber>(0.0));
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> Back() requires (TDimension == 3)
        {
            return Vector<3, TNumber>(static_cast<TNumber>(0.0), static_cast<TNumber>(0.0), static_cast<TNumber>(-1.0));
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> Forward() requires (TDimension == 3)
        {
            return Vector<3, TNumber>(static_cast<TNumber>(0.0), static_cast<TNumber>(0.0), static_cast<TNumber>(1.0));
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> One()
        {
            return Vector<TDimension, TNumber>(static_cast<TNumber>(1.0));
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> Zero()
        {
            return Vector<TDimension, TNumber>(static_cast<TNumber>(0.0));
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> PositiveInfinity()
        {
            return Vector<TDimension, TNumber>(Math::PositiveInfinity<TNumber>());
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> NegativeInfinity()
        {
            return Vector<TDimension, TNumber>(Math::NegativeInfinity<TNumber>());
        }
    };

    class Vector2DUtils final
    {
        template<AnyNumber TNumber>
        using Vec2D = Vector<2, TNumber>;

    public:
        template<AnyNumber TNumber>
        OTR_INLINE static bool IsApproximatelyZero(const Vec2D<TNumber>& vector)
        {
            if constexpr (IntegerNumber<TNumber>)
                return vector[0] == 0 && vector[1] == 0;

            return Math::IsApproximatelyZero(vector[0]) && Math::IsApproximatelyZero(vector[1]);
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static bool AreApproximatelyEqual(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs)
        {
            if constexpr (IntegerNumber<Tx> && IntegerNumber<Ty>)
                return lhs[0] == rhs[0] && lhs[1] == rhs[1];

            return Math::AreApproximatelyEqual(lhs[0], rhs[0]) && Math::AreApproximatelyEqual(lhs[1], rhs[1]);
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto MagnitudeSquared(const Vec2D<TNumber>& vector)
        {
            return Math::Square(vector[0]) + Math::Square(vector[1]);
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Magnitude(const Vec2D<TNumber>& vector)
        {
            return Math::SquareRoot(MagnitudeSquared(vector));
        }

        template<AnyNumber TNumber, AnyNumber TMaxMagnitude>
        OTR_INLINE static auto ClampMagnitude(const Vec2D<TNumber>& vector, const TMaxMagnitude& maxMagnitude)
        {
            const auto magnitudeSquared = MagnitudeSquared(vector);
            if (magnitudeSquared > Math::Square(maxMagnitude))
                return vector * Math::Min(maxMagnitude / Math::SquareRoot(magnitudeSquared), 1);

            return vector;
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Normalise(const Vec2D<TNumber>& vector)
        {
            const auto magnitude = Magnitude(vector);
            if (magnitude < Epsilon<Double128>)
                return Vec2D<TNumber>::Zero();

            return vector / magnitude;
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Dot(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs)
        {
            return lhs[0] * rhs[0] + lhs[1] * rhs[1];
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto DistanceSquared(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs)
        {
            return MagnitudeSquared(lhs - rhs);
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Distance(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs)
        {
            return Magnitude(lhs - rhs);
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto Lerp(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs, Tz t)
        {
            return lhs + (rhs - lhs) * t;
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto LerpClamped(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs, Tz t)
        {
            return Lerp(lhs, rhs, Math::Clamp(t, (Tz) 0.0, (Tz) 1.0));
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Max(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs)
        {
            return Vec2D<decltype(Math::Max(lhs[0], rhs[0]))>(Math::Max(lhs[0], rhs[0]),
                                                              Math::Max(lhs[1], rhs[1]));
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Min(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs)
        {
            return Vec2D<decltype(Math::Min(lhs[0], rhs[0]))>(Math::Min(lhs[0], rhs[0]),
                                                              Math::Min(lhs[1], rhs[1]));
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto Clamp(const Vec2D<Tx>& value, const Vec2D<Ty>& min, const Vec2D<Tz>& max)
        {
            return Vec2D<decltype(Math::Clamp(value[0], min[0], max[0]))>(Math::Clamp(value[0], min[0], max[0]),
                                                                          Math::Clamp(value[1], min[1], max[1]));
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Reflect(const Vec2D<Tx>& vector, const Vec2D<Ty>& normal)
        {
            return vector - 2 * Dot(vector, normal) * normal;
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Angle(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs)
        {
            const auto magnitudeProduct = Magnitude(lhs) * Magnitude(rhs);
            if (magnitudeProduct < Epsilon<Double128>)
                return 0;

            return Math::Acos(Dot(lhs, rhs) / magnitudeProduct);
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto AngleSigned(const Vec2D<Tx>& from, const Vec2D<Ty>& to)
        {
            return Math::Sign(from[0] * to[1] - from[1] * to[0]) * Angle(from, to);
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto SmoothStep(const Vec2D<Tx>& min, const Vec2D<Ty>& max, const Vec2D<Tz>& value)
        {
            return Vec2D<decltype(Math::SmoothStep(min[0], max[0], value[0]))>{
                Math::SmoothStep(min[0], max[0], value[0]),
                Math::SmoothStep(min[1], max[1], value[1])
            };
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto InverseSmoothStep(const Vec2D<Tx>& min,
                                                 const Vec2D<Ty>& max,
                                                 const Vec2D<Tz>& smoothenedValue)
        {
            return Vec2D<decltype(Math::InverseSmoothStep(min, max, smoothenedValue[0]))>{
                Math::InverseSmoothStep(min[0], max[0], smoothenedValue[0]),
                Math::InverseSmoothStep(min[1], max[1], smoothenedValue[1])
            };
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto MoveTowards(const Vec2D<Tx>& current, const Vec2D<Ty>& target, Tz maxDistanceDelta)
        {
            if (AreApproximatelyEqual(current, target))
                return target;

            const auto difference       = { target[0] - current[0],
                                            target[1] - current[1] };
            const auto magnitudeSquared = MagnitudeSquared(difference);

            if (Math::IsApproximatelyZero(magnitudeSquared))
                return target;

            const auto distance = Math::SquareRoot(magnitudeSquared);
            if (distance <= maxDistanceDelta)
                return target;

            const auto factor = maxDistanceDelta / distance;

            return Vec2D<decltype(current[0] + difference[0] * factor)>{
                current[0] + difference[0] * factor,
                current[1] + difference[1] * factor
            };
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto RotateTowards(const Vec2D<Tx>& current,
                                             const Vec2D<Ty>& target,
                                             Tz maxDeltaAngle,
                                             AngleType angleType = AngleType::Radians)
        {
            if (AreApproximatelyEqual(current, target))
                return target;

            if (angleType == AngleType::Degrees)
                maxDeltaAngle = Math::DegToRad(maxDeltaAngle);

            const auto currentAngle = Math::Atan2(current.y, current.x);
            const auto targetAngle  = Math::Atan2(target.y, target.x);
            const auto deltaAngle   = Math::Clamp(Math::NormaliseAngle(targetAngle - currentAngle),
                                                  -maxDeltaAngle,
                                                  maxDeltaAngle);

            const auto rotatedAngle = currentAngle + deltaAngle;
            const auto length       = Magnitude(current);

            return Vec2D<decltype(length * Math::Cos(rotatedAngle))>{
                length * Math::Cos(rotatedAngle),
                length * Math::Sin(rotatedAngle)
            };
        }
    };

    class Vector3DUtils final
    {
        template<AnyNumber TNumber>
        using Vec3D = Vector<3, TNumber>;

    public:
        template<AnyNumber TNumber>
        OTR_INLINE static bool IsApproximatelyZero(const Vec3D<TNumber>& vector)
        {
            if constexpr (IntegerNumber<TNumber>)
                return vector[0] == 0 && vector[1] == 0 && vector[2] == 0;

            return Math::IsApproximatelyZero(vector[0])
                   && Math::IsApproximatelyZero(vector[1])
                   && Math::IsApproximatelyZero(vector[2]);
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static bool AreApproximatelyEqual(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs)
        {
            if constexpr (IntegerNumber<Tx> && IntegerNumber<Ty>)
                return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2];

            return Math::AreApproximatelyEqual(lhs[0], rhs[0])
                   && Math::AreApproximatelyEqual(lhs[1], rhs[1])
                   && Math::AreApproximatelyEqual(lhs[2], rhs[2]);
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto MagnitudeSquared(const Vec3D<TNumber>& vector)
        {
            return Math::Square(vector[0]) + Math::Square(vector[1]) + Math::Square(vector[2]);
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Magnitude(const Vec3D<TNumber>& vector)
        {
            return Math::SquareRoot(MagnitudeSquared(vector));
        }

        template<AnyNumber TNumber, AnyNumber TMaxMagnitude>
        OTR_INLINE static auto ClampMagnitude(const Vec3D<TNumber>& vector, const TMaxMagnitude& maxMagnitude)
        {
            const auto magnitudeSquared = MagnitudeSquared(vector);
            if (magnitudeSquared > Math::Square(maxMagnitude))
                return vector * Math::Min(maxMagnitude / Math::SquareRoot(magnitudeSquared), 1);

            return vector;
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Normalise(const Vec3D<TNumber>& vector)
        {
            const auto magnitude = Magnitude(vector);
            if (magnitude < Epsilon<Double128>)
                return Vec3D<TNumber>::Zero();

            return vector / magnitude;
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Dot(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs)
        {
            return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Cross(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs)
        {
            return Vec3D<decltype(lhs[0] * rhs[0] - lhs[0] * rhs[0] + lhs[0] * rhs[0])>{
                lhs[1] * rhs[2] - lhs[2] * rhs[1],
                lhs[2] * rhs[0] - lhs[0] * rhs[2],
                lhs[0] * rhs[1] - lhs[1] * rhs[0]
            };
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto DistanceSquared(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs)
        {
            return MagnitudeSquared(lhs - rhs);
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Distance(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs)
        {
            return Magnitude(lhs - rhs);
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto Lerp(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs, Tz t)
        {
            return lhs + (rhs - lhs) * t;
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto LerpClamped(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs, Tz t)
        {
            return Lerp(lhs, rhs, Math::Clamp(t, (Tz) 0.0, (Tz) 1.0));
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto Slerp(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs, Tz t)
        {
            auto dotClamped = Math::Clamp(Dot(lhs, rhs), -1.0, 1.0);
            auto theta      = Math::Acos(dotClamped) * t;
            auto relative   = Normalise(rhs - lhs * dotClamped);

            return ((lhs * Math::Cos(theta)) + (relative * Math::Sin(theta)));
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto SlerpClamped(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs, Tz t)
        {
            return Slerp(lhs, rhs, Math::Clamp(t, (Tz) 0.0, (Tz) 1.0));
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Max(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs)
        {
            return Vec3D<decltype(Math::Max(lhs[0], rhs[0]))>(Math::Max(lhs[0], rhs[0]),
                                                              Math::Max(lhs[1], rhs[1]),
                                                              Math::Max(lhs[2], rhs[2]));
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Min(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs)
        {
            return Vec3D<decltype(Math::Min(lhs[0], rhs[0]))>(Math::Min(lhs[0], rhs[0]),
                                                              Math::Min(lhs[1], rhs[1]),
                                                              Math::Min(lhs[2], rhs[2]));
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto Clamp(const Vec3D<Tx>& value, const Vec3D<Ty>& min, const Vec3D<Tz>& max)
        {
            return Vec3D<decltype(Math::Clamp(value[0], min[0], max[0]))>(Math::Clamp(value[0], min[0], max[0]),
                                                                          Math::Clamp(value[1], min[1], max[1]),
                                                                          Math::Clamp(value[2], min[2], max[2]));
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Reflect(const Vec3D<Tx>& vector, const Vec3D<Ty>& normal)
        {
            return vector - 2 * Dot(vector, normal) * normal;
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Project(const Vec3D<Tx>& vector, const Vec3D<Ty>& normal)
        {
            const auto magnitudeSquared = MagnitudeSquared(normal);
            if (magnitudeSquared < Epsilon<Double128>)
                return Vec3D<decltype(vector[0] * normal[0])>::Zero();

            return (Dot(vector, normal) / magnitudeSquared) * normal;
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Angle(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs)
        {
            const auto magnitudeProduct = Magnitude(lhs) * Magnitude(rhs);
            if (magnitudeProduct < Epsilon<Double128>)
                return 0;

            return Math::Acos(Dot(lhs, rhs) / magnitudeProduct);
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto AngleSigned(const Vec3D<Tx>& from, const Vec3D<Ty>& to, const Vec3D<Ty>& axis)
        {
            return Math::Sign(Dot(axis, Cross(from, to))) * Angle(from, to);
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto SmoothStep(const Vec3D<Tx>& min, const Vec3D<Ty>& max, const Vec3D<Tz>& value)
        {
            return Vec3D<decltype(Math::SmoothStep(min[0], max[0], value[0]))>{
                Math::SmoothStep(min[0], max[0], value[0]),
                Math::SmoothStep(min[1], max[1], value[1]),
                Math::SmoothStep(min[2], max[2], value[2])
            };
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static constexpr auto InverseSmoothStep(const Vec3D<Tx>& min,
                                                           const Vec3D<Ty>& max,
                                                           const Vec3D<Tz>& smoothenedValue)
        {
            return Vec3D<decltype(Math::InverseSmoothStep(min, max, smoothenedValue[0]))>{
                Math::InverseSmoothStep(min[0], max[0], smoothenedValue[0]),
                Math::InverseSmoothStep(min[1], max[1], smoothenedValue[1]),
                Math::InverseSmoothStep(min[2], max[2], smoothenedValue[2])
            };
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto MoveTowards(const Vec3D<Tx>& current, const Vec3D<Ty>& target, Tz maxDistanceDelta)
        {
            if (AreApproximatelyEqual(current, target))
                return target;

            const auto difference       = { target[0] - current[0],
                                            target[1] - current[1],
                                            target[2] - current[2] };
            const auto magnitudeSquared = MagnitudeSquared(difference);

            if (Math::IsApproximatelyZero(magnitudeSquared))
                return target;

            const auto distance = Math::SquareRoot(magnitudeSquared);
            if (distance <= maxDistanceDelta)
                return target;

            const auto factor = maxDistanceDelta / distance;

            return Vec3D<decltype(current[0] + difference[0] * factor)>{
                current[0] + difference[0] * factor,
                current[1] + difference[1] * factor,
                current[2] + difference[2] * factor
            };
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto RotateTowards(const Vec3D<Tx>& current,
                                             const Vec3D<Ty>& target,
                                             Tz maxDeltaAngle,
                                             AngleType angleType = AngleType::Radians)
        {
            if (AreApproximatelyEqual(current, target))
                return target;

            if (angleType == AngleType::Degrees)
                maxDeltaAngle = Math::DegToRad(maxDeltaAngle);

            const auto axis  = Cross(current, target);
            const auto angle = Math::Acos(Math::Clamp(Dot(current, target), -1.0, 1.0));
            maxDeltaAngle = Math::Clamp(maxDeltaAngle, -angle, maxDeltaAngle);

            const auto sinHalfAngle = std::sin(maxDeltaAngle / 2.0);
            const auto cosHalfAngle = std::cos(maxDeltaAngle / 2.0);

            return Vec3D<decltype(current[0] * cosHalfAngle + axis[0] * sinHalfAngle)>{
                cosHalfAngle * current[0] + sinHalfAngle * axis[0],
                cosHalfAngle * current[1] + sinHalfAngle * axis[1],
                cosHalfAngle * current[2] + sinHalfAngle * axis[2]
            };
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto ProjectOnPlane(const Vec3D<Tx>& vector, const Vec3D<Ty>& planeNormal)
        {
            if (IsApproximatelyZero(vector) || IsApproximatelyZero(planeNormal))
                return vector;

            if (AreApproximatelyEqual(vector, planeNormal))
                return Vec3D<decltype(vector[0] - planeNormal[0])>::Zero();

            planeNormal = Normalise(planeNormal);
            const auto dot = Dot(vector, planeNormal);

            return Vec3D<decltype(vector[0] - dot * planeNormal[0])>{
                vector[0] - dot * planeNormal[0],
                vector[1] - dot * planeNormal[1],
                vector[2] - dot * planeNormal[2]
            };
        }

        template<FloatingPointNumber Tx, FloatingPointNumber Ty, FloatingPointNumber Tz>
        OTR_INLINE static void OrthoNormalise(Vec3D<Tx>* normal, Vec3D<Ty>* tangent, Vec3D<Tz>* biNormal)
        {
            if (!normal || normal == Vec3D<Tx>::Zero()
                || !tangent || tangent == Vec3D<Ty>::Zero())
            {
                *normal   = Vec3D<Tx>::Zero();
                *tangent  = Vec3D<Ty>::Zero();
                *biNormal = Vec3D<Tz>::Zero();

                return;
            }

            auto normalDeref   = *normal;
            auto tangentDeref  = *tangent;
            auto biNormalDeref = biNormal ? *biNormal : Vec3D<Tz>::Zero();

            // Step 1: Normal
            normalDeref  = Normalise(normalDeref);

            if (!biNormal || biNormal == Vec3D<Ty>::Zero())
            {
                // Step 2: BiNormal
                biNormalDeref = Cross(normalDeref, tangentDeref);
                // Step 3: Tangent
                tangentDeref  = Cross(biNormalDeref, normalDeref);

                normal   = &normalDeref;
                tangent  = &tangentDeref;
                biNormal = &biNormalDeref;

                return;
            }

            // Step 2: Tangent
            tangentDeref -= normalDeref * Dot(normalDeref, tangentDeref);
            tangentDeref = Normalise(tangentDeref);

            // Step 3: BiNormal
            const auto cross = Cross(normalDeref, tangentDeref);
            biNormalDeref -= normalDeref * Dot(normalDeref, biNormalDeref);
            biNormalDeref -= tangentDeref * Dot(tangentDeref, biNormalDeref);
            biNormalDeref -= cross * Dot(cross, biNormalDeref);
            biNormalDeref    = Normalise(biNormalDeref);

            normal   = &normalDeref;
            tangent  = &tangentDeref;
            biNormal = &biNormalDeref;
        }
    };

    class Vector4DUtils final
    {
        template<AnyNumber TNumber>
        using Vec4D = Vector<4, TNumber>;

    public:
        template<AnyNumber TNumber>
        OTR_INLINE static bool IsApproximatelyZero(const Vec4D<TNumber>& vector)
        {
            if constexpr (IntegerNumber<TNumber>)
                return vector[0] == 0 && vector[1] == 0 && vector[2] == 0 && vector[3] == 0;

            return Math::IsApproximatelyZero(vector[0])
                   && Math::IsApproximatelyZero(vector[1])
                   && Math::IsApproximatelyZero(vector[2])
                   && Math::IsApproximatelyZero(vector[3]);
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static bool AreApproximatelyEqual(const Vec4D<Tx>& lhs, const Vec4D<Ty>& rhs)
        {
            if constexpr (IntegerNumber<Tx> && IntegerNumber<Ty>)
                return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2] && lhs[3] == rhs[3];

            return Math::AreApproximatelyEqual(lhs[0], rhs[0])
                   && Math::AreApproximatelyEqual(lhs[1], rhs[1])
                   && Math::AreApproximatelyEqual(lhs[2], rhs[2])
                   && Math::AreApproximatelyEqual(lhs[3], rhs[3]);
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto MagnitudeSquared(const Vec4D<TNumber>& vector)
        {
            return Math::Square(vector[0]) + Math::Square(vector[1]) + Math::Square(vector[2]);
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Magnitude(const Vec4D<TNumber>& vector)
        {
            return Math::SquareRoot(MagnitudeSquared(vector));
        }

        template<AnyNumber TNumber, AnyNumber TMaxMagnitude>
        OTR_INLINE static auto ClampMagnitude(const Vec4D<TNumber>& vector, const TMaxMagnitude& maxMagnitude)
        {
            const auto magnitudeSquared = MagnitudeSquared(vector);
            if (magnitudeSquared > Math::Square(maxMagnitude))
                return vector * Math::Min(maxMagnitude / Math::SquareRoot(magnitudeSquared), 1);

            return vector;
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Normalise(const Vec4D<TNumber>& vector)
        {
            const auto magnitude = Magnitude(vector);
            if (magnitude < Epsilon<Double128>)
                return Vec4D<TNumber>::Zero();

            return vector / magnitude;
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Dot(const Vec4D<Tx>& lhs, const Vec4D<Ty>& rhs)
        {
            return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3];
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Cross(const Vec4D<Tx>& lhs, const Vec4D<Ty>& rhs)
        {
            return Vec4D<decltype(lhs[0] * rhs[0] - lhs[0] * rhs[0] + lhs[0] * rhs[0])>{
                lhs[1] * rhs[2] - lhs[2] * rhs[1],
                lhs[2] * rhs[0] - lhs[0] * rhs[2],
                lhs[0] * rhs[1] - lhs[1] * rhs[0]
            };
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto DistanceSquared(const Vec4D<Tx>& lhs, const Vec4D<Ty>& rhs)
        {
            return MagnitudeSquared(lhs - rhs);
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Distance(const Vec4D<Tx>& lhs, const Vec4D<Ty>& rhs)
        {
            return Magnitude(lhs - rhs);
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto Lerp(const Vec4D<Tx>& lhs, const Vec4D<Ty>& rhs, Tz t)
        {
            return lhs + (rhs - lhs) * t;
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto LerpClamped(const Vec4D<Tx>& lhs, const Vec4D<Ty>& rhs, Tz t)
        {
            return Lerp(lhs, rhs, Math::Clamp(t, (Tz) 0.0, (Tz) 1.0));
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Max(const Vec4D<Tx>& lhs, const Vec4D<Ty>& rhs)
        {
            return Vec4D<decltype(Math::Max(lhs[0], rhs[0]))>(Math::Max(lhs[0], rhs[0]),
                                                              Math::Max(lhs[1], rhs[1]),
                                                              Math::Max(lhs[2], rhs[2]),
                                                              Math::Max(lhs[3], rhs[3]));
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Min(const Vec4D<Tx>& lhs, const Vec4D<Ty>& rhs)
        {
            return Vec4D<decltype(Math::Min(lhs[0], rhs[0]))>(Math::Min(lhs[0], rhs[0]),
                                                              Math::Min(lhs[1], rhs[1]),
                                                              Math::Min(lhs[2], rhs[2]),
                                                              Math::Min(lhs[3], rhs[3]));
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Project(const Vec4D<Tx>& vector, const Vec4D<Ty>& normal)
        {
            const auto magnitudeSquared = MagnitudeSquared(normal);
            if (magnitudeSquared < Epsilon<Double128>)
                return Vec4D<decltype(vector[0] * normal[0])>::Zero();

            return (Dot(vector, normal) / magnitudeSquared) * normal;
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto MoveTowards(const Vec4D<Tx>& current, const Vec4D<Ty>& target, Tz maxDistanceDelta)
        {
            if (AreApproximatelyEqual(current, target))
                return target;

            const auto difference       = { target[0] - current[0],
                                            target[1] - current[1],
                                            target[2] - current[2],
                                            target[3] - current[3] };
            const auto magnitudeSquared = MagnitudeSquared(difference);

            if (Math::IsApproximatelyZero(magnitudeSquared))
                return target;

            const auto distance = Math::SquareRoot(magnitudeSquared);
            if (distance <= maxDistanceDelta)
                return target;

            const auto factor = maxDistanceDelta / distance;

            return Vector<3, decltype(current[0] + difference[0] * factor)>{
                current[0] + difference[0] * factor,
                current[1] + difference[1] * factor,
                current[2] + difference[2] * factor,
                current[3] + difference[3] * factor
            };
        }
    };
}

#endif //OTTERENGINE_VECTOR_H
