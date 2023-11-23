#ifndef OTTERENGINE_DEFINES_TYPEDEFINITION_H
#define OTTERENGINE_DEFINES_TYPEDEFINITION_H

#include "Core/Defines/Defines.Compilation.h"
#include "Core/Types/Types.Base.h"

#define OTR_WITH_DEFAULT_CONSTRUCTOR(Type)  \
    Type() = default;                       \
    ~Type() = default;

#define OTR_WITH_DEFAULT_CONSTRUCTOR_AND_VIRTUAL_DESTRUCTOR(Type)   \
    Type() = default;                                               \
    virtual ~Type() = default;

#define OTR_WITH_DEFAULT_CONSTRUCTOR_AND_FINAL_DESTRUCTOR(Type) \
    Type() = default;                                           \
    ~Type() final = default;

#define OTR_WITH_ITERATOR(Iterator, pointer, size)                                  \
    OTR_INLINE Iterator begin() noexcept { return Iterator(pointer); }              \
    OTR_INLINE Iterator end() noexcept { return Iterator(pointer + size); }         \
    OTR_INLINE Iterator rbegin() noexcept { return Iterator(pointer + size - 1); }  \
    OTR_INLINE Iterator rend() noexcept { return Iterator(pointer - 1); }

#define OTR_WITH_CONST_ITERATOR(Iterator, pointer, size)                                    \
    OTR_INLINE Iterator begin() const noexcept { return Iterator(pointer); }                \
    OTR_INLINE Iterator end() const noexcept { return Iterator(pointer + size); }           \
    OTR_INLINE Iterator rbegin() const noexcept { return Iterator(pointer + size - 1); }    \
    OTR_INLINE Iterator rend() const noexcept { return Iterator(pointer - 1); }

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

#endif //OTTERENGINE_DEFINES_TYPEDEFINITION_H
