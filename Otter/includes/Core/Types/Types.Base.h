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

template<typename... TArgs>
struct VariadicArgs
{
    static constexpr UInt64 GetSize() { return sizeof...(TArgs); }
};

template<typename T>
struct TypeOf
{
public:
    static const char* GetName() { return typeid(T).name(); }
};

#define  OTR_TYPEOF_DEFINITION(T)                   \
    template<>                                      \
    struct TypeOf<T>                                \
    {                                               \
    public:                                         \
        static const char* GetName() { return #T; } \
    };

OTR_TYPEOF_DEFINITION(void)
OTR_TYPEOF_DEFINITION(bool)
OTR_TYPEOF_DEFINITION(char)

OTR_TYPEOF_DEFINITION(Char8)
OTR_TYPEOF_DEFINITION(Char16)
OTR_TYPEOF_DEFINITION(Char32)
OTR_TYPEOF_DEFINITION(UInt8)
OTR_TYPEOF_DEFINITION(UInt16)
OTR_TYPEOF_DEFINITION(UInt32)
OTR_TYPEOF_DEFINITION(UInt64)
OTR_TYPEOF_DEFINITION(Int8)
OTR_TYPEOF_DEFINITION(Int16)
OTR_TYPEOF_DEFINITION(Int32)
OTR_TYPEOF_DEFINITION(Int64)
OTR_TYPEOF_DEFINITION(Float16)
OTR_TYPEOF_DEFINITION(Float32)
OTR_TYPEOF_DEFINITION(Double64)
OTR_TYPEOF_DEFINITION(Double128)

#undef OTR_TYPEOF_DEFINITION

#endif //OTTERENGINE_TYPES_BASE_H
