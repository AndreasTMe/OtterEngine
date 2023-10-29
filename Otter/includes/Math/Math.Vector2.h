#ifndef OTTERENGINE_MATH_VECTOR2_H
#define OTTERENGINE_MATH_VECTOR2_H

#include "Math/Core.h"
#include "Math/Vector.h"

namespace Otter
{
    template<AnyNumber TNumber>
    struct Vector<2, TNumber> final
    {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

        union
        {
            struct
            {
                TNumber X, Y;
            };

            TNumber Values[2];
        };

#pragma clang diagnostic pop

        Vector()
            : X(0), Y(0)
        {
        }

        explicit Vector(TNumber scalar)
            : X(scalar), Y(scalar)
        {
        }

        template<typename... TArgs>
        requires (sizeof...(TArgs) == 1 && (AnyNumber<TArgs>&& ...))
        explicit Vector(TNumber x, TArgs... args)
            : X(x)
        {
            TNumber values[] = { args... };
            Y = values[0];
        }

        Vector(const Vector<2, TNumber>& other)
            : X(other.X), Y(other.Y)
        {
        }

        Vector(Vector<2, TNumber>&& other) noexcept
            : X(other.X), Y(other.Y)
        {
        }

        Vector<2, TNumber>& operator=(const Vector<2, TNumber>& other)
        {
            X = other.X;
            Y = other.Y;

            return *this;
        }

        Vector<2, TNumber>& operator=(Vector<2, TNumber>&& other) noexcept
        {
            X = other.X;
            Y = other.Y;

            return *this;
        }

        TNumber& operator[](UInt8 index)
        {
            OTR_ASSERT_MSG(index < 2, "Index {0} is out of range", index)

            return Values[index];
        }

        template<AnyNumber T>
        Vector<2, TNumber>& operator+=(const Vector<2, T>& other)
        {
            X += static_cast<TNumber>(other.X);
            Y += static_cast<TNumber>(other.Y);

            return *this;
        }

        Vector<2, TNumber>& operator+=(const Vector<2, TNumber>& other) noexcept
        {
            X += other.X;
            Y += other.Y;

            return *this;
        }

        template<AnyNumber T>
        Vector<2, TNumber>& operator-=(const Vector<2, T>& other)
        {
            X -= static_cast<TNumber>(other.X);
            Y -= static_cast<TNumber>(other.Y);

            return *this;
        }

        Vector<2, TNumber>& operator-=(const Vector<2, TNumber>& other) noexcept
        {
            X -= other.X;
            Y -= other.Y;

            return *this;
        }

        template<AnyNumber T>
        Vector<2, TNumber> operator+(const Vector<2, T>& other) const
        {
            return Vector<2, TNumber>(*this) += other;
        }

        template<AnyNumber T>
        Vector<2, TNumber> operator-(const Vector<2, T>& other) const
        {
            return Vector<2, TNumber>(*this) -= other;
        }

        template<AnyNumber T>
        Vector<2, TNumber>& operator*=(const Vector<2, T>& other)
        {
            X *= static_cast<TNumber>(other.X);
            Y *= static_cast<TNumber>(other.Y);

            return *this;
        }

        template<AnyNumber T>
        Vector<2, TNumber>& operator*=(const T& scalar) noexcept
        {
            X *= scalar;
            Y *= scalar;

            return *this;
        }

        template<AnyNumber T>
        Vector<2, TNumber>& operator/=(const Vector<2, T>& other)
        {
            X /= static_cast<TNumber>(other.X);
            Y /= static_cast<TNumber>(other.Y);

            return *this;
        }

        template<AnyNumber T>
        Vector<2, TNumber>& operator/=(const T& scalar) noexcept
        {
            X /= scalar;
            Y /= scalar;

            return *this;
        }

        template<AnyNumber TOtherNumber>
        bool operator==(const Vector<2, TOtherNumber>& other) const noexcept
        {
            if constexpr (IntegerNumber<TNumber> && IntegerNumber<TOtherNumber>)
                return X == other.X && Y == other.Y;

            return Math::AreApproximatelyEqual(X, other.X) && Math::AreApproximatelyEqual(Y, other.Y);
        }

        template<AnyNumber TOtherNumber>
        bool operator!=(const Vector<2, TNumber>& other) const noexcept
        {
            if constexpr (IntegerNumber<TNumber> && IntegerNumber<TOtherNumber>)
                return X != other.X || Y != other.Y;

            return !Math::AreApproximatelyEqual(X, other.X) || !Math::AreApproximatelyEqual(Y, other.Y);
        }
    };

    namespace Math
    {
        template<>
        OTR_INLINE Vector<2, Int32> Left() { return Vector<2, Int32>(-1, 0); }

        template<>
        OTR_INLINE Vector<2, Int32> Right() { return Vector<2, Int32>(1, 0); }

        template<>
        OTR_INLINE Vector<2, Int32> Down() { return Vector<2, Int32>(0, -1); }

        template<>
        OTR_INLINE Vector<2, Int32> Up() { return Vector<2, Int32>(0, 1); }

        template<>
        OTR_INLINE Vector<2, Int32> One() { return Vector<2, Int32>(1, 1); }

        template<>
        OTR_INLINE Vector<2, Int32> Zero() { return Vector<2, Int32>(0, 0); }

        template<AnyNumber TNumber>
        OTR_INLINE Vector<2, TNumber> VectorPositiveInfinity()
        {
            return Vector<2, TNumber>(PositiveInfinity<TNumber>(), PositiveInfinity<TNumber>());
        }

        template<AnyNumber TNumber>
        OTR_INLINE Vector<2, TNumber> VectorNegativeInfinity()
        {
            return Vector<2, TNumber>(NegativeInfinity<TNumber>(), NegativeInfinity<TNumber>());
        }

        template<AnyNumber TNumber>
        OTR_INLINE bool IsApproximatelyZero(const Vector<2, TNumber>& vector)
        {
            if constexpr (IntegerNumber<TNumber>)
                return vector.X == 0 && vector.Y == 0;

            return Math::IsApproximatelyZero(vector.X) && Math::IsApproximatelyZero(vector.Y);
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE bool AreApproximatelyEqual(const Vector<2, Tx>& lhs, const Vector<2, Ty>& rhs)
        {
            if constexpr (IntegerNumber<Tx> && IntegerNumber<Ty>)
                return lhs.X == rhs.X && lhs.Y == rhs.Y;

            return Math::AreApproximatelyEqual(lhs.X, rhs.X) && Math::AreApproximatelyEqual(lhs.Y, rhs.Y);
        }

        template<AnyNumber TNumber>
        OTR_INLINE auto MagnitudeSquared(const Vector<2, TNumber>& vector)
        {
            return Square(vector.X) + Square(vector.Y);
        }

        template<AnyNumber TNumber>
        OTR_INLINE auto Magnitude(const Vector<2, TNumber>& vector) { return Sqrt(MagnitudeSquared(vector)); }

        template<AnyNumber TNumber>
        OTR_INLINE auto Normalise(const Vector<2, TNumber>& vector) { return vector / Magnitude(vector); }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE auto Dot(const Vector<2, Tx>& lhs, const Vector<2, Ty>& rhs)
        {
            return lhs.X * rhs.X + lhs.Y * rhs.Y;
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE auto Cross(const Vector<2, Tx>& lhs, const Vector<2, Ty>& rhs)
        {
            return Magnitude(lhs) * Magnitude(rhs) * Sin(Angle(lhs, rhs));
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

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE auto Max(const Vector<2, Tx>& lhs, const Vector<2, Ty>& rhs)
        {
            return Vector<2, decltype(Math::Max(lhs.X, rhs.X))>(Math::Max(lhs.X, rhs.X),
                                                                Math::Max(lhs.Y, rhs.Y));
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE auto Min(const Vector<2, Tx>& lhs, const Vector<2, Ty>& rhs)
        {
            return Vector<2, decltype(Math::Min(lhs.X, rhs.X))>(Math::Min(lhs.X, rhs.X),
                                                                Math::Min(lhs.Y, rhs.Y));
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE auto Clamp(const Vector<2, Tx>& value, const Vector<2, Ty>& min, const Vector<2, Tz>& max)
        {
            return Vector<2, decltype(Math::Clamp(value.X, min.X, max.X))>(Math::Clamp(value.X, min.X, max.X),
                                                                           Math::Clamp(value.Y, min.Y, max.Y));
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE auto Reflect(const Vector<2, Tx>& vector, const Vector<2, Ty>& normal)
        {
            return vector - 2 * Dot(vector, normal) * normal;
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE auto Project(const Vector<2, Tx>& vector, const Vector<2, Ty>& onNormal)
        {
            return Dot(vector, onNormal) / MagnitudeSquared(onNormal) * onNormal;
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE auto Angle(const Vector<2, Tx>& lhs, const Vector<2, Ty>& rhs)
        {
            return Acos(Dot(lhs, rhs) / (Magnitude(lhs) * Magnitude(rhs)));
        }

        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE auto AngleSigned(const Vector<2, Tx>& from, const Vector<2, Ty>& to)
        {
            return Sign(from.X * to.Y - from.Y * to.X) * Angle(from, to);
        }

        // TODO: Vector2::SmoothStep
        // TODO: Vector2::SmoothDamp
        // TODO: Vector2::SmoothDampAngle
        // TODO: Vector2::MoveTowards
        // TODO: Vector2::RotateTowards
    }
}

#endif //OTTERENGINE_MATH_VECTOR2_H
