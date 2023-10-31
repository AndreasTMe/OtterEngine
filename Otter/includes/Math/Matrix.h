#ifndef OTTERENGINE_MATRIX_H
#define OTTERENGINE_MATRIX_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Math/Math.Concepts.h"
#include "Math/Core.h"
#include "Math/Vector.h"

namespace Otter
{
    template<UInt8 Tx, UInt8 Ty, AnyNumber TNumber>
    requires Dimension<Tx> && Dimension<Ty> && (!UnsignedNumber<TNumber>)
    struct Matrix final
    {
        constexpr Matrix()
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] = 0;
        }

        constexpr explicit Matrix(TNumber scalar)
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] = scalar;
        }

        template<typename... TArgs>
        requires((sizeof...(TArgs) == Tx * Ty - 1 && (AnyNumber<TArgs> && ...)))
        constexpr explicit Matrix(TNumber x, TArgs... args)
        {
            m_Values[0] = x;

            UInt8 i = 1;
            ([&]
            {
                m_Values[i++] = args;
            }(), ...);
        }

        Matrix(const Matrix<Tx, Ty, TNumber>& other)
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] = other.m_Values[i];
        }

        Matrix(Matrix<Tx, Ty, TNumber>&& other) noexcept
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] = other.m_Values[i];
        }

        Matrix<Tx, Ty, TNumber>& operator=(const Matrix<Tx, Ty, TNumber>& other)
        {
            if (this == &other)
                return *this;

            for (UInt8 i = 0; i < 3; ++i)
                m_Values[i] = other.m_Values[i];

            return *this;
        }

        Matrix<Tx, Ty, TNumber>& operator=(Matrix<Tx, Ty, TNumber>&& other) noexcept
        {
            if (this == &other)
                return *this;

            for (UInt8 i = 0; i < 3; ++i)
                m_Values[i] = other.m_Values[i];

            return *this;
        }

        TNumber& operator[](UInt8 index)
        {
            OTR_ASSERT_MSG(index < Tx * Ty, "Index {0} is out of range", index)
            return m_Values[index];
        }

        TNumber& operator[](UInt8 x, UInt8 y)
        {
            OTR_ASSERT_MSG(x < Tx, "Row index {0} is out of range", x)
            OTR_ASSERT_MSG(y < Ty, "Column index {0} is out of range", y)
            return m_Values[y + x * Ty];
        }

        const TNumber& operator[](UInt8 index) const
        {
            OTR_ASSERT_MSG(index < Tx * Ty, "Index {0} is out of range", index)
            return m_Values[index];
        }

        const TNumber& operator[](UInt8 x, UInt8 y) const
        {
            OTR_ASSERT_MSG(x < Tx, "Row index {0} is out of range", x)
            OTR_ASSERT_MSG(y < Ty, "Column index {0} is out of range", y)
            return m_Values[y + x * Ty];
        }

        template<AnyNumber TOtherNumber>
        explicit operator Matrix<Tx, Ty, TOtherNumber>() const
        {
            Matrix<Tx, Ty, TOtherNumber> result;

            for (UInt8 i = 0; i < Tx * Ty; ++i)
                result[i] = static_cast<TOtherNumber>(m_Values[i]);

            return result;
        }

        template<AnyNumber TOtherNumber>
        Matrix<Tx, Ty, TNumber>& operator+=(const Matrix<Tx, Ty, TOtherNumber>& other)
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] += static_cast<TNumber>(other.m_Values[i]);

            return *this;
        }

        Matrix<Tx, Ty, TNumber>& operator+=(const Matrix<Tx, Ty, TNumber>& other) noexcept
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] += other.m_Values[i];

            return *this;
        }

        template<AnyNumber TOtherNumber>
        Matrix<Tx, Ty, TNumber>& operator-=(const Matrix<Tx, Ty, TOtherNumber>& other)
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] -= static_cast<TNumber>(other.m_Values[i]);

            return *this;
        }

        Matrix<Tx, Ty, TNumber>& operator-=(const Matrix<Tx, Ty, TNumber>& other) noexcept
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] -= other.m_Values[i];

            return *this;
        }

        template<AnyNumber TOtherNumber>
        Matrix<Tx, Ty, TNumber>& operator*=(const TOtherNumber& scalar) noexcept
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] *= scalar;

            return *this;
        }

        template<AnyNumber TOtherNumber>
        Matrix<Tx, Ty, TNumber>& operator/=(const TOtherNumber& scalar) noexcept
        {
            OTR_ASSERT_MSG(scalar != 0, "Division by zero")

            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] /= scalar;

            return *this;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator+(Matrix<Tx, Ty, TNumber>& lhs,
                                        const Matrix<Tx, Ty, TOtherNumber>& rhs)
        {
            lhs += rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator-(Matrix<Tx, Ty, TNumber>& lhs,
                                        const Matrix<Tx, Ty, TOtherNumber>& rhs)
        {
            lhs -= rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator*(Matrix<Tx, Ty, TNumber>& lhs, const TOtherNumber& rhs)
        {
            lhs *= rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator/(Matrix<Tx, Ty, TNumber>& lhs, const TOtherNumber& rhs)
        {
            OTR_ASSERT_MSG(rhs != 0, "Division by zero")

            lhs /= rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        bool operator==(const Matrix<Tx, Ty, TOtherNumber>& other) const noexcept
        {
            if constexpr (IntegerNumber<TNumber> && IntegerNumber<TOtherNumber>)
            {
                for (UInt8 i = 0; i < Tx * Ty; ++i)
                    if (m_Values[i] != other.m_Values[i])
                        return false;

                return true;
            }

            for (UInt8 i = 0; i < Tx * Ty; ++i)
                if (!Math::AreApproximatelyEqual(m_Values[i], other.m_Values[i]))
                    return false;

            return true;
        }

        template<AnyNumber TOtherNumber>
        bool operator!=(const Matrix<Tx, Ty, TOtherNumber>& other) const noexcept
        {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "Simplify"

            return !(*this == other);

#pragma clang diagnostic pop
        }

        [[nodiscard]] Vector<Ty, TNumber> GetRow(UInt8 index) const noexcept
        {
            OTR_ASSERT_MSG(index < Ty, "Row index {0} is out of range", index)

            auto row = Vector<Ty, TNumber>();

            for (UInt8 i = 0; i < Ty; ++i)
                row[i] = m_Values[i + index * Ty];

            return row;
        }

        void SetRow(const UInt8& index, const Vector<Ty, TNumber>& row) noexcept
        {
            OTR_ASSERT_MSG(index < Ty, "Row index {0} is out of range", index)

            for (UInt8 i = 0; i < Ty; ++i)
                m_Values[i + index * Ty] = row[i];
        }

        [[nodiscard]] Vector<Tx, TNumber> GetColumn(UInt8 index) const noexcept
        {
            OTR_ASSERT_MSG(index < Tx, "Column index {0} is out of range", index)

            auto column = Vector<Tx, TNumber>();

            for (UInt8 i = 0; i < Tx; ++i)
                column[i] = m_Values[index + i * Tx];

            return column;
        }

        void SetColumn(const UInt8& index, const Vector<Tx, TNumber>& column) noexcept
        {
            OTR_ASSERT_MSG(index < Tx, "Column index {0} is out of range", index)

            for (UInt8 i = 0; i < Tx; ++i)
                m_Values[index + i * Tx] = column[i];
        }

    private:
        TNumber m_Values[Tx * Ty];
    };

    namespace Math
    {
        template<UInt8 Tx, UInt8 Ty>
        OTR_INLINE constexpr Matrix<Tx, Ty, Int32> MatrixZero() { return Matrix<Tx, Ty, Int32>(0); }

        template<UInt8 Tx, UInt8 Ty>
        requires Dimension<Tx> && Dimension<Ty> && (Tx == Ty)
        constexpr Matrix<Tx, Ty, Int32> MatrixIdentity();

        template<UInt8 Tx, UInt8 Ty, AnyNumber TNumber>
        requires Dimension<Tx> && Dimension<Ty>
        TNumber Determinant(const Matrix<Tx, Ty, TNumber>& matrix);

        template<UInt8 Tx, UInt8 Ty, AnyNumber TNumber>
        requires Dimension<Tx> && Dimension<Ty>
        Matrix<Ty, Tx, TNumber> Transpose(const Matrix<Tx, Ty, TNumber>& matrix);

        template<UInt8 Tx, UInt8 Ty, AnyNumber TNumber>
        requires Dimension<Tx> && Dimension<Ty> && (Tx == Ty)
        Matrix<Tx, Ty, TNumber> Inverse(const Matrix<Tx, Ty, TNumber>& matrix);

        template<UInt8 Tx, UInt8 Ty, UInt8 Tz, AnyNumber TNumber, AnyNumber TOtherNumber>
        requires Dimension<Tx> && Dimension<Ty> && Dimension<Tz>
        OTR_INLINE decltype(auto) Multiply(const Matrix<Tx, Ty, TNumber>& m1, const Matrix<Ty, Tz, TOtherNumber>& m2)
        {
            Matrix<Tx, Tz, decltype(TNumber{ } * TOtherNumber{ })> result;

            for (int col = 0; col < Tz; col++)
                for (int row = 0; row < Tx; row++)
                    for (int inner = 0; inner < Ty; inner++)
                        result[row, col] += m1[row, inner] * m2[inner, col];

            return result;
        }
    }
}

#include "Math/Math.Matrix2x2.h"
#include "Math/Math.Matrix3x3.h"
#include "Math/Math.Matrix4x4.h"

#endif //OTTERENGINE_MATRIX_H
