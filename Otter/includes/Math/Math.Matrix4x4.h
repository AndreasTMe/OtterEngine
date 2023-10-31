#ifndef OTTERENGINE_MATH_MATRIX4X4_H
#define OTTERENGINE_MATH_MATRIX4X4_H

#include "Math/Matrix.h"

namespace Otter::Math
{
    template<>
    OTR_INLINE Matrix<4, 4, Int32> Identity<4, 4>()
    {
        return Matrix<4, 4, Int32>{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
    }

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
        // To calculate the inverse of a 4x4 matrix, we need the minor of all elements.
        // The minor is the determinant of the sub-matrix formed by deleting one row and one column from
        // the original matrix.
        //
        //    |  0  1  2  3 |
        //    |  4  5  6  7 |          |  5  6  7 |           |  4  6  7 |           |  4  5  7 |           |  4  5  6 |
        // inv|  8  9 10 11 | = 0 * det|  9 10 11 |  - 1 * det|  8 10 11 |  + 2 * det|  8  9 11 |  - 3 * det|  8  9 10 |
        //    | 12 13 14 15 |          | 13 14 15 |           | 12 14 15 |           | 12 13 15 |           | 12 13 14 |
        //
        //                             |  5  6  7 |           |  4  6  7 |           |  4  5  7 |           |  4  5  6 |
        //                    + 4 * det|  9 10 11 |  - 5 * det|  8 10 11 |  + 6 * det|  8  9 11 |  - 7 * det|  8  9 10 |
        //                             | 13 14 15 |           | 12 14 15 |           | 12 13 15 |           | 12 13 14 |
        //
        //                             |  5  6  7 |           |  4  6  7 |           |  4  5  7 |           |  4  5  6 |
        //                    + 8 * det|  9 10 11 |  - 9 * det|  8 10 11 | + 10 * det|  8  9 11 | - 11 * det|  8  9 10 |
        //                             | 13 14 15 |           | 12 14 15 |           | 12 13 15 |           | 12 13 14 |
        //
        //                             |  5  6  7 |           |  4  6  7 |           |  4  5  7 |           |  4  5  6 |
        //                   + 12 * det|  9 10 11 | - 13 * det|  8 10 11 | + 14 * det|  8  9 11 | - 15 * det|  8  9 10 |
        //                             | 13 14 15 |           | 12 14 15 |           | 12 13 15 |           | 12 13 14 |

        const auto m00 = matrix[8] * matrix[13] - matrix[9] * matrix[12];
        const auto m01 = matrix[8] * matrix[14] - matrix[10] * matrix[12];
        const auto m02 = matrix[8] * matrix[15] - matrix[11] * matrix[12];
        const auto m03 = matrix[9] * matrix[14] - matrix[10] * matrix[13];
        const auto m04 = matrix[9] * matrix[15] - matrix[11] * matrix[13];
        const auto m05 = matrix[10] * matrix[15] - matrix[11] * matrix[14];

        const auto d0 = matrix[4] * m03 - matrix[5] * m01 + matrix[6] * m00;
        const auto d1 = matrix[4] * m04 - matrix[5] * m02 + matrix[7] * m00;
        const auto d2 = matrix[4] * m05 - matrix[6] * m02 + matrix[7] * m01;
        const auto d3 = matrix[5] * m05 - matrix[6] * m04 + matrix[7] * m03;

        const auto determinant = matrix[0] * d3 - matrix[1] * d2 + matrix[2] * d1 - matrix[3] * d0;
        if (determinant == 0)
            return Math::Zero<4, 4>();

        const auto detReversed = 1.0 / determinant;

        // TODO: Implement this

        return { };
    }
}

#endif //OTTERENGINE_MATH_MATRIX4X4_H
