#ifndef OTTERENGINE_VECTOR_H
#define OTTERENGINE_VECTOR_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Math/Math.Concepts.h"
#include "Math/Core.h"

namespace Otter
{
    template<UInt8 TDimension, AnyNumber TNumber> requires Dimension<TDimension>
    struct Vector final
    {
    public:
        Vector()
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = 0;
        }

        explicit Vector(TNumber scalar)
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = scalar;
        }

        template<typename... TArgs>
        requires (sizeof...(TArgs) == TDimension - 1 && (AnyNumber<TArgs>&& ...))
        explicit Vector(TNumber x, TArgs... args)
        {
            m_Values[0] = x;

            UInt8 i = 1;
            ([&]
            {
                m_Values[i++] = args;
            }(), ...);
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

        template<AnyNumber T>
        Vector<TDimension, TNumber>& operator+=(const Vector<TDimension, T>& other)
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

        template<AnyNumber T>
        Vector<TDimension, TNumber>& operator-=(const Vector<TDimension, T>& other)
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

        template<AnyNumber T>
        Vector<TDimension, TNumber> operator+(const Vector<TDimension, T>& other) const
        {
            return Vector<TDimension, TNumber>(*this) += other;
        }

        template<AnyNumber T>
        Vector<TDimension, TNumber> operator-(const Vector<TDimension, T>& other) const
        {
            return Vector<TDimension, TNumber>(*this) -= other;
        }

        template<AnyNumber T>
        Vector<TDimension, TNumber> operator*(const T& scalar) const
        {
            auto result = Vector<TDimension, TNumber>(*this);

            for (UInt8 i = 0; i < TDimension; ++i)
                this->m_Values[i] *= scalar;

            return result;
        }

        template<AnyNumber T>
        Vector<TDimension, TNumber> operator/(const T& scalar) const
        {
            auto result = Vector<TDimension, TNumber>(*this);

            for (UInt8 i = 0; i < TDimension; ++i)
                result.m_Values[i] /= scalar;

            return result;
        }

        template<AnyNumber T>
        Vector<TDimension, TNumber>& operator*=(const Vector<TDimension, T>& other)
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] *= static_cast<TNumber>(other.m_Values[i]);

            return *this;
        }

        template<AnyNumber T>
        Vector<TDimension, TNumber>& operator*=(const T& scalar) noexcept
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] *= scalar;

            return *this;
        }

        template<AnyNumber T>
        Vector<TDimension, TNumber>& operator/=(const Vector<TDimension, T>& other)
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] /= static_cast<TNumber>(other.m_Values[i]);

            return *this;
        }

        template<AnyNumber T>
        Vector<TDimension, TNumber>& operator/=(const T& scalar) noexcept
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] /= scalar;

            return *this;
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
    private:
        TNumber m_Values[TDimension];
    };

    namespace Math
    {
        template<UInt8 TDimension>
        requires Dimension<TDimension> && (TDimension < 4)
        Vector<TDimension, Int32> Left();

        template<UInt8 TDimension>
        requires Dimension<TDimension> && (TDimension < 4)
        Vector<TDimension, Int32> Right();

        template<UInt8 TDimension>
        requires Dimension<TDimension> && (TDimension < 4)
        Vector<TDimension, Int32> Down();

        template<UInt8 TDimension>
        requires Dimension<TDimension> && (TDimension < 4)
        Vector<TDimension, Int32> Up();

        template<UInt8 TDimension>
        requires Dimension<TDimension> && (TDimension == 3)
        Vector<TDimension, Int32> Back();

        template<UInt8 TDimension>
        requires Dimension<TDimension> && (TDimension == 3)
        Vector<TDimension, Int32> Forward();

        template<UInt8 TDimension>
        requires Dimension<TDimension>
        Vector<TDimension, Int32> VectorOne();

        template<UInt8 TDimension>
        requires Dimension<TDimension>
        Vector<TDimension, Int32> VectorZero();

        template<UInt8 TDimension, AnyNumber TNumber>
        requires Dimension<TDimension>
        Vector<TDimension, TNumber> VectorPositiveInfinity();

        template<UInt8 TDimension, AnyNumber TNumber>
        requires Dimension<TDimension>
        Vector<TDimension, TNumber> VectorNegativeInfinity();
    }
}

#include "Math/Math.Vector2.h"
#include "Math/Math.Vector3.h"
#include "Math/Math.Vector4.h"

#endif //OTTERENGINE_VECTOR_H
