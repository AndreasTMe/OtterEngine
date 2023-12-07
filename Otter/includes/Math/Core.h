#ifndef OTTERENGINE_CORE_H
#define OTTERENGINE_CORE_H

#include <cmath>
#include <limits>

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Math/Math.Concepts.h"

// TODO: Add SIMD support.
namespace Otter::Math
{
    /**
     * @enum AngleType
     * @brief Represents the units of an angle.
     */
    enum class AngleType
    {
        Radians,
        Degrees
    };

    /**
     * @enum Axis
     * @brief Enumeration representing the different axes (X, Y, and Z).
     */
    enum class Axis
    {
        X,
        Y,
        Z
    };

    template<FloatingPointNumber TNumber>
    OTR_INLINE constexpr TNumber Pi = static_cast<TNumber>(3.141592653589793238462643383279502884L);

    template<FloatingPointNumber TNumber>
    OTR_INLINE constexpr TNumber Tau = static_cast<TNumber>(6.283185307179586476925286766559005768L);

    template<FloatingPointNumber TNumber>
    OTR_INLINE constexpr TNumber Epsilon = std::numeric_limits<TNumber>::epsilon();

    template<FloatingPointNumber TNumber>
    OTR_INLINE constexpr TNumber Sqrt2 = static_cast<TNumber>(1.414213562373095048801688724209698079L);

    template<FloatingPointNumber TNumber>
    OTR_INLINE constexpr TNumber Sqrt3 = static_cast<TNumber>(1.732050807568877293527446341505872366L);

    template<AnyNumber TNumber>
    OTR_INLINE constexpr TNumber PositiveInfinity = std::numeric_limits<TNumber>::infinity();

    template<AnyNumber TNumber>
    OTR_INLINE constexpr TNumber NegativeInfinity = -std::numeric_limits<TNumber>::infinity();

    /**
     * @brief Calculates the maximum value between two numbers.
     *
     * @tparam Tx The type of the first number.
     * @tparam Ty The type of the second number.
     *
     * @param x The first number.
     * @param y The second number.
     *
     * @return The maximum value between x and y.
     */
    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE constexpr auto Max(Tx x, Ty y) { return x > y ? x : y; }

    /**
     * @brief Template function to find the maximum value among a set of numbers.
     *
     * @tparam Tx The first parameter type.
     * @tparam Ty The second parameter type.
     * @tparam TRest The rest of the parameter types.
     *
     * @param x The first number.
     * @param y The second number.
     * @param rest The rest of the numbers.
     *
     * @return The maximum value among the numbers.
     */
    template<AnyNumber Tx, AnyNumber Ty, AnyNumber... TRest>
    OTR_INLINE constexpr auto Max(Tx x, Ty y, TRest... rest) { return Max(Max(x, y), rest...); }

    /**
     * @brief Calculates the minimum value between two numbers.
     *
     * @tparam Tx The type of the first number.
     * @tparam Ty The type of the second number.
     *
     * @param x The first number.
     * @param y The second number.
     *
     * @return The minimum value between x and y.
     */
    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE constexpr auto Min(Tx x, Ty y) { return x < y ? x : y; }

    /**
     * @brief Template function to find the minimum value among a set of numbers.
     *
     * @tparam Tx The first parameter type.
     * @tparam Ty The second parameter type.
     * @tparam TRest The rest of the parameter types.
     *
     * @param x The first number.
     * @param y The second number.
     * @param rest The rest of the numbers.
     *
     * @return The minimum value among the numbers.
     */
    template<AnyNumber Tx, AnyNumber Ty, AnyNumber... TRest>
    OTR_INLINE constexpr auto Min(Tx x, Ty y, TRest... rest) { return Min(Min(x, y), rest...); }

    /**
     * @brief Calculates the absolute value of a number.
     *
     * @tparam TNumber The type of the number.
     *
     * @param x The number for which to calculate the absolute value.
     *
     * @return The absolute value of the number.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Abs(TNumber x) { return x < 0 ? -x : x; }

    /**
     * @brief Checks whether two numbers are approximately equal.
     *
     * @tparam Tx The type of the first number.
     * @tparam Ty The type of the second number.
     *
     * @param a The first number.
     * @param b The second number.
     *
     * @return True if the numbers are approximately equal, false otherwise.
     */
    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE constexpr bool AreApproximatelyEqual(Tx a, Ty b) { return Abs(a - b) < Epsilon<Double128>; }

    /**
     * @brief Checks whether a number is approximately zero.
     *
     * @tparam TNumber The type of the number.
     *
     * @param x The number to be checked.
     *
     * @return True if the number is approximately zero, false otherwise.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr bool IsApproximatelyZero(TNumber x)
    {
        if constexpr (IntegerNumber<TNumber>)
            return x == 0;

        return AreApproximatelyEqual(x, static_cast<TNumber>(0.0));
    }

    /**
     * @brief Checks if the given number is NaN (Not a Number).
     *
     * @tparam TNumber The number type to check for NaN.
     *
     * @param x The number to check.
     *
     * @return True if the number is NaN, false otherwise.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr bool IsNaN(TNumber x) { return std::numeric_limits<TNumber>::has_quiet_NaN && std::isnan(x); }

    /**
     * @brief Determines whether a given number is infinity.
     *
     * @tparam TNumber The number type to check.
     *
     * @param x The number to check for infinity.
     *
     * @return True if the number is infinity, false otherwise.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr bool IsInfinity(TNumber x)
    {
        return std::numeric_limits<TNumber>::has_infinity && std::isinf(x);
    }

    /**
     * @brief Calculates the sign of a number.
     *
     * @tparam TNumber The type of the number.
     *
     * @param x The number to calculate the sign for.
     *
     * @return -1 if the number is less than zero, 1 if the number is greater than or equal to zero.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Sign(TNumber x) { return x < 0 ? -1 : 1; }

    /**
     * @brief Calculates the floating-point remainder of x divided by y.
     *
     * @tparam Tx The type of the dividend.
     * @tparam Ty The type of the divisor.
     *
     * @param x The dividend.
     * @param y The divisor.
     *
     * @return The floating-point remainder of x divided by y.
     */
    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE constexpr auto FMod(Tx x, Ty y) { return std::fmod(x, y); }

    /**
     * @brief Calculates the power of a number.
     *
     * @tparam Tx The type of the base number (`x`).
     * @tparam Ty The type of the exponent (`y`).
     *
     * @param x The base number.
     * @param y The exponent.
     *
     * @return The result of raising `x` to the power of `y`.
     */
    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE constexpr auto Power(Tx x, Ty y) { return (IsApproximatelyZero(y)) ? 1 : std::pow(x, y); }

    /**
     * @brief Calculates the square of a number.
     *
     * @tparam TNumber The type of the number.
     *
     * @param x The number to calculate the square of.
     *
     * @return The square of the given number.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Square(TNumber x) { return x * x; }

    /**
     * @brief Calculates the cube of a given number.
     *
     * @tparam TNumber The type of the number.
     *
     * @param x The input number.
     *
     * @return The cube of the input number.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Cube(TNumber x) { return x * x * x; }

    /**
     * @brief Checks whether a number is a power of two.
     *
     * @tparam TNumber The type of the number.
     *
     * @param x The number to be checked.
     *
     * @return True if the number is a power of two, false otherwise.
     */
    template<IntegerNumber TNumber>
    OTR_INLINE constexpr bool IsPowerOfTwo(TNumber x) { return x > 0 && IsApproximatelyZero(x & (x - 1)); }

    /**
     * @brief Compute the exponential of a number.
     *
     * @tparam TNumber The type of the number.
     *
     * @param x The input number.
     *
     * @return The exponential of the input number.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Exp(TNumber x) { return std::exp(x); }

    /**
     * @brief Calculates the square root of a number.
     *
     * @tparam TNumber The type of the number.
     *
     * @param x The number to calculate the square root of.
     *
     * @return The square root of the number.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto SquareRoot(TNumber x)
    {
        OTR_ASSERT_MSG(x >= 0, "Cannot take the square root of a negative number")
        return std::sqrt(x);
    }

    /**
     * @brief Calculates the base-2 logarithm of a given number.
     *
     * @tparam TNumber The type of the input number.
     *
     * @param x The number for which the base-2 logarithm should be calculated.
     *
     * @return The base-2 logarithm of the input number.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Log2(TNumber x) { return std::log2(x); }

    /**
     * @brief Calculates the base-10 logarithm of a given number.
     *
     * @tparam TNumber The type of the input number.
     *
     * @param x The number for which the base-10 logarithm should be calculated.
     *
     * @return The base-10 logarithm of the input number.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Log10(TNumber x) { return std::log10(x); }

    /**
     * @brief Computes the natural logarithm of a given number.
     *
     * @tparam TNumber The type of the input number.
     *
     * @param x The number for which the natural logarithm should be calculated.
     *
     * @return The natural logarithm of the input number.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Ln(TNumber x) { return std::log(x); }

    /**
     * @brief Converts degrees to radians.
     *
     * @tparam TNumber The numeric type of the input value.
     *
     * @param degrees The value in degrees to be converted to radians.
     *
     * @return The value in radians.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto DegToRad(TNumber degrees)
    {
        return static_cast<TNumber>(degrees * (Pi<Double128> / 180));
    }

    /**
     * @brief Converts radians to degrees.
     *
     * @tparam TNumber The numeric type of the input value.
     *
     * @param degrees The value in radians to be converted to degrees.
     *
     * @return The value in degrees.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto RadToDeg(TNumber radians)
    {
        return static_cast<TNumber>(radians * (180 / Pi<Double128>));
    }

    /**
     * @brief Calculates the sine of a given number.
     *
     * @tparam TNumber The type of the number.
     *
     * @param x The number for which the sine needs to be calculated.
     *
     * @return The sine of the given number.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Sin(TNumber x) { return std::sin(x); }

    /**
     * @brief Calculates the cosine of a given number.
     *
     * @tparam TNumber The type of the number.
     *
     * @param x The number for which the cosine needs to be calculated.
     *
     * @return The cosine of the given number.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Cos(TNumber x) { return std::cos(x); }

    /**
     * @brief Calculates the tangent of a given number.
     *
     * @tparam TNumber The type of the number.
     *
     * @param x The number for which the tangent needs to be calculated.
     *
     * @return The tangent of the given number.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Tan(TNumber x) { return std::tan(x); }

    /**
     * @brief Calculates the arc sine of a given number.
     *
     * @tparam TNumber The type of the number.
     *
     * @param x The number for which the arc sine needs to be calculated.
     *
     * @return The arc sine of the given number.
     *
     * @warning The behavior is undefined if the input number is outside the range [-1, 1].
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Asin(TNumber x) { return std::asin(x); }

    /**
    * @brief Calculates the arccosine of a given number.
    *
    * @tparam TNumber The type of the number.
    *
    * @param x The number for which the arccosine needs to be calculated.
    *
    * @return The arccosine of the given number.
    */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Acos(TNumber x) { return std::acos(x); }

    /**
   * @brief Calculates the arctangent of a given number.
   *
   * @tparam TNumber The type of the number.
   *
   * @param x The number for which the arctangent needs to be calculated.
   *
   * @return The arctangent of the given number.
   */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Atan(TNumber x) { return std::atan(x); }

    /**
     * @brief Calculates the arctangent of the quotient of two numbers.
     *
     * @tparam Tx The type of the numerator `y`.
     * @tparam Ty The type of the denominator `x`.
     *
     * @param y The numerator.
     * @param x The denominator.
     *
     * @return The arctangent of `y/x`.
     */
    template<AnyNumber Tx, AnyNumber Ty>
    OTR_INLINE constexpr auto Atan2(Tx y, Ty x) { return std::atan2(y, x); }

    /**
     * @brief Rounds a number to the nearest integer.
     *
     * @tparam TNumber The type of the number to round.
     *
     * @param x The number to round.
     *
     * @return The rounded value.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Round(TNumber x) { return std::round(x); }

    /**
     * @brief Calculates the ceiling value of a given number.
     *
     * @tparam TNumber The type of the input number.
     *
     * @param x The number whose ceiling value is to be calculated.
     *
     * @return The ceiling value of the given number.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Ceil(TNumber x) { return std::ceil(x); }

    /**
     * @brief Calculates the floor value of a given number.
     *
     * @tparam TNumber The type of the number.
     *
     * @param x The number whose floor value is to be calculated.
     *
     * @return The floor value of the given number.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Floor(TNumber x) { return std::floor(x); }

    /**
     * @brief Clamps a value between a minimum and maximum bound.
     *
     * @tparam Tx The type of the value.
     * @tparam Ty The type of the minimum bound.
     * @tparam Tz The type of the maximum bound.
     *
     * @param value The value to be clamped.
     * @param min The minimum bound.
     * @param max The maximum bound.
     *
     * @return The clamped value.
     */
    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE constexpr auto Clamp(Tx value, Ty min, Tz max)
    {
        return value < min ? min : value > max ? max : value;
    }

    /**
     * @brief Clamps a value between 0 and 1 inclusive.
     *
     * @tparam TNumber The type of the value and the bounds.
     *
     * @param value The value to be clamped.
     *
     * @return The clamped value.
     */
    template<AnyNumber TNumber>
    OTR_INLINE constexpr auto Clamp01(TNumber value) { return Clamp(value, (TNumber) 0.0, (TNumber) 1.0); }

    /**
     * @brief Performs linear interpolation between two values `a` and `b`
     * based on a parameter `t`. The resulting value is a weighted average of `a` and `b`,
     * where `t` represents the interpolation factor.
     *
     * @tparam Tx Any numeric type for value `a`
     * @tparam Ty Any numeric type for value `b`
     * @tparam Tz Any numeric type for parameter `t`
     *
     * @param a The start value
     * @param b The end value
     * @param t The interpolation factor
     *
     * @return The interpolated value between `a` and `b`
     *
     * @note The linear interpolation formula is: <br> a + (b - a) * t
     */
    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE constexpr auto Lerp(Tx a, Ty b, Tz t) { return a + (b - a) * t; }

    /**
     * @brief Performs linear interpolation between two values `a` and `b`
     * based on a parameter `t`. The resulting value is a weighted average of `a` and `b`,
     * where `t` represents the interpolation factor. The resulting value is then
     * clamped between `a` and `b`.
     *
     * @tparam Tx Any numeric type for value `a`
     * @tparam Ty Any numeric type for value `b`
     * @tparam Tz Any numeric type for parameter `t`
     *
     * @param a The start value
     * @param b The end value
     * @param t The interpolation factor
     *
     * @return The clamped interpolated value between `a` and `b`
     *
     * @note The linear interpolation formula is: <br> a + (b - a) * t
     */
    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE constexpr auto LerpClamped(Tx a, Ty b, Tz t) { return Clamp(Lerp(a, b, t), a, b); }

    /**
     * @brief Calculates the inverse linear interpolation value between two numbers.
     *
     * @tparam Tx The type of the first number.
     * @tparam Ty The type of the second number.
     * @tparam Tz The type of the value.
     *
     * @param a The first number.
     * @param b The second number.
     * @param value The value between a and b for which to calculate the inverse lerp.
     *
     * @return The inverse lerp value between a and b.
     *
     * @note The inverse linear interpolation formula is: <br> (value - a) / (b - a)
     * @note If a and b are approximately equal, returns 0.0.
     */
    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE constexpr auto InverseLerp(Tx a, Ty b, Tz value)
    {
        if (AreApproximatelyEqual(a, b))
            return static_cast<decltype(a * b * value)>(0.0);

        return (value - a) / (b - a);
    }

    /**
     * @brief Calculates the clamped inverse linear interpolation value between two numbers.
     *
     * @tparam Tx The type of the first number.
     * @tparam Ty The type of the second number.
     * @tparam Tz The type of the value.
     *
     * @param a The first number.
     * @param b The second number.
     * @param value The value between 'a' and 'b' for which to calculate the clamped inverse lerp.
     *
     * @return The clamped inverse lerp value between 'a' and 'b'.
     *
     * @note The inverse linear interpolation formula is: <br> (value - a) / (b - a)
     * @note If a and b are approximately equal, returns 0.0.
     */
    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE constexpr auto InverseLerpClamped(Tx a, Ty b, Tz value) { return Clamp01(InverseLerp(a, b, value)); }

    /**
     * @brief Calculates the smooth interpolation value between two numbers. This function takes in a minimum value,
     * a maximum value, and a value between the minimum and maximum values. It applies the smooth step formula
     * to interpolate between the minimum and maximum values based on the input value. The result is a smooth transition
     * from the minimum value to the maximum value as the input value increases.
     *
     * @tparam Tx The type of the minimum value.
     * @tparam Ty The type of the maximum value.
     * @tparam Tz The type of the input value.
     *
     * @param min The minimum value.
     * @param max The maximum value.
     * @param value The input value between the minimum and maximum values.
     *
     * @return The smooth interpolated value.
     *
     * @note The smooth step formula is: <br> t * t * (3 - 2 * t)
     */
    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE constexpr auto SmoothStep(Tx min, Ty max, Tz value)
    {
        auto t = Clamp01(InverseLerp(min, max, value));
        return t * t * (3 - 2 * t);
    }

    /**
     * @brief Calculates the inverse smooth step value of a given smoothened value between a minimum and maximum bound.
     * The function ensures that the output value smoothly transitions from the minimum to the maximum bound
     * based on the input smoothened value.
     *
     * @tparam Tx The type of the minimum bound.
     * @tparam Ty The type of the maximum bound.
     * @tparam Tz The type of the smoothened value.
     *
     * @param min The minimum bound.
     * @param max The maximum bound.
     * @param smoothenedValue The smoothened value.
     *
     * @return The calculated inverse smooth step value.
     *
     * @note The inverse smooth step formula is: <br> min + t * t * (3 - 2 * t) * (max - min)
     * @note If the minimum and maximum bounds are approximately equal, it returns the minimum bound.
     */
    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE constexpr auto InverseSmoothStep(Tx min, Ty max, Tz smoothenedValue)
    {
        if (AreApproximatelyEqual(min, max))
            return min;

        smoothenedValue = Clamp01(InverseLerp(min, max, smoothenedValue));
        return min + smoothenedValue * smoothenedValue * (3 - 2 * smoothenedValue) * (max - min);
    }

    /**
     * @brief Normalizes an angle to a specified unit, within the range of a full circle (360 degrees or 2π radians).
     *
     * @tparam TNumber The type of the angle value.
     *
     * @param angle The angle value to normalize.
     * @param angleType The unit type of the angle (radians or degrees). Defaults to radians.
     *
     * @return The normalized angle value.
     */
    template<AnyNumber TNumber>
    OTR_INLINE auto NormaliseAngle(TNumber angle, AngleType angleType = AngleType::Radians)
    {
        const auto fullCircle = angleType == AngleType::Radians ? Tau<Double128> : 360.0;
        const auto halfCircle = angleType == AngleType::Radians ? Pi<Double128> : 180.0;

        while (angle > halfCircle)
            angle -= fullCircle;

        while (angle <= -halfCircle)
            angle += fullCircle;

        return angle;
    }

    /**
     * @brief Performs linear interpolation between two angles, `angleA` and `angleB`, at a given interpolation
     * factor `t`. The result is the angle that is `t` fractional distance from `angleA` to `angleB`,
     * in the specified `angleType`.
     *
     * @tparam Tx The type of the first angle value.
     * @tparam Ty The type of the second angle value.
     * @tparam Tz The type of the interpolation factor.
     *
     * @param angleA The first angle value.
     * @param angleB The second angle value.
     * @param t The interpolation factor.
     * @param angleType The type of the angle values. Default is `AngleType::Radians`.
     *
     * @return The interpolated angle value.
     *
     * @note Uses the linear interpolation formula for the angles but normalises them in order to fall within
     * the range of a full circle before and after interpolation.
     */
    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto LerpAngle(Tx angleA, Ty angleB, Tz t, AngleType angleType = AngleType::Radians)
    {
        const auto fullCircle = angleType == AngleType::Radians ? Tau<Double128> : 360.0;
        const auto halfCircle = angleType == AngleType::Radians ? Pi<Double128> : 180.0;

        auto angleAMod = FMod(angleA, fullCircle);
        auto angleBMod = FMod(angleB, fullCircle);

        auto angleDifference = angleBMod - angleAMod;

        if (angleDifference > halfCircle)
            angleDifference -= fullCircle;
        else if (angleDifference < -halfCircle)
            angleDifference += fullCircle;

        auto result = angleAMod + angleDifference * t;

        result = FMod(result, fullCircle);
        if (result < 0.0)
            result += fullCircle;

        return result;
    }

    /**
     * @brief Calculates the inverse linear interpolation value between three angles. The result represents
     * the relative position of angleC between angleA and angleB. The angle type can be specified as either
     * radians or degrees.
     *
     * @tparam Tx The type of angleA.
     * @tparam Ty The type of angleB.
     * @tparam Tz The type of angleC.
     *
     * @param angleA The start angle.
     * @param angleB The end angle.
     * @param angleC The target angle.
     * @param angleType The unit of the angles (radians or degrees).
     *
     * @return The inverse lerp value between angleA and angleC with respect to angleB.
     *
     * @note Uses the inverse linear interpolation formula for the angles but normalises them in order to fall within
     * the range of a full circle before and after interpolation.
     */
    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto InverseLerpAngle(Tx angleA, Ty angleB, Tz angleC, AngleType angleType = AngleType::Radians)
    -> decltype(angleA * angleB * angleC)
    {
        const auto fullCircle = angleType == AngleType::Radians ? Tau<Double128> : 360.0;
        const auto halfCircle = angleType == AngleType::Radians ? Pi<Double128> : 180.0;

        auto angleAMod = FMod(angleA, fullCircle);
        auto angleBMod = FMod(angleB, fullCircle);
        auto angleCMod = FMod(angleC, fullCircle);

        auto angleDifference = angleBMod - angleAMod;

        if (angleDifference > halfCircle)
            angleDifference -= fullCircle;
        else if (angleDifference < -halfCircle)
            angleDifference += fullCircle;

        if (IsApproximatelyZero(angleDifference))
            return 0.0;

        return (angleCMod - angleAMod) / angleDifference;
    }

    /**
     * @brief Calculates the next value in a sequence by moving the current value towards
     * the target value at a specified speed. The value is only updated if the current value is
     * not already equal to the target value.
     *
     * @tparam Tx The type of the current value.
     * @tparam Ty The type of the target value.
     * @tparam Tz The type of the speed value.
     *
     * @param current The current value.
     * @param target The target value.
     * @param speed The speed at which to move towards the target.
     *
     * @return The updated value that is moved closer towards the target value.
     */
    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto MoveTowards(Tx current, Ty target, Tz speed)
    {
        if (AreApproximatelyEqual(current, target))
            return target;

        const auto direction = (target - current > 0) ? 1.0 : -1.0;
        current += direction * speed;

        if ((direction > 0 && current > target) || (direction < 0 && current < target))
            return target;

        return current;
    }

    /**
     * @brief Moves the current angle towards the target angle by a maximum delta angle. The resulting angle
     * is normalized to be within the range of a full circle (360 degrees or 2π radians).
     *
     * @tparam Tx The type of the current angle.
     * @tparam Ty The type of the target angle.
     * @tparam Tz The type of the maximum delta angle.
     *
     * @param current The current angle.
     * @param target The target angle.
     * @param maxDeltaAngle The maximum delta angle.
     * @param angleType The units of the angle. Defaults to radians.
     *
     * @return The new angle.
     *
     * @note The angle is normalized to be within the range of a full circle (360 degrees or 2π radians).
     */
    template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
    OTR_INLINE auto MoveTowardsAngle(Tx current, Ty target, Tz maxDeltaAngle, AngleType angleType = AngleType::Radians)
    {
        const auto fullCircle = angleType == AngleType::Radians ? Tau<Double128> : 360.0;
        const auto halfCircle = angleType == AngleType::Radians ? Pi<Double128> : 180.0;

        current = FMod(current, fullCircle);
        target  = FMod(target, fullCircle);

        const auto angleDifference = FMod(target - current + fullCircle, fullCircle) - halfCircle;
        const auto clampedAngle    = Clamp(angleDifference, -maxDeltaAngle, maxDeltaAngle);

        return FMod(current + clampedAngle + fullCircle, fullCircle);
    }
}

#endif //OTTERENGINE_CORE_H
