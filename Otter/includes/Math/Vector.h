#ifndef OTTERENGINE_VECTOR_H
#define OTTERENGINE_VECTOR_H

#include "Core/Types.h"
#include "Math/Math.Concepts.h"

namespace Otter
{
    template<UInt8 TDimension, AnyNumber TNumber> requires Dimension<TDimension>
    struct Vector final { };

    template<AnyNumber TNumber>
    struct Vector<2, TNumber>;

    template<AnyNumber TNumber>
    struct Vector<3, TNumber>;

    template<AnyNumber TNumber>
    struct Vector<4, TNumber>;

    namespace Math
    {
        template<UInt8 TDimension>
        requires Dimension<TDimension> && (TDimension < 4)
        Vector<TDimension, Int32> Left();

        template<UInt8 TDimension>
        requires Dimension<TDimension> && (TDimension < 4)
        Vector<TDimension, Int32> Right();

        template<UInt8 TDimension>
        requires Dimension<TDimension> && (TDimension < 4)
        Vector<TDimension, Int32> Down();

        template<UInt8 TDimension>
        requires Dimension<TDimension> && (TDimension < 4)
        Vector<TDimension, Int32> Up();

        Vector<3, Int32> Back();

        Vector<3, Int32> Forward();

        template<UInt8 TDimension>
        requires Dimension<TDimension>
        Vector<TDimension, Int32> One();

        template<UInt8 TDimension>
        requires Dimension<TDimension>
        Vector<TDimension, Int32> Zero();

        template<UInt8 TDimension, AnyNumber TNumber>
        requires Dimension<TDimension>
        Vector<TDimension, TNumber> VectorPositiveInfinity();

        template<UInt8 TDimension, AnyNumber TNumber>
        requires Dimension<TDimension>
        Vector<TDimension, TNumber> VectorNegativeInfinity();
    }
}

#include "Math/Math.Vector2.h"
#include "Math/Math.Vector3.h"
#include "Math/Math.Vector4.h"

#endif //OTTERENGINE_VECTOR_H
