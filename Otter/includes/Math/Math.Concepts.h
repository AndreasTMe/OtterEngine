#ifndef OTTERENGINE_MATH_CONCEPTS_H
#define OTTERENGINE_MATH_CONCEPTS_H

#include <type_traits>

namespace Otter
{
    template<typename T>
    concept AnyNumber = std::is_arithmetic_v<T>
                        && !std::is_same_v<T, bool>
                        && !std::is_same_v<T, char>
                        && !std::is_const_v<T>;

    template<typename T>
    concept IntegerNumber = AnyNumber<T> && std::is_integral_v<T>;

    template<typename T>
    concept FloatingPointNumber = AnyNumber<T> && std::is_floating_point_v<T>;

    template<typename T>
    concept SignedNumber = std::is_signed_v<T>;

    template<typename T>
    concept UnsignedNumber = std::is_unsigned_v<T>;

    template<UInt8 num>
    concept Dimension = num == 2 || num == 3 || num == 4;
}

#endif //OTTERENGINE_MATH_CONCEPTS_H
