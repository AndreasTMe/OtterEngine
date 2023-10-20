#ifndef OTTERENGINE_TYPES_BASE_H
#define OTTERENGINE_TYPES_BASE_H

#include <cstdint>
#include <stdfloat>
#include <memory>
#include <type_traits>

typedef std::size_t Size;

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
using Unique = std::unique_ptr<T>;

template<typename T, typename... TArgs>
constexpr Unique<T> CreateUniquePointer(TArgs&& ... args)
{
    return std::make_unique<T>(std::forward<TArgs>(args)...);
}

template<typename T>
using Shared = std::shared_ptr<T>;

template<typename T, typename... TArgs>
constexpr Shared<T> CreateSharedPointer(TArgs&& ... args)
{
    return std::make_shared<T>(std::forward<TArgs>(args)...);
}

template<typename T>
struct TypeOf
{
public:
    static const char* GetName() { return typeid(T).name(); }
};

template<typename... TArgs>
struct VariadicArgs
{
    static constexpr UInt64 GetSize() { return sizeof...(TArgs); }
};

#endif //OTTERENGINE_TYPES_BASE_H
