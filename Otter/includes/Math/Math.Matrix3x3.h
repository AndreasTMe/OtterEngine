#ifndef OTTERENGINE_MATH_MATRIX3X3_H
#define OTTERENGINE_MATH_MATRIX3X3_H

#include "Math/Matrix.h"

namespace Otter::Math
{
    template<AnyNumber TNumber>
    OTR_INLINE TNumber Determinant(const Matrix<3, 3, TNumber>& matrix)
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
    OTR_INLINE Matrix<3, 3, TNumber> Transpose(const Matrix<3, 3, TNumber>& matrix)
    {
        return Matrix<3, 3, TNumber>{
            matrix[0], matrix[3], matrix[6],
            matrix[1], matrix[4], matrix[7],
            matrix[2], matrix[5], matrix[8]
        };
    }

    template<AnyNumber TNumber>
    OTR_INLINE Matrix<3, 3, TNumber> Inverse(const Matrix<3, 3, TNumber>& matrix)
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
            return Matrix<3, 3, TNumber>::Zero();

        const auto detReversed = 1.0 / determinant;

        const auto m3 = matrix[1] * matrix[8] - matrix[2] * matrix[7];
        const auto m4 = matrix[0] * matrix[8] - matrix[2] * matrix[6];
        const auto m5 = matrix[0] * matrix[7] - matrix[1] * matrix[6];

        const auto m6 = matrix[1] * matrix[5] - matrix[2] * matrix[4];
        const auto m7 = matrix[0] * matrix[5] - matrix[2] * matrix[3];
        const auto m8 = matrix[0] * matrix[4] - matrix[1] * matrix[3];

        return Matrix<3, 3, TNumber>{
            m0 * detReversed, -m1 * detReversed, m2 * detReversed,
            -m3 * detReversed, m4 * detReversed, -m5 * detReversed,
            m6 * detReversed, -m7 * detReversed, m8 * detReversed
        };
    }
}

#endif //OTTERENGINE_MATH_MATRIX3X3_H
