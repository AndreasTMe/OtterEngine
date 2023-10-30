#ifndef OTTERENGINE_MATH_MATRIX2X2_H
#define OTTERENGINE_MATH_MATRIX2X2_H

#include "Math/Core.h"
#include "Math/Matrix.h"

namespace Otter::Math
{
    template<>
    OTR_INLINE Matrix<2, 2, Int32> Zero() { return Matrix<2, 2, Int32>(0); }

    template<>
    Matrix<2, 2, Int32> Identity<2, 2>()
    {
        return Matrix<2, 2, Int32>{
            1, 0,
            0, 1,
        };
    }

    template<AnyNumber TNumber>
    TNumber Determinant(const Matrix<2, 2, TNumber>& matrix)
    {
        return matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0];
    }

    template<AnyNumber TNumber>
    Matrix<2, 2, TNumber> Inverse(const Matrix<2, 2, TNumber>& matrix)
    {
        TNumber det = Determinant(matrix);
        if (det == 0)
            return Math::Zero<2, 2>();

        return Matrix<2, 2, TNumber>{
            matrix[1][1] / det, -matrix[0][1] / det,
            -matrix[1][0] / det, matrix[0][0] / det
        };
    }

    template<AnyNumber TNumber>
    Matrix<2, 2, TNumber> Transpose(const Matrix<2, 2, TNumber>& matrix)
    {
        return Matrix<2, 2, TNumber>{
            matrix[0][0], matrix[1][0],
            matrix[0][1], matrix[1][1]
        };
    }
}

#endif //OTTERENGINE_MATH_MATRIX2X2_H
