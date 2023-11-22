#ifndef OTTERENGINE_TYPES_BASE_H
#define OTTERENGINE_TYPES_BASE_H

#include <cstdint>
#include <stdfloat>
#include <memory>
#include <type_traits>

typedef std::size_t Size;

typedef std::uintptr_t UIntPtr;
typedef std::intptr_t  IntPtr;

typedef unsigned char Byte;

typedef char8_t  Char8;
typedef char16_t Char16;
typedef char32_t Char32;

typedef uint8_t  UInt8;
typedef uint16_t UInt16;
typedef uint32_t UInt32;
typedef uint64_t UInt64;

typedef int8_t  Int8;
typedef int16_t Int16;
typedef int32_t Int32;
typedef int64_t Int64;

typedef std::float16_t  Float16;
typedef std::float32_t  Float32;
typedef std::float64_t  Double64;
typedef std::float128_t Double128;

template<typename T>
using InitialiserList = std::initializer_list<T>;

template<typename TBase, typename TDerived>
concept IsBaseOf = std::is_base_of_v<TBase, TDerived>;

template<typename TDerived, typename TBase>
concept IsDerivedFrom = std::derived_from<TDerived, TBase>;

template<typename... TArgs>
struct VariadicArgs
{
    OTR_INLINE static constexpr UInt64 GetSize() { return sizeof...(TArgs); }
};

template<typename T>
[[nodiscard]] OTR_INLINE constexpr UInt64 GetHashCode(const T& value)
{
    return std::hash<T>{ }(value);
}

#endif //OTTERENGINE_TYPES_BASE_H
