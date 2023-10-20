#ifndef OTTERENGINE_TYPES_OPERATORS_H
#define OTTERENGINE_TYPES_OPERATORS_H

#include <cstddef>

#include "Core/Defines.h"
#include "Core/Types/Types.Base.h"

OTR_INLINE Size operator ""_KiB(const Size x) { return 1024 * x; }
OTR_INLINE Size operator ""_MiB(const Size x) { return 1024 * 1024 * x; }
OTR_INLINE Size operator ""_GiB(const Size x) { return 1024 * 1024 * 1024 * x; }

OTR_INLINE Size operator ""_KB(const Size x) { return 1000 * x; }
OTR_INLINE Size operator ""_MB(const Size x) { return 1000 * 1000 * x; }
OTR_INLINE Size operator ""_GB(const Size x) { return 1000 * 1000 * 1000 * x; }

#endif //OTTERENGINE_TYPES_OPERATORS_H
