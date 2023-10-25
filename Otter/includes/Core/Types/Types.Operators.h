#ifndef OTTERENGINE_TYPES_OPERATORS_H
#define OTTERENGINE_TYPES_OPERATORS_H

#include <cstddef>

#include "Core/Defines.h"
#include "Core/Types/Types.Base.h"

OTR_INLINE UInt64 operator ""_KiB(const unsigned long long x) { return 1024 * x; }
OTR_INLINE UInt64 operator ""_MiB(const unsigned long long x) { return 1024 * 1024 * x; }
OTR_INLINE UInt64 operator ""_GiB(const unsigned long long x) { return 1024 * 1024 * 1024 * x; }

OTR_INLINE UInt64 operator ""_KB(const unsigned long long x) { return 1000 * x; }
OTR_INLINE UInt64 operator ""_MB(const unsigned long long x) { return 1000 * 1000 * x; }
OTR_INLINE UInt64 operator ""_GB(const unsigned long long x) { return 1000 * 1000 * 1000 * x; }

#endif //OTTERENGINE_TYPES_OPERATORS_H
