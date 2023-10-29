#ifndef OTTERENGINE_MATH_MATRIX4X4_H
#define OTTERENGINE_MATH_MATRIX4X4_H

#include "Math/Matrix.h"

namespace Otter
{
    template<AnyNumber TNumber>
    struct Matrix<4, 4, TNumber>
    {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

        union
        {
            struct
            {
                TNumber m00, m01, m02, m03;
                TNumber m10, m11, m12, m13;
                TNumber m20, m21, m22, m23;
                TNumber m30, m31, m32, m33;
            };

            TNumber Values[16];
        };

#pragma clang diagnostic pop

        Matrix()
        {
            for (UInt8 i = 0; i < 16; ++i)
                Values[i] = 0;
        }

        explicit Matrix(TNumber scalar)
        {
            for (UInt8 i = 0; i < 16; ++i)
                Values[i] = scalar;
        }

        template<typename... TArgs>
        requires((sizeof...(TArgs) == 15 && (AnyNumber<TArgs> && ...)))
        explicit Matrix(TNumber x, TArgs... args)
            : m00(x)
        {
            UInt8 i = 1;
            ([&]
            {
                Values[++i] = args;
            }(), ...);
        }

        Matrix(const Matrix<4, 4, TNumber>& other)
        {
            for (UInt8 i = 0; i < 16; ++i)
                Values[i] = other.Values[i];
        }

        Matrix(Matrix<4, 4, TNumber>&& other) noexcept
        {
            for (UInt8 i = 0; i < 16; ++i)
                Values[i] = other.Values[i];
        }

        Matrix<4, 4, TNumber>& operator=(const Matrix<4, 4, TNumber>& other)
        {
            for (UInt8 i = 0; i < 3; ++i)
                Values[i] = other.Values[i];

            return *this;
        }

        Matrix<4, 4, TNumber>& operator=(Matrix<4, 4, TNumber>&& other) noexcept
        {
            for (UInt8 i = 0; i < 3; ++i)
                Values[i] = other.Values[i];

            return *this;
        }

        TNumber& operator[](UInt8 index)
        {
            OTR_ASSERT_MSG(index < 16, "Index {0} is out of range", index)
            return Values[index];
        }

        const TNumber& operator[](UInt8 index) const
        {
            OTR_ASSERT_MSG(index < 16, "Index {0} is out of range", index)
            return Values[index];
        }
    };

    namespace Math
    {
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
    }
}

#endif //OTTERENGINE_MATH_MATRIX4X4_H
