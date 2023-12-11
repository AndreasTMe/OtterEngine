#ifndef OTTERENGINE_BASETYPES_H
#define OTTERENGINE_BASETYPES_H

#include <cstddef>
#include <cstdint>
#include <stdfloat>
#include <memory>
#include <type_traits>

#include "Core/Defines.h"

/// @brief Alias for the `std::uintptr_t` type.
typedef std::uintptr_t UIntPtr;
/// @brief Alias for the `std::intptr_t` type.
typedef std::intptr_t  IntPtr;
/// @brief Alias for the `std::nullptr_t` type.
typedef std::nullptr_t NullPtr;

/// @brief Alias for the `std::size_t` type.
typedef std::size_t Size;

/// @brief Alias for the `unsigned char` type. Has a size of 1 byte.
typedef unsigned char Byte;

/// @brief Alias for the `char8_t` type. Has a size of 1 byte.
typedef char8_t  Char8;
/// @brief Alias for the `char16_t` type. Has a size of 2 bytes.
typedef char16_t Char16;
/// @brief Alias for the `char32_t` type. Has a size of 4 bytes.
typedef char32_t Char32;

/// @brief Alias for the `uint8_t` type. Has a size of 1 byte.
typedef uint8_t  UInt8;
/// @brief Alias for the `uint16_t` type. Has a size of 2 bytes.
typedef uint16_t UInt16;
/// @brief Alias for the `uint32_t` type. Has a size of 4 bytes.
typedef uint32_t UInt32;
/// @brief Alias for the `uint64_t` type. Has a size of 8 bytes.
typedef uint64_t UInt64;

/// @brief Alias for the `int8_t` type. Has a size of 1 byte.
typedef int8_t  Int8;
/// @brief Alias for the `int16_t` type. Has a size of 2 bytes.
typedef int16_t Int16;
/// @brief Alias for the `int32_t` type. Has a size of 4 bytes.
typedef int32_t Int32;
/// @brief Alias for the `int64_t` type. Has a size of 8 bytes.
typedef int64_t Int64;

/// @brief Alias for the `std::float16_t` type. Has a size of 2 bytes.
typedef std::float16_t  Float16;
/// @brief Alias for the `std::float32_t` type. Has a size of 4 bytes.
typedef std::float32_t  Float32;
/// @brief Alias for the `std::float64_t` type. Has a size of 8 bytes.
typedef std::float64_t  Double64;
/// @brief Alias for the `std::float128_t` type. Has a size of 16 bytes.
typedef std::float128_t Double128;

/**
 * @brief Alias for the `std::initializer_list` type.
 *
 * @tparam T The type of the list.
 */
template<typename T>
using InitialiserList = std::initializer_list<T>;

/**
 * @brief Concept for checking if a type is the base of another.
 *
 * @tparam TBase The base type.
 * @tparam TDerived The derived type.
 */
template<typename TBase, typename TDerived>
concept IsBaseOf = std::is_base_of_v<TBase, TDerived>;

/**
 * @brief Concept for checking if a type is derived from another.
 *
 * @tparam TDerived The derived type.
 * @tparam TBase The base type.
 */
template<typename TDerived, typename TBase>
concept IsDerivedFrom = std::derived_from<TDerived, TBase>;

/**
 * @brief Concept for checking if a type is the same as another.
 *
 * @tparam TLeft The left type.
 * @tparam TRight The right type.
 */
template<typename TLeft, typename TRight>
concept IsSame = std::is_same_v<TLeft, TRight>;

/**
 * @brief Alias for the `std::underlying_type_t` type.
 *
 * @tparam T The type to get the underlying type of.
 */
template<typename T>
using UnderlyingType = std::underlying_type_t<T>;

/**
 * @brief Concept for checking if a type has an underlying type.
 *
 * @tparam T The type to check.
 * @tparam TComparison The type to compare to.
 */
template<typename T, typename TComparison>
concept WithUnderlyingType = IsSame<UnderlyingType<T>, TComparison>;

/**
 * @brief Alias for the `std::enable_if_t` type.
 *
 * @tparam Condition The condition to check.
 * @tparam T The type to enable if the condition is true.
 */
template<bool Condition, typename T = void>
using EnableIf = std::enable_if_t<Condition, T>;

/**
 * @brief A template class for working with a variadic list of arguments. Provides a static constexpr member function
 * to retrieve the number of arguments in the variadic list.
 *
 * @tparam TArgs The types of the arguments in the variadic list.
 */
template<typename... TArgs>
struct VariadicArgs
{
    /**
     * @brief Static constexpr member function to retrieve the number of arguments in the variadic list.
     *
     * @tparam TArgs The types of the arguments in the variadic list.
     *
     * @return The number of arguments in the variadic list.
     */
    OTR_INLINE static constexpr UInt64 GetSize() { return sizeof...(TArgs); }
};

/**
 * @brief Calculates the hash code for a given value. The hash code is calculated using the std::hash function object.
 *
 * @tparam T The type of the value.
 *
 * @param value The value for which the hash code is calculated.
 *
 * @return The hash code as a UInt64.
 */
template<typename T>
[[nodiscard]] OTR_INLINE constexpr UInt64 GetHashCode(const T& value)
{
    return std::hash<T>{ }(value);
}

/**
 * @brief Converts a value to Kibibytes
 *
 * @param x The value to convert
 *
 * @return The value in Kibibytes
 */
OTR_INLINE UInt64 operator ""_KiB(const unsigned long long x) { return 1024 * x; }

/**
 * @brief Converts a value to Mebibytes
 *
 * @param x The value to convert
 *
 * @return The value in Mebibytes
 */
OTR_INLINE UInt64 operator ""_MiB(const unsigned long long x) { return 1024 * 1024 * x; }

/**
 * @brief Converts a value to Gibibytes
 *
 * @param x The value to convert
 *
 * @return The value in Gibibytes
 */
OTR_INLINE UInt64 operator ""_GiB(const unsigned long long x) { return 1024 * 1024 * 1024 * x; }

/**
 * @brief Converts a value to Kilobytes
 *
 * @param x The value to convert
 *
 * @return The value in Kilobytes
 */
OTR_INLINE UInt64 operator ""_KB(const unsigned long long x) { return 1000 * x; }

/**
 * @brief Converts a value to Megabytes
 *
 * @param x The value to convert
 *
 * @return The value in Megabytes
 */
OTR_INLINE UInt64 operator ""_MB(const unsigned long long x) { return 1000 * 1000 * x; }

/**
 * @brief Converts a value to Gigabytes
 *
 * @param x The value to convert
 *
 * @return The value in Gigabytes
 */
OTR_INLINE UInt64 operator ""_GB(const unsigned long long x) { return 1000 * 1000 * 1000 * x; }

#endif //OTTERENGINE_BASETYPES_H