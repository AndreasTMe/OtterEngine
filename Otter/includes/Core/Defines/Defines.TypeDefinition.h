#ifndef OTTERENGINE_DEFINES_TYPEDEFINITION_H
#define OTTERENGINE_DEFINES_TYPEDEFINITION_H

#include "Core/Defines/Defines.Compilation.h"
#include "Core/Types/Types.Base.h"

#define OTR_WITH_DEFAULT_CONSTRUCTOR(Type)  \
    OTR_INLINE Type() = default;            \
    OTR_INLINE ~Type() = default;

#define OTR_WITH_DEFAULT_CONSTRUCTOR_AND_VIRTUAL_DESTRUCTOR(Type)   \
    OTR_INLINE Type() = default;                                    \
    OTR_INLINE virtual ~Type() = default;

#define OTR_WITH_DEFAULT_CONSTRUCTOR_AND_FINAL_DESTRUCTOR(Type) \
    OTR_INLINE Type() = default;                                \
    OTR_INLINE ~Type() final = default;

#define OTR_WITH_ITERATOR(Iterator, pointer, size)                      \
    Iterator begin() noexcept { return Iterator(pointer); }             \
    Iterator end() noexcept { return Iterator(pointer + size); }        \
    Iterator rbegin() noexcept { return Iterator(pointer + size - 1); } \
    Iterator rend() noexcept { return Iterator(pointer - 1); }

#define OTR_WITH_CONST_ITERATOR(Iterator, pointer, size)                        \
    Iterator begin() const noexcept { return Iterator(pointer); }               \
    Iterator end() const noexcept { return Iterator(pointer + size); }          \
    Iterator rbegin() const noexcept { return Iterator(pointer + size - 1); }   \
    Iterator rend() const noexcept { return Iterator(pointer - 1); }

#define OTR_WITH_TYPENAME(T)                    \
    template<>                                  \
    struct TypeOf<T>                            \
    {                                           \
        OTR_INLINE static const char* GetName() \
        {                                       \
            return #T;                          \
        }                                       \
    };

#define OTR_DISABLE_CONSTRUCTION(Type)      \
    Type() = delete;                        \
    ~Type() = delete;

#define OTR_DISABLE_OBJECT_COPIES(Type)     \
    Type(Type&) = delete;                   \
    Type(const Type&) = delete;             \
    Type& operator=(const Type&) = delete;

#define OTR_DISABLE_OBJECT_MOVES(Type)      \
    Type(Type&&) = delete;                  \
    Type& operator=(Type&&) = delete;

#define OTR_DISABLE_HEAP_ALLOCATION                 \
    void* operator new(::Size size) = delete;       \
    void* operator new[](::Size size) = delete;     \
    void operator delete(void* block) = delete;     \
    void operator delete[](void* block) = delete;

#define ENABLE_TYPENAME(A) template<> struct TypeName<A> { static const char *Get() { return #A; }}

#endif //OTTERENGINE_DEFINES_TYPEDEFINITION_H
