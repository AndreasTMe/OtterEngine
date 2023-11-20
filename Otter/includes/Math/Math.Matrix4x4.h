#ifndef OTTERENGINE_MATH_MATRIX4X4_H
#define OTTERENGINE_MATH_MATRIX4X4_H

#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "Math/Quaternion.h"

namespace Otter::Math
{
    template<AnyNumber TNumber>
    OTR_INLINE TNumber Determinant(const Matrix<4, 4, TNumber>& matrix)
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
    OTR_INLINE Matrix<4, 4, TNumber> Transpose(const Matrix<4, 4, TNumber>& matrix)
    {
        return Matrix<4, 4, TNumber>{
            matrix[0], matrix[4], matrix[8], matrix[12],
            matrix[1], matrix[5], matrix[9], matrix[13],
            matrix[2], matrix[6], matrix[10], matrix[14],
            matrix[3], matrix[7], matrix[11], matrix[15]
        };
    }

    template<AnyNumber TNumber>
    OTR_INLINE Matrix<4, 4, TNumber> Inverse(const Matrix<4, 4, TNumber>& matrix)
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
            return Matrix<4, 4, TNumber>::Zero();

        const auto detReversed = 1.0 / determinant;

        return Matrix<4, 4, TNumber>{
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

    template<AnyNumber TNumber>
    OTR_INLINE Matrix<4, 4, TNumber> Orthographic(const TNumber& left,
                                                  const TNumber& right,
                                                  const TNumber& bottom,
                                                  const TNumber& top,
                                                  const TNumber& nearClip,
                                                  const TNumber& farClip)
    {
        Matrix<4, 4, TNumber> result = Matrix<4, 4, TNumber>::Identity();

        const auto lr = static_cast<TNumber>(1.0 / (left - right));
        const auto bt = static_cast<TNumber>(1.0 / (bottom - top));
        const auto nf = static_cast<TNumber>(1.0 / (nearClip - farClip));

        result[0]  = static_cast<TNumber>(-2.0 * lr);
        result[5]  = static_cast<TNumber>(-2.0 * bt);
        result[10] = static_cast<TNumber>(2.0 * nf);
        result[12] = static_cast<TNumber>((left + right) * lr);
        result[13] = static_cast<TNumber>((top + bottom) * bt);
        result[14] = static_cast<TNumber>((farClip + nearClip) * nf);

        return result;
    }

    template<AnyNumber TNumber>
    OTR_INLINE Matrix<4, 4, TNumber> Perspective(const TNumber& fieldOfView,
                                                 const TNumber& aspectRatio,
                                                 const TNumber& nearClip,
                                                 const TNumber& farClip)
    {
        const auto tanHalfFov = std::tan(fieldOfView * 0.5);

        Matrix<4, 4, TNumber> result = Matrix<4, 4, TNumber>::Zero();

        result[0]  = static_cast<TNumber>(1.0 / (aspectRatio * tanHalfFov));
        result[5]  = static_cast<TNumber>(1.0 / tanHalfFov);
        result[10] = static_cast<TNumber>(-(farClip + nearClip) / (farClip - nearClip));
        result[11] = static_cast<TNumber>(-1.0);
        result[14] = static_cast<TNumber>(-(2.0 * farClip * nearClip) / (farClip - nearClip));

        return result;
    }

    template<AnyNumber TNumber>
    OTR_INLINE Matrix<4, 4, TNumber> LookAt(Vector<3, TNumber> position,
                                            Vector<3, TNumber> target,
                                            Vector<3, TNumber> up)
    {
        const auto zAxis = Math::Normalise(position - target);
        const auto xAxis = Math::Normalise(Math::Cross(up, zAxis));
        const auto yAxis = Math::Cross(zAxis, xAxis);

        return Matrix<4, 4, TNumber>{
            xAxis.x, yAxis.x, -zAxis.x, static_cast<TNumber>(0.0),
            xAxis.y, yAxis.y, -zAxis.y, static_cast<TNumber>(0.0),
            xAxis.z, yAxis.z, -zAxis.z, static_cast<TNumber>(0.0),
            -Math::Dot(xAxis, position),
            -Math::Dot(yAxis, position),
            Math::Dot(zAxis, position),
            static_cast<TNumber>(1.0)
        };
    }
}

#endif //OTTERENGINE_MATH_MATRIX4X4_H
