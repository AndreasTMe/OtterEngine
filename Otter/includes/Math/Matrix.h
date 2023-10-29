#ifndef OTTERENGINE_MATRIX_H
#define OTTERENGINE_MATRIX_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Math/Math.Concepts.h"

namespace Otter
{
    template<UInt8 Tx, UInt8 Ty, AnyNumber TNumber> requires Dimension<Tx> && Dimension<Ty>
    struct Matrix final
    {
        Matrix()
        {
            OTR_ASSERT_MSG(false, "Matrix of dimension {0}x{1} is not supported", (int) Tx, (int) Ty)
        }
    };

    template<AnyNumber TNumber>
    struct Matrix<4, 4, TNumber>;

    namespace Math
    {
        template<UInt8 Tx, UInt8 Ty>
        requires Dimension<Tx> && Dimension<Ty> && (Tx == Ty)
        Matrix<Tx, Ty, Int32> Identity();
    }
}

#include "Math/Math.Matrix4x4.h"

#endif //OTTERENGINE_MATRIX_H
