#ifndef OTTERENGINE_DEFINES_UTILITIES_H
#define OTTERENGINE_DEFINES_UTILITIES_H

#include <bitset>
#include <iomanip>
#include <typeinfo>

#include "Core/Defines/Defines.Compilation.h"

#define OTR_USING_BASE(BaseType) using base = BaseType;

#define OTR_NAME_OF(...) #__VA_ARGS__

#define OTR_BIT_STRING(value, length) std::bitset<length>(value)
#define OTR_HEX_STRING(type, value, length)             \
    "0x" << std::setfill('0')                           \
         << std::setw(sizeof(type) * length)            \
         << std::hex                                    \
         << std::uppercase << static_cast<int>(value)

#define OTR_BITMASK(value) (1 << value)
#define OTR_IS_POWER_OF_TWO(value) (value > 1 && !(value & (value - 1)))

#define OTR_ALIGNED_OFFSET(value, alignment) ((value + (alignment - 1)) & ~(alignment - 1))
#define OTR_ALIGNED_PADDING(address, alignment) ((alignment - (address % alignment)) % alignment)

#if OTR_DEBUG
    #define OTR_INTERNAL_DEBUG_BLOCK(code)  \
        {                                   \
            code                            \
        }
#elif OTR_EDITOR
    #define OTR_INTERNAL_DEBUG_BLOCK(code)
#else
    #define OTR_INTERNAL_DEBUG_BLOCK(code)
#endif

#if OTR_DEBUG
    #define OTR_DEBUG_BLOCK(code)   \
        {                           \
            code                    \
        }
#elif OTR_EDITOR
    #define OTR_DEBUG_BLOCK(code)   \
        {                           \
            code                    \
        }
#else
    #define OTR_DEBUG_BLOCK(code)
#endif

#endif //OTTERENGINE_DEFINES_UTILITIES_H
