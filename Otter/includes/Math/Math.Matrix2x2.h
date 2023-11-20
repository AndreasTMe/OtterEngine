#ifndef OTTERENGINE_MATH_MATRIX2X2_H
#define OTTERENGINE_MATH_MATRIX2X2_H

#include "Math/Matrix.h"

namespace Otter::Math
{
    template<AnyNumber TNumber>
    OTR_INLINE TNumber Determinant(const Matrix<2, 2, TNumber>& matrix)
    {
        return matrix[0, 0] * matrix[1, 1] - matrix[0, 1] * matrix[1, 0];
    }

    template<AnyNumber TNumber>
    OTR_INLINE Matrix<2, 2, TNumber> Transpose(const Matrix<2, 2, TNumber>& matrix)
    {
        return Matrix<2, 2, TNumber>{
            matrix[0], matrix[2],
            matrix[1], matrix[3]
        };
    }

    template<AnyNumber TNumber>
    OTR_INLINE Matrix<2, 2, TNumber> Inverse(const Matrix<2, 2, TNumber>& matrix)
    {
        const auto determinant = matrix[0] * matrix[3] - matrix[1] * matrix[2];
        if (determinant == 0)
            return Matrix<2, 2, TNumber>::Zero();

        const auto detReversed = 1.0 / determinant;

        return Matrix<2, 2, TNumber>{
            matrix[3] * detReversed, -matrix[1] * detReversed,
            -matrix[2] * detReversed, matrix[0] * detReversed
        };
    }
}

#endif //OTTERENGINE_MATH_MATRIX2X2_H
