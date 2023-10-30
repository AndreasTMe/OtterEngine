#ifndef OTTERENGINE_MATH_MATRIX4X4_H
#define OTTERENGINE_MATH_MATRIX4X4_H

#include "Math/Core.h"
#include "Math/Matrix.h"

namespace Otter::Math
{
    template<>
    OTR_INLINE Matrix<4, 4, Int32> Zero() { return { }; }

    template<>
    Matrix<4, 4, Int32> Identity<4, 4>()
    {
        return Matrix<4, 4, Int32>{
            1, 0, 0, 0,
            0, 1, 0, 0,
            0, 0, 1, 0,
            0, 0, 0, 1
        };
    }

    template<AnyNumber TNumber>
    TNumber Determinant(const Matrix<4, 4, TNumber>& matrix)
    {
        // Requires determinants of 2x2 and 3x3 matrices
        return 0;
    }

    template<AnyNumber TNumber>
    Matrix<4, 4, TNumber> Inverse(const Matrix<4, 4, TNumber>& matrix)
    {
        // Requires determinants
        return { };
    }

    template<AnyNumber TNumber>
    Matrix<4, 4, TNumber> Transpose(const Matrix<4, 4, TNumber>& matrix)
    {
        return { };
    }
}

#endif //OTTERENGINE_MATH_MATRIX4X4_H
