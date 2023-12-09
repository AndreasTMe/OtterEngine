#ifndef OTTERENGINE_TYPES_OPERATORS_H
#define OTTERENGINE_TYPES_OPERATORS_H

#include "Core/Defines.h"
#include "Core/Types/Types.Base.h"

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

#endif //OTTERENGINE_TYPES_OPERATORS_H
