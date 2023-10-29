#ifndef OTTERENGINE_MATH_VECTOR3_H
#define OTTERENGINE_MATH_VECTOR3_H

#include "Math/Vector.h"

namespace Otter
{
    template<AnyNumber TNumber>
    struct Vector<3, TNumber> final
    {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

        union
        {
            struct
            {
                TNumber X, Y, Z;
            };

            TNumber Values[3];
        };

#pragma clang diagnostic pop

        Vector()
            : X(0), Y(0), Z(0)
        {
        }

        explicit Vector(TNumber scalar)
            : X(scalar), Y(scalar), Z(scalar)
        {
        }

        template<typename... TArgs>
        requires (sizeof...(TArgs) == 2 && (AnyNumber<TArgs>&& ...))
        explicit Vector(TNumber x, TArgs... args)
            : X(x)
        {
            UInt8 i = 1;
            ([&]
            {
                Values[++i] = args;
            }(), ...);
        }

        Vector(const Vector<3, TNumber>& other)
            : X(other.X), Y(other.Y), Z(other.Z)
        {
        }

        Vector(Vector<3, TNumber>&& other) noexcept
            : X(other.X), Y(other.Y), Z(other.Z)
        {
        }

        Vector<3, TNumber>& operator=(const Vector<3, TNumber>& other)
        {
            for (UInt8 i = 0; i < 3; ++i)
                Values[i] = other.Values[i];

            return *this;
        }

        Vector<3, TNumber>& operator=(Vector<3, TNumber>&& other) noexcept
        {
            for (UInt8 i = 0; i < 3; ++i)
                Values[i] = other.Values[i];

            return *this;
        }

        TNumber& operator[](UInt8 index)
        {
            OTR_ASSERT_MSG(index < 3, "Index {0} is out of range", index)

            return Values[index];
        }

        template<AnyNumber T>
        Vector<3, TNumber>& operator+=(const Vector<3, T>& other)
        {
            for (UInt8 i = 0; i < 3; ++i)
                Values[i] += static_cast<TNumber>(other.Values[i]);

            return *this;
        }

        Vector<3, TNumber>& operator+=(const Vector<3, TNumber>& other) noexcept
        {
            for (UInt8 i = 0; i < 3; ++i)
                Values[i] += other.Values[i];

            return *this;
        }

        template<AnyNumber T>
        Vector<3, TNumber>& operator-=(const Vector<3, T>& other)
        {
            for (UInt8 i = 0; i < 3; ++i)
                Values[i] -= static_cast<TNumber>(other.Values[i]);

            return *this;
        }

        Vector<3, TNumber>& operator-=(const Vector<3, TNumber>& other) noexcept
        {
            for (UInt8 i = 0; i < 3; ++i)
                Values[i] -= other.Values[i];

            return *this;
        }

        template<AnyNumber T>
        Vector<3, TNumber> operator+(const Vector<3, T>& other) const
        {
            return Vector<3, TNumber>(*this) += other;
        }

        template<AnyNumber T>
        Vector<3, TNumber> operator-(const Vector<3, T>& other) const
        {
            return Vector<3, TNumber>(*this) -= other;
        }

        template<AnyNumber T>
        Vector<3, TNumber>& operator*=(const Vector<3, T>& other)
        {
            for (UInt8 i = 0; i < 3; ++i)
                Values[i] *= static_cast<TNumber>(other.Values[i]);

            return *this;
        }

        template<AnyNumber T>
        Vector<3, TNumber>& operator*=(const T& scalar) noexcept
        {
            for (UInt8 i = 0; i < 3; ++i)
                Values[i] *= scalar;

            return *this;
        }

        template<AnyNumber T>
        Vector<3, TNumber>& operator/=(const Vector<3, T>& other)
        {
            for (UInt8 i = 0; i < 3; ++i)
                Values[i] /= static_cast<TNumber>(other.Values[i]);

            return *this;
        }

        template<AnyNumber T>
        Vector<4, TNumber>& operator/=(const T& scalar) noexcept
        {
            for (UInt8 i = 0; i < 3; ++i)
                Values[i] /= scalar;

            return *this;
        }

        template<AnyNumber TOtherNumber>
        bool operator==(const Vector<3, TOtherNumber>& other) const noexcept
        {
            if constexpr (IntegerNumber<TNumber> && IntegerNumber<TOtherNumber>)
                return X == other.X && Y == other.Y && Z == other.Z;

            return Math::AreApproximatelyEqual(X, other.X)
                   && Math::AreApproximatelyEqual(Y, other.Y)
                   && Math::AreApproximatelyEqual(Z, other.Z);
        }

        template<AnyNumber TOtherNumber>
        bool operator!=(const Vector<3, TOtherNumber>& other) const noexcept
        {
            if constexpr (IntegerNumber<TNumber> && IntegerNumber<TOtherNumber>)
                return X != other.X || Y != other.Y || Z != other.Z;

            return !Math::AreApproximatelyEqual(X, other.X)
                   || !Math::AreApproximatelyEqual(Y, other.Y)
                   || !Math::AreApproximatelyEqual(Z, other.Z);
        }
    };

    namespace Math
    {
        // TODO: Implement Vector3 functions
    }
}

#endif //OTTERENGINE_MATH_VECTOR3_H
