#ifndef OTTERENGINE_MATH_MATRIX3X3_H
#define OTTERENGINE_MATH_MATRIX3X3_H

#include "Math/Core.h"
#include "Math/Matrix.h"

namespace Otter::Math
{
    template<>
    OTR_INLINE Matrix<3, 3, Int32> Zero() { return Matrix<3, 3, Int32>(0); }

    template<>
    Matrix<3, 3, Int32> Identity<3, 3>()
    {
        return Matrix<3, 3, Int32>{
            1, 0, 0,
            0, 1, 0,
            0, 0, 1,
        };
    }

    template<AnyNumber TNumber>
    TNumber Determinant(const Matrix<3, 3, TNumber>& matrix)
    {
        return 0;
    }

    template<AnyNumber TNumber>
    Matrix<3, 3, TNumber> Inverse(const Matrix<3, 3, TNumber>& matrix)
    {
        return { };
    }

    template<AnyNumber TNumber>
    Matrix<3, 3, TNumber> Transpose(const Matrix<3, 3, TNumber>& matrix)
    {
        return { };
    }
}

#endif //OTTERENGINE_MATH_MATRIX3X3_H
