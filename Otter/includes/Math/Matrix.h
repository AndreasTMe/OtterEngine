#ifndef OTTERENGINE_MATRIX_H
#define OTTERENGINE_MATRIX_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Math/Math.Concepts.h"
#include "Math/Core.h"
#include "Math/Vector.h"
#include "Math/Quaternion.h"

namespace Otter
{
    namespace Math
    {
        template<UInt8 Tx, UInt8 Ty, AnyNumber TNumber>
        requires Dimension<Tx> && Dimension<Ty> && (!UnsignedNumber<TNumber>)
        struct Matrix;

        class MatrixUtils;

        class Matrix2x2Utils;

        class Matrix3x3Utils;

        class Matrix4x4Utils;
    }

    template<UInt8 Tx, UInt8 Ty, AnyNumber TNumber>
        requires Dimension<Tx> && Dimension<Ty> && (!UnsignedNumber<TNumber>)
    using Matrix = Math::Matrix<Tx, Ty, TNumber>;
    using MatrixUtils = Math::MatrixUtils;

    template<AnyNumber TNumber> requires (!UnsignedNumber<TNumber>)
    using Matrix2x2 = Math::Matrix<2, 2, TNumber>;
    using Matrix2x2Utils = Math::Matrix2x2Utils;

    template<AnyNumber TNumber> requires (!UnsignedNumber<TNumber>)
    using Matrix3x3 = Math::Matrix<3, 3, TNumber>;
    using Matrix3x3Utils = Math::Matrix3x3Utils;

    template<AnyNumber TNumber> requires (!UnsignedNumber<TNumber>)
    using Matrix4x4 = Math::Matrix<4, 4, TNumber>;
    using Matrix4x4Utils = Math::Matrix4x4Utils;
}

namespace Otter::Math
{
    template<UInt8 Tx, UInt8 Ty, AnyNumber TNumber>
    requires Dimension<Tx> && Dimension<Ty> && (!UnsignedNumber<TNumber>)
    struct Matrix final
    {
        // HELP: I usually add the member variables at the bottom of the class,
        //      but it's the only member and the class is too large.
    private:
        TNumber m_Values[Tx * Ty];

    public:
        constexpr Matrix()
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] = static_cast<TNumber>(0.0);
        }

        constexpr explicit Matrix(TNumber scalar)
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] = scalar;
        }

        constexpr Matrix(InitialiserList<TNumber> list)
        {
            OTR_ASSERT_MSG(list.size() == Tx * Ty, "Initialiser list size does not match Matrix size")

            UInt64 i = 0;
            for (const TNumber& value: list)
                m_Values[i++] = value;
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

        template<UInt8 TOtherX, UInt8 TOtherY, AnyNumber TOtherNumber>
        requires Dimension<TOtherX> && Dimension<TOtherY>
        explicit operator Matrix<TOtherX, TOtherY, TOtherNumber>() const
        {
            Matrix<TOtherX, TOtherY, TOtherNumber> result;

            for (UInt8 j = 0; j < TOtherY; ++j)
                for (UInt8 i = 0; i < TOtherX; ++i)
                    if (i >= Tx || j >= Ty)
                        result[i, j] = static_cast<TOtherNumber>(0.0);
                    else
                        result[i, j] = static_cast<TOtherNumber>(m_Values[j + i * Ty]);

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
        Matrix<Tx, Ty, TNumber>& operator*=(TOtherNumber scalar) noexcept
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] *= scalar;

            return *this;
        }

        template<AnyNumber TOtherNumber>
        Matrix<Tx, Ty, TNumber>& operator/=(TOtherNumber scalar) noexcept
        {
            OTR_ASSERT_MSG(scalar != 0, "Division by zero")

            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] /= scalar;

            return *this;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator+(Matrix<Tx, Ty, TNumber>& lhs, const Matrix<Tx, Ty, TOtherNumber>& rhs)
        {
            lhs += rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator-(Matrix<Tx, Ty, TNumber>& lhs, const Matrix<Tx, Ty, TOtherNumber>& rhs)
        {
            lhs -= rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator*(Matrix<Tx, Ty, TNumber>& lhs, TOtherNumber rhs)
        {
            lhs *= rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator/(Matrix<Tx, Ty, TNumber>& lhs, TOtherNumber rhs)
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

        [[nodiscard]] OTR_INLINE Math::Vector<Ty, TNumber> GetRow(UInt8 index) const noexcept
        {
            OTR_ASSERT_MSG(index < Ty, "Row index {0} is out of range", index)

            auto row = Vector<Ty, TNumber>();

            for (UInt8 i = 0; i < Ty; ++i)
                row[i] = m_Values[i + index * Ty];

            return row;
        }

        OTR_INLINE void SetRow(UInt8 index, const Math::Vector<Ty, TNumber>& row) noexcept
        {
            OTR_ASSERT_MSG(index < Ty, "Row index {0} is out of range", index)

            for (UInt8 i = 0; i < Ty; ++i)
                m_Values[i + index * Ty] = row[i];
        }

        [[nodiscard]] OTR_INLINE Math::Vector<Tx, TNumber> GetColumn(UInt8 index) const noexcept
        {
            OTR_ASSERT_MSG(index < Tx, "Column index {0} is out of range", index)

            auto column = Vector<Tx, TNumber>();

            for (UInt8 i = 0; i < Tx; ++i)
                column[i] = m_Values[index + i * Tx];

            return column;
        }

        OTR_INLINE void SetColumn(UInt8 index, const Math::Vector<Tx, TNumber>& column) noexcept
        {
            OTR_ASSERT_MSG(index < Tx, "Column index {0} is out of range", index)

            for (UInt8 i = 0; i < Tx; ++i)
                m_Values[index + i * Tx] = column[i];
        }

        OTR_INLINE static constexpr Matrix<Tx, Ty, TNumber> Zero()
        {
            return Matrix<Tx, Ty, TNumber>(static_cast<TNumber>(0.0));
        }

        OTR_INLINE static constexpr Matrix<Tx, Ty, TNumber> Identity()
        requires Dimension<Tx>
                 && Dimension<Ty>
                 && (Tx == Ty)
        {
            Matrix<Tx, Ty, TNumber> result;

            for (UInt8 i = 0; i < Tx; ++i)
                result[i, i] = static_cast<TNumber>(1.0);

            return result;
        }
    };

    class MatrixUtils final
    {
    public:
        template<UInt8 Tx, UInt8 Ty, UInt8 Tz, AnyNumber TNumber, AnyNumber TOtherNumber>
        requires Dimension<Tx> && Dimension<Ty> && Dimension<Tz>
        OTR_INLINE static decltype(auto) Multiply(const Matrix<Tx, Ty, TNumber>& m1,
                                                  const Matrix<Ty, Tz, TOtherNumber>& m2)
        {
            Matrix<Tx, Tz, decltype(TNumber{ } * TOtherNumber{ })> result;

            for (int col = 0; col < Tz; col++)
                for (int row = 0; row < Tx; row++)
                    for (int inner = 0; inner < Ty; inner++)
                        result[row, col] += m1[row, inner] * m2[inner, col];

            return result;
        }
    };

    class Matrix2x2Utils final
    {
        template<AnyNumber TNumber> requires (!UnsignedNumber<TNumber>)
        using Mat2x2 = Matrix<2, 2, TNumber>;

    public:
        template<AnyNumber TNumber>
        OTR_INLINE static auto Determinant(const Mat2x2<TNumber>& matrix)
        {
            return matrix[0, 0] * matrix[1, 1] - matrix[0, 1] * matrix[1, 0];
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Transpose(const Mat2x2<TNumber>& matrix)
        {
            return Mat2x2<TNumber>{
                matrix[0], matrix[2],
                matrix[1], matrix[3]
            };
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Inverse(const Mat2x2<TNumber>& matrix)
        {
            const auto determinant = matrix[0] * matrix[3] - matrix[1] * matrix[2];
            if (determinant == 0)
                return Mat2x2<TNumber>::Zero();

            const auto detReversed = static_cast<TNumber>(1.0) / determinant;

            return Mat2x2<TNumber>{
                matrix[3] * detReversed, -matrix[1] * detReversed,
                -matrix[2] * detReversed, matrix[0] * detReversed
            };
        }
    };

    class Matrix3x3Utils final
    {
        template<AnyNumber TNumber> requires (!UnsignedNumber<TNumber>)
        using Mat3x3 = Matrix<3, 3, TNumber>;

    public:
        template<AnyNumber TNumber>
        OTR_INLINE static auto Determinant(const Mat3x3<TNumber>& matrix)
        {
            // To calculate the determinant of a 3x3 matrix, we need the minor of some elements.
            // The minor is the determinant of the sub-matrix formed by deleting one row and one column from
            // the original matrix.
            //
            //    | 0 1 2 |          | 4 5 |          | 3 5 |          | 3 4 |
            // det| 3 4 5 | = 0 * det| 7 8 | - 1 * det| 6 8 | + 2 * det| 6 7 |
            //    | 6 7 8 |
            //
            // After some simplification:

            return matrix[0] * (matrix[4] * matrix[8] - matrix[5] * matrix[7])
                   - matrix[1] * (matrix[3] * matrix[8] - matrix[5] * matrix[6])
                   + matrix[2] * (matrix[3] * matrix[7] - matrix[4] * matrix[6]);
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Transpose(const Mat3x3<TNumber>& matrix)
        {
            return Mat3x3<TNumber>{
                matrix[0], matrix[3], matrix[6],
                matrix[1], matrix[4], matrix[7],
                matrix[2], matrix[5], matrix[8]
            };
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Inverse(const Mat3x3<TNumber>& matrix)
        {
            // To calculate the inverse of a 3x3 matrix, we need the minor of all elements.
            // The minor is the determinant of the sub-matrix formed by deleting one row and one column from
            // the original matrix.
            //
            //                |         | 4 5 |            | 3 5 |            | 3 4 | |
            //                |  0 * det| 7 8 |   - 1 * det| 6 8 |   + 2 * det| 6 7 | |
            //                |                                                       |
            //    | 0 1 2 |   |         | 1 2 |            | 0 2 |            | 0 1 | |
            // inv| 3 4 5 | = | -3 * det| 7 8 |   + 4 * det| 6 8 |   - 5 * det| 6 7 | |
            //    | 6 7 8 |   |                                                       |
            //                |         | 1 2 |            | 0 2 |            | 0 1 | |
            //                | +6 * det| 4 5 |   - 7 * det| 3 5 |   + 8 * det| 3 4 | |

            const auto m0 = matrix[4] * matrix[8] - matrix[5] * matrix[7];
            const auto m1 = matrix[3] * matrix[8] - matrix[5] * matrix[6];
            const auto m2 = matrix[3] * matrix[7] - matrix[4] * matrix[6];

            const auto determinant = matrix[0] * m0 - matrix[1] * m1 + matrix[2] * m2;
            if (determinant == 0)
                return Mat3x3<TNumber>::Zero();

            const auto detReversed = static_cast<TNumber>(1.0) / determinant;

            const auto m3 = matrix[1] * matrix[8] - matrix[2] * matrix[7];
            const auto m4 = matrix[0] * matrix[8] - matrix[2] * matrix[6];
            const auto m5 = matrix[0] * matrix[7] - matrix[1] * matrix[6];

            const auto m6 = matrix[1] * matrix[5] - matrix[2] * matrix[4];
            const auto m7 = matrix[0] * matrix[5] - matrix[2] * matrix[3];
            const auto m8 = matrix[0] * matrix[4] - matrix[1] * matrix[3];

            return Mat3x3<TNumber>{
                m0 * detReversed, -m1 * detReversed, m2 * detReversed,
                -m3 * detReversed, m4 * detReversed, -m5 * detReversed,
                m6 * detReversed, -m7 * detReversed, m8 * detReversed
            };
        }
    };

    class Matrix4x4Utils final
    {
        template<AnyNumber TNumber> requires (!UnsignedNumber<TNumber>)
        using Mat4x4 = Matrix<4, 4, TNumber>;

    public:
        template<AnyNumber TNumber>
        OTR_INLINE static auto Determinant(const Mat4x4<TNumber>& matrix)
        {
            // To calculate the determinant of a 4x4 matrix, we need the minor of some elements.
            // The minor is the determinant of the sub-matrix formed by deleting one row and one column from
            // the original matrix.
            //
            //    |  0  1  2  3 |
            //    |  4  5  6  7 |          |  5  6  7 |          |  4  6  7 |          |  4  5  7 |          |  4  5  6 |
            // det|  8  9 10 11 | = 0 * det|  9 10 11 | - 1 * det|  8 10 11 | + 2 * det|  8  9 11 | - 3 * det|  8  9 10 |
            //    | 12 13 14 15 |          | 13 14 15 |          | 12 14 15 |          | 12 13 15 |          | 12 13 14 |
            //
            //             = 0 * det(5 * det(10 * 15 - 11 * 14) - 6 * det(9 * 15 - 11 * 13) + 7 * det(9 * 14 - 10 * 13))
            //               - 1 * det(4 * det(10 * 15 - 11 * 14) - 6 * det(8 * 15 - 11 * 12) + 7 * det(8 * 14 - 10 * 12))
            //               + 2 * det(4 * det( 9 * 15 - 11 * 13) - 5 * det(8 * 15 - 11 * 12) + 7 * det(8 * 13 -  9 * 12))
            //               - 3 * det(4 * det( 9 * 14 - 10 * 13) - 5 * det(8 * 14 - 10 * 12) + 6 * det(8 * 13 -  9 * 12))
            //
            // We could use an existing function to calculate the determinant of a 3x3 matrix, but some of the sub-matrices
            // are the same, so we can reuse the results to reduce the number of calculations.

            const auto d00 = matrix[10] * matrix[15] - matrix[11] * matrix[14];
            const auto d01 = matrix[9] * matrix[15] - matrix[11] * matrix[13];
            const auto d02 = matrix[9] * matrix[14] - matrix[10] * matrix[13];
            const auto d03 = matrix[8] * matrix[15] - matrix[11] * matrix[12];
            const auto d04 = matrix[8] * matrix[14] - matrix[10] * matrix[12];
            const auto d05 = matrix[8] * matrix[13] - matrix[9] * matrix[12];

            return matrix[0] * (matrix[5] * d00 - matrix[6] * d01 + matrix[7] * d02)
                   - matrix[1] * (matrix[4] * d00 - matrix[6] * d03 + matrix[7] * d04)
                   + matrix[2] * (matrix[4] * d01 - matrix[5] * d03 + matrix[7] * d05)
                   - matrix[3] * (matrix[4] * d02 - matrix[5] * d04 + matrix[6] * d05);
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Transpose(const Mat4x4<TNumber>& matrix)
        {
            return Mat4x4<TNumber>{
                matrix[0], matrix[4], matrix[8], matrix[12],
                matrix[1], matrix[5], matrix[9], matrix[13],
                matrix[2], matrix[6], matrix[10], matrix[14],
                matrix[3], matrix[7], matrix[11], matrix[15]
            };
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Inverse(const Mat4x4<TNumber>& matrix)
        {
            // Laplace Expansion Theorem: Let A be an n × n matrix. Let r = (r1, r2, ..., rk) be a list of k row
            // indices, where 1 <= k < n and 0 <= r1 < r2 < ... rk < n.
            // The determinant of A is:
            //
            // det(A) = ((−1)^|r|) * Sum((-1)^|c| * det(S(A; r, c)) * det(S'(A; r, c)))
            //
            // where:
            // - |r| = r1 + r2 + ... + rk, |c| = c1 + c2 + ... +ck, and the summation is over all k-tuples
            //   c = (c1, c2, ..., ck) for which 1 <= c1 < c2 < ... < ck < n.
            // - S(A; r, c) are the sub-matrices obtained by keeping only the rows and columns of A indexed by r and c.
            // - S'(A; r, c) are the sub-matrices obtained by removing the rows and columns of A indexed by r and c.
            //
            // In our case, we will use |r| = 0 + 1, c = (c0, c1) and the above becomes:
            // det(A) = ((-1)^|0 + 1|) * Sum((-1)^|c0 + c1| * det(S(A; (0, 1), (c0, c1))) * det(S'(A; (0, 1), (c0, c1))))
            //        = - Sum((-1)^|c0 + c1| * det(S(A; (0, 1), (c0, c1))) * det(S'(A; (0, 1), (c0, c1))))
            //        = + det(S(A; (0, 1), (0, 1))) * det(S'(A; (0, 1), (0, 1)))
            //          - det(S(A; (0, 1), (0, 2))) * det(S'(A; (0, 1), (0, 2)))
            //          + det(S(A; (0, 1), (0, 3))) * det(S'(A; (0, 1), (0, 3)))
            //          + det(S(A; (0, 1), (1, 2))) * det(S'(A; (0, 1), (1, 2)))
            //          - det(S(A; (0, 1), (1, 3))) * det(S'(A; (0, 1), (1, 3)))
            //          + det(S(A; (0, 1), (2, 3))) * det(S'(A; (0, 1), (2, 3)))
            //
            // The formula below gives us the inverse matrix:
            //
            // A^-1 = 1 / det(A) * adj(A)
            //
            // where:
            // - adj(A) = transpose(cofactor(A))
            // - The cofactor of an element aij is the determinant of the sub-matrix (minor) obtained by deleting
            //   the ith row and jth column.
            // - The transpose of a matrix is a matrix whose rows are the columns of the original matrix.
            //
            // As a reference:
            //     |  0  1  2  3 |
            //     |  4  5  6  7 |
            // A = |  8  9 10 11 |
            //     | 12 13 14 15 |

            const auto s0 = matrix[0] * matrix[5] - matrix[1] * matrix[4];
            const auto s1 = matrix[0] * matrix[6] - matrix[2] * matrix[4];
            const auto s2 = matrix[0] * matrix[7] - matrix[3] * matrix[4];
            const auto s3 = matrix[1] * matrix[6] - matrix[2] * matrix[5];
            const auto s4 = matrix[1] * matrix[7] - matrix[3] * matrix[5];
            const auto s5 = matrix[2] * matrix[7] - matrix[3] * matrix[6];

            const auto c5 = matrix[10] * matrix[15] - matrix[11] * matrix[14];
            const auto c4 = matrix[9] * matrix[15] - matrix[11] * matrix[13];
            const auto c3 = matrix[9] * matrix[14] - matrix[10] * matrix[13];
            const auto c2 = matrix[8] * matrix[15] - matrix[11] * matrix[12];
            const auto c1 = matrix[8] * matrix[14] - matrix[10] * matrix[12];
            const auto c0 = matrix[8] * matrix[13] - matrix[9] * matrix[12];

            const auto determinant = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
            if (determinant == 0)
                return Mat4x4<TNumber>::Zero();

            const auto detReversed = static_cast<TNumber>(1.0) / determinant;

            return Mat4x4<TNumber>{
                detReversed * (matrix[5] * c5 - matrix[6] * c4 + matrix[7] * c3),
                detReversed * -(matrix[1] * c5 - matrix[2] * c4 + matrix[3] * c3),
                detReversed * (matrix[1] * s5 - matrix[2] * s4 + matrix[3] * s3),
                detReversed * -(matrix[1] * s2 - matrix[2] * s1 + matrix[3] * s0),

                detReversed * -(matrix[4] * c5 - matrix[6] * c2 + matrix[7] * c1),
                detReversed * (matrix[0] * c5 - matrix[2] * c2 + matrix[3] * c1),
                detReversed * -(matrix[0] * s5 - matrix[2] * s2 + matrix[3] * s1),
                detReversed * (matrix[0] * s4 - matrix[2] * s1 + matrix[3] * s0),

                detReversed * (matrix[4] * c4 - matrix[5] * c2 + matrix[7] * c0),
                detReversed * -(matrix[0] * c4 - matrix[1] * c2 + matrix[3] * c0),
                detReversed * (matrix[0] * s4 - matrix[1] * s2 + matrix[3] * s0),
                detReversed * -(matrix[0] * s3 - matrix[1] * s0 + matrix[3] * s0),

                detReversed * -(matrix[4] * c3 - matrix[5] * c1 + matrix[6] * c0),
                detReversed * (matrix[0] * c3 - matrix[1] * c1 + matrix[2] * c0),
                detReversed * -(matrix[0] * s3 - matrix[1] * s1 + matrix[2] * s0),
                detReversed * (matrix[0] * s2 - matrix[1] * s0 + matrix[2] * s0)
            };
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz, AnyNumber Tu, AnyNumber Tv, AnyNumber Tw>
        OTR_INLINE static auto Orthographic(Tx left, Ty right, Tz bottom, Tu top, Tv nearClip, Tw farClip)
        {
#define DECL_TYPE decltype(left + right + bottom + top + nearClip + farClip)

            auto result = Mat4x4<DECL_TYPE>::Identity();

            const auto lr = static_cast<DECL_TYPE>(1.0 / (left - right));
            const auto bt = static_cast<DECL_TYPE>(1.0 / (bottom - top));
            const auto nf = static_cast<DECL_TYPE>(1.0 / (nearClip - farClip));

            result[0]  = static_cast<DECL_TYPE>(-2.0 * lr);
            result[5]  = static_cast<DECL_TYPE>(-2.0 * bt);
            result[10] = static_cast<DECL_TYPE>(2.0 * nf);
            result[12] = static_cast<DECL_TYPE>((left + right) * lr);
            result[13] = static_cast<DECL_TYPE>((top + bottom) * bt);
            result[14] = static_cast<DECL_TYPE>((farClip + nearClip) * nf);

#undef DECL_TYPE

            return result;
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz, AnyNumber Tw>
        OTR_INLINE static auto Perspective(Tx fieldOfView, Ty aspectRatio, Tz nearClip, Tw farClip,
                                           AngleType angleType = AngleType::Radians)
        {
            if (angleType == AngleType::Degrees)
                fieldOfView = Math::DegToRad(fieldOfView);

#define DECL_TYPE decltype(fieldOfView * aspectRatio * nearClip * farClip)

            auto result = Mat4x4<DECL_TYPE>::Zero();

            const auto tanHalfFov = Math::Tan(fieldOfView * 0.5);

            result[0]  = static_cast<DECL_TYPE>(1.0 / (aspectRatio * tanHalfFov));
            result[5]  = static_cast<DECL_TYPE>(1.0 / tanHalfFov);
            result[10] = static_cast<DECL_TYPE>(-(farClip + nearClip) / (farClip - nearClip));
            result[11] = static_cast<DECL_TYPE>(-1.0);
            result[14] = static_cast<DECL_TYPE>(-(2.0 * farClip * nearClip) / (farClip - nearClip));

#undef DECL_TYPE

            return result;
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto LookAt(Vector3D<Tx> position, Vector3D<Ty> target, Vector3D<Tz> up)
        {
            const auto zAxis = Vec3D::Normalise(position - target);
            const auto xAxis = Vec3D::Normalise(Vec3D::Cross(up, zAxis));
            const auto yAxis = Vec3D::Cross(zAxis, xAxis);

#define DECL_TYPE decltype(position[0] + target[0] + up[0])
#define CELL(axis) Vec3D::Dot(axis, position)

            return Mat4x4<DECL_TYPE>{
                xAxis.x, yAxis.x, -zAxis.x, static_cast<DECL_TYPE>(0.0),
                xAxis.y, yAxis.y, -zAxis.y, static_cast<DECL_TYPE>(0.0),
                xAxis.z, yAxis.z, -zAxis.z, static_cast<DECL_TYPE>(0.0),
                -CELL(xAxis), -CELL(yAxis), CELL(zAxis), static_cast<DECL_TYPE>(1.0)
            };

#undef DECL_TYPE
#undef CELL
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Translation(const Vector3D<TNumber>& translation)
        {
            auto result = Mat4x4<TNumber>::Identity();

            result[12] = translation[0];
            result[13] = translation[1];
            result[14] = translation[2];

            return result;
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Translation(Vector3D<TNumber>&& translation)
        {
            auto result = Mat4x4<TNumber>::Identity();

            result[12] = translation[0];
            result[13] = translation[1];
            result[14] = translation[2];

            return result;
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Rotation(const Quaternion<TNumber>& quaternion)
        {
            const auto quaternionNormalised = Quat::Normalise(quaternion);

            const auto x   = quaternionNormalised[0];
            const auto y   = quaternionNormalised[1];
            const auto z   = quaternionNormalised[2];
            const auto w   = quaternionNormalised[3];
            const auto two = static_cast<TNumber>(2.0);

            auto result  = Mat4x4<TNumber>::Identity();

            result[0, 0] -= two * (y * y + z * z);
            result[0, 1] = two * (x * y - w * z);
            result[0, 2] = two * (x * z + w * y);

            result[1, 0] = two * (x * y + w * z);
            result[1, 1] -= two * (x * x + z * z);
            result[1, 2] = two * (y * z - w * x);

            result[2, 0] = two * (x * z - w * y);
            result[2, 1] = two * (y * z + w * x);
            result[2, 2] -= two * (x * x + y * y);

            return result;
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Rotation(Quaternion<TNumber>&& quaternion)
        {
            const auto quaternionNormalised = Normalise(quaternion);

            const auto x   = quaternionNormalised[0];
            const auto y   = quaternionNormalised[1];
            const auto z   = quaternionNormalised[2];
            const auto w   = quaternionNormalised[3];
            const auto two = static_cast<TNumber>(2.0);

            auto result  = Mat4x4<TNumber>::Identity();

            result[0, 0] -= two * (y * y + z * z);
            result[0, 1] = two * (x * y - w * z);
            result[0, 2] = two * (x * z + w * y);

            result[1, 0] = two * (x * y + w * z);
            result[1, 1] -= two * (x * x + z * z);
            result[1, 2] = two * (y * z - w * x);

            result[2, 0] = two * (x * z - w * y);
            result[2, 1] = two * (y * z + w * x);
            result[2, 2] -= two * (x * x + y * y);

            return result;
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Rotation(TNumber angle, Axis axis, AngleType angleType = AngleType::Radians)
        {
            if (angleType == AngleType::Degrees)
                angle = Math::DegToRad(angle);

            const auto cosAngle = Math::Cos(angle);
            const auto sinAngle = Math::Sin(angle);

            auto result = Mat4x4<TNumber>::Identity();

            switch (axis)
            {
                case Axis::X:
                    result[1, 1] = cosAngle;
                    result[1, 2] = -sinAngle;
                    result[2, 1] = sinAngle;
                    result[2, 2] = cosAngle;
                    break;
                case Axis::Y:
                    result[0, 0] = cosAngle;
                    result[0, 2] = sinAngle;
                    result[2, 0] = -sinAngle;
                    result[2, 2] = cosAngle;
                    break;
                case Axis::Z:
                    result[0, 0] = cosAngle;
                    result[0, 1] = -sinAngle;
                    result[1, 0] = sinAngle;
                    result[1, 1] = cosAngle;
                    break;
            }

            return result;
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Scale(const Vector3D<TNumber>& scale)
        {
            auto result = Mat4x4<TNumber>::Identity();

            result[0]  = scale[0];
            result[5]  = scale[1];
            result[10] = scale[2];

            return result;
        }

        template<AnyNumber TNumber>
        OTR_INLINE static auto Scale(Vector3D<TNumber>&& scale)
        {
            auto result = Mat4x4<TNumber>::Identity();

            result[0]  = scale[0];
            result[5]  = scale[1];
            result[10] = scale[2];

            return result;
        }

        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto TRS(const Vector3D<Tx>& translation,
                                   const Quaternion<Ty>& rotation,
                                   const Vector3D<Tz>& scale)
        {
            return MatrixUtils::Multiply(
                MatrixUtils::Multiply(Translation(translation),
                                      Rotation(rotation)),
                Scale(scale));
        }
    };
}

#endif //OTTERENGINE_MATRIX_H
