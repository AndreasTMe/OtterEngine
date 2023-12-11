#ifndef OTTERENGINE_QUATERNION_H
#define OTTERENGINE_QUATERNION_H

#include "Math/Core.h"

namespace Otter
{
    namespace Math
    {
        template<AnyNumber TNumber>
        struct Quaternion;

        class QuaternionUtils;
    }

    /**
     * @brief Alias for a quaternion.
     *
     * @tparam TNumber The type of the coordinates in the quaternion.
     */
    template<AnyNumber TNumber>
    using Quaternion = Math::Quaternion<TNumber>;

    /**
     * @brief Alias for a quaternion utilities class.
     */
    using Quat = Math::QuaternionUtils;
}

namespace Otter::Math
{
    /**
     * @brief A struct representing a quaternion.
     *
     * @tparam TNumber The type of the quaternion's coordinates.
     */
    template<AnyNumber TNumber>
    struct Quaternion final
    {
        // HELP: I usually add the member variables at the bottom of the class,
        //      but it's the only member and the class is too large.
    private:
        TNumber m_Values[4];

    public:
        /**
         * @brief Default constructor that initializes the quaternion with zero coordinates.
         */
        constexpr Quaternion()
        {
            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = static_cast<TNumber>(0.0);
        }

        /**
         * @brief Constructs a quaternion object with all coordinates initialized to the given scalar value.
         *
         * @tparam TNumber The type of the coordinates in the quaternion.
         *
         * @param scalar The scalar value to initialize the quaternion coordinates with.
         */
        constexpr explicit Quaternion(TNumber scalar)
        {
            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = scalar;
        }

        /**
         * @brief Constructs a quaternion object with coordinates initialised from an initializer list.
         *
         * @tparam TNumber The type of the coordinates in the quaternion.
         *
         * @param list The list containing the coordinates to initialise the quaternion with.
         */
        constexpr Quaternion(InitialiserList<TNumber> list)
        {
            OTR_ASSERT_MSG(list.size() == 4, "Initialiser list size does not match Quaternion size")

            UInt64 i = 0;
            for (const TNumber& value: list)
                m_Values[i++] = value;
        }

        /**
         * @brief Copy constructor for quaternion objects.
         *
         * @tparam TDimension The dimension of the quaternion.
         * @tparam TNumber The type of the coordinates in the quaternion.
         *
         * @param other The quaternion object to copy from.
         */
        Quaternion(const Quaternion<TNumber>& other)
        {
            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = other.m_Values[i];
        }

        /**
         * @brief Move constructor for quaternion objects.
         *
         * @tparam TDimension The dimension of the quaternion.
         * @tparam TNumber The type of the coordinates in the quaternion.
         *
         * @param other The quaternion object to move from.
         */
        Quaternion(Quaternion<TNumber>&& other) noexcept
        {
            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = other.m_Values[i];
        }

        /**
         * @brief Copy assignment operator for quaternion objects.
         *
         * @tparam TDimension The dimension of the quaternion.
         * @tparam TNumber The type of the coordinates in the quaternion.
         *
         * @param other The quaternion object to copy from.
         *
         * @return A reference to the current quaternion object after the copy assignment.
         */
        Quaternion<TNumber>& operator=(const Quaternion<TNumber>& other)
        {
            if (this == &other)
                return *this;

            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = other.m_Values[i];

            return *this;
        }

        /**
         * @brief Move assignment operator for quaternion objects.
         *
         * @tparam TDimension The dimension of the quaternion.
         * @tparam TNumber The type of the coordinates in the quaternion.
         *
         * @param other The quaternion object to move from.
         *
         * @return A reference to the current quaternion object after the move assignment.
         */
        Quaternion<TNumber>& operator=(Quaternion<TNumber>&& other) noexcept
        {
            if (this == &other)
                return *this;

            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = other.m_Values[i];

            return *this;
        }

        /**
         * @brief Accesses the coordinate at the specified index.
         *
         * @tparam TNumber The type of the coordinates in the quaternion.
         *
         * @param index The index of the coordinate to access.
         *
         * @return A reference to the coordinate at the specified index.
         */
        TNumber& operator[](UInt8 index)
        {
            OTR_ASSERT_MSG(index < 4, "Index {0} is out of range", index)
            return m_Values[index];
        }

        /**
         * @brief Accesses the coordinate at the specified index.
         *
         * @tparam TNumber The type of the coordinates in the quaternion.
         *
         * @param index The index of the coordinate to access.
         *
         * @return A reference to the coordinate at the specified index.
         */
        const TNumber& operator[](UInt8 index) const
        {
            OTR_ASSERT_MSG(index < 4, "Index {0} is out of range", index)
            return m_Values[index];
        }

        /**
         * @brief Conversion operator that allows the quaternion to be converted to a different quaternion type.
         *
         * @tparam TOtherNumber The type of the coordinates in the resulting quaternion.
         *
         * @return A new quaternion with coordinates converted to the specified type.
         */
        template<AnyNumber TOtherNumber>
        explicit operator Quaternion<TOtherNumber>() const
        {
            Quaternion<TOtherNumber> result;

            for (UInt8 i = 0; i < 4; ++i)
                result[i] = static_cast<TOtherNumber>(m_Values[i]);

            return result;
        }

        /**
         * @brief Multiplies the current quaternion by another quaternion.
         *
         * @tparam TNumber The type of the coordinates in the current quaternion.
         * @tparam TOtherNumber The type of the coordinates in the other quaternion.
         *
         * @param other The quaternion to multiply with.
         *
         * @return A reference to the updated current quaternion after the multiplication.
         */
        template<AnyNumber TOtherNumber>
        Quaternion<TNumber>& operator*=(const Quaternion<TOtherNumber>& other)
        {
            *this = Quaternion<TNumber>{
                m_Values[3] * other.m_Values[0]
                + m_Values[0] * other.m_Values[3]
                + m_Values[1] * other.m_Values[2]
                - m_Values[2] * other.m_Values[1],

                m_Values[3] * other.m_Values[1]
                + m_Values[1] * other.m_Values[3]
                + m_Values[2] * other.m_Values[0]
                - m_Values[0] * other.m_Values[2],

                m_Values[3] * other.m_Values[2]
                + m_Values[2] * other.m_Values[3]
                + m_Values[0] * other.m_Values[1]
                - m_Values[1] * other.m_Values[0],

                m_Values[3] * other.m_Values[3]
                - m_Values[0] * other.m_Values[0]
                - m_Values[1] * other.m_Values[1]
                - m_Values[2] * other.m_Values[2]
            };

            return *this;
        }

        /**
         * @brief Multiplies the current quaternion by a scalar value, coordinate-wise.
         *
         * @tparam TNumber The type of the coordinates in the quaternion.
         * @tparam TOtherNumber The type of the scalar value.
         *
         * @param scalar The scalar value to multiply the quaternion coordinates with.
         *
         * @return A reference to the updated current quaternion after the multiplication.
         */
        template<AnyNumber TOtherNumber>
        Quaternion<TNumber>& operator*=(TOtherNumber scalar) noexcept
        {
            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] *= scalar;

            return *this;
        }

        /**
         * @brief Divides the current quaternion by a scalar value, coordinate-wise.
         *
         * @tparam TNumber The type of the coordinates in the quaternion.
         * @tparam TOtherNumber The type of the scalar value.
         *
         * @param scalar The scalar value to divide the quaternion coordinates with.
         *
         * @return A reference to the updated current quaternion after the division.
         */
        template<AnyNumber TOtherNumber>
        Quaternion<TNumber>& operator/=(TOtherNumber scalar) noexcept
        {
            OTR_ASSERT_MSG(scalar != 0, "Division by zero")

            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] /= scalar;

            return *this;
        }

        /**
         * @brief Multiplies the `lhs` quaternion with another quaternion.
         *
         * @tparam TNumber The type of the coordinates in the current quaternion.
         * @tparam TOtherNumber The type of the coordinates in the other quaternion.
         *
         * @param lhs The current quaternion.
         * @param rhs The other quaternion.
         *
         * @return The result of the multiplication.
         */
        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator*(Quaternion<TNumber> lhs, const Quaternion<TOtherNumber>& rhs)
        {
            lhs *= rhs;
            return lhs;
        }

        /**
         * @brief Multiplies the `lhs` quaternion with a scalar value, coordinate-wise.
         *
         * @tparam TNumber The type of the coordinates in the quaternion.
         * @tparam TOtherNumber The type of the scalar value.
         *
         * @param lhs The quaternion.
         * @param rhs The scalar value.
         *
         * @return The result of the multiplication.
         */
        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator*(Quaternion<TNumber> lhs, TOtherNumber rhs)
        {
            lhs *= rhs;
            return lhs;
        }

        /**
         * @brief Divides the `lhs` quaternion with a scalar value, coordinate-wise.
         *
         * @tparam TNumber The type of the coordinates in the quaternion.
         * @tparam TOtherNumber The type of the scalar value.
         *
         * @param lhs The quaternion.
         * @param rhs The scalar value.
         *
         * @return The result of the division.
         */
        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator/(Quaternion<TNumber> lhs, TOtherNumber rhs)
        {
            OTR_ASSERT_MSG(rhs != 0, "Division by zero")

            lhs /= rhs;
            return lhs;
        }

        /**
         * @brief Compares the current quaternion instance with another quaternion instance for equality.
         *
         * @tparam TNumber The type of the coordinates in the current quaternion.
         * @tparam TOtherNumber The type of the coordinates in the other quaternion.
         *
         * @param other The other quaternion instance to compare with.
         *
         * @return true if the current quaternion instance is equal to the other quaternion instance,
         *         false otherwise.
         *
         * @note The comparison behavior is different based on the data types of the coordinates. If both TNumber and
         * TOtherNumber are integral types, the function compares each coordinate of the quaternions using the '==' operator.
         * If either TNumber or TOtherNumber are floating-point types, the function uses the Math::AreApproximatelyEqual
         * function to compare each coordinate of the quaternion with a specified tolerance.
         */
        template<AnyNumber TOtherNumber>
        bool operator==(const Quaternion<TOtherNumber>& other) const noexcept
        {
            if constexpr (IntegerNumber<TNumber> && IntegerNumber<TOtherNumber>)
            {
                for (UInt8 i = 0; i < 4; ++i)
                    if (m_Values[i] != other.m_Values[i])
                        return false;

                return true;
            }

            for (UInt8 i = 0; i < 4; ++i)
                if (!Math::AreApproximatelyEqual(m_Values[i], other.m_Values[i]))
                    return false;

            return true;
        }

        /**
         * @brief Compares the current quaternion instance with another quaternion instance for inequality.
         *
         * @tparam TNumber The type of the coordinates in the current quaternion.
         * @tparam TOtherNumber The type of the coordinates in the other quaternion.
         *
         * @param other The other quaternion instance to compare with.
         *
         * @return true if the current quaternion instance is not equal to the other quaternion instance,
         *         false otherwise.
         *
         * @note The '!=' operator is implemented in terms of the '==' operator.
         */
        template<AnyNumber TOtherNumber>
        bool operator!=(const Quaternion<TOtherNumber>& other) const noexcept
        {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "Simplify"

            return !(*this == other);

#pragma clang diagnostic pop
        }

        /**
         * @brief Getter for the X coordinate value.
         *
         * @tparam TNumber The coordinate type.
         *
         * @return The X coordinate value.
         */
        [[nodiscard]] OTR_INLINE TNumber GetX() const noexcept { return m_Values[0]; }

        /**
         * @brief Setter for the X coordinate value.
         *
         * @tparam TNumber The coordinate value type.
         *
         * @param x The value to set.
         */
        OTR_INLINE void SetX(TNumber x) noexcept { m_Values[0] = x; }

        /**
         * @brief Getter for the Y coordinate value.
         *
         * @tparam TNumber The coordinate type.
         *
         * @return The Y coordinate value.
         */
        [[nodiscard]] OTR_INLINE TNumber GetY() const noexcept { return m_Values[1]; }

        /**
         * @brief Setter for the Y coordinate value.
         *
         * @tparam TNumber The coordinate value type.
         *
         * @param x The value to set.
         */
        OTR_INLINE void SetY(TNumber y) noexcept { m_Values[1] = y; }

        /**
         * @brief Getter for the Z coordinate value.
         *
         * @tparam TNumber The coordinate type.
         *
         * @return The Z coordinate value.
         */
        [[nodiscard]] OTR_INLINE TNumber GetZ() const noexcept { return m_Values[2]; }

        /**
         * @brief Setter for the Z coordinate value.
         *
         * @tparam TNumber The coordinate value type.
         *
         * @param x The value to set.
         */
        OTR_INLINE void SetZ(TNumber z) noexcept { m_Values[2] = z; }

        /**
         * @brief Getter for the W coordinate value.
         *
         * @tparam TNumber The coordinate type.
         *
         * @return The W coordinate value.
         */
        [[nodiscard]] OTR_INLINE TNumber GetW() const noexcept { return m_Values[3]; }

        /**
         * @brief Setter for the W coordinate value.
         *
         * @tparam TNumber The coordinate value type.
         *
         * @param x The value to set.
         */
        OTR_INLINE void SetW(TNumber w) noexcept { m_Values[3] = w; }

        /**
         * @brief Returns a quaternion with all coordinates set to zero.
         *
         * @tparam TNumber The coordinate type.
         *
         * @return The quaternion with all coordinates set to zero.
         */
        OTR_INLINE static constexpr Quaternion<TNumber> Zero() noexcept
        {
            return Quaternion<TNumber>(static_cast<TNumber>(0.0));
        }

        /**
         * @brief Returns a quaternion with all coordinates set to one.
         *
         * @tparam TNumber The coordinate type.
         *
         * @return The quaternion with all coordinates set to one.
         */
        OTR_INLINE static constexpr Quaternion<TNumber> One() noexcept
        {
            return Quaternion<TNumber>(static_cast<TNumber>(1.0));
        }

        /**
         * @brief Returns a quaternion with X, Y and Z coordinates set to zero and W coordinate set to one.
         *
         * @tparam TNumber The coordinate type.
         *
         * @return The quaternion with X, Y and Z coordinates set to zero and W coordinate set to one.
         */
        OTR_INLINE static constexpr Quaternion<TNumber> Identity() noexcept
        {
            return Quaternion<TNumber>{ static_cast<TNumber>(0.0),
                                        static_cast<TNumber>(0.0),
                                        static_cast<TNumber>(0.0),
                                        static_cast<TNumber>(1.0) };
        }
    };

    /**
     * @brief This class provides utility functions for working with quaternions.
     */
    class QuaternionUtils final
    {
    public:
        /**
         * @brief Checks whether a quaternion is approximately zero.
         *
         * @tparam TNumber The type of the coordinates of the quaternion.
         *
         * @param quaternion The quaternion to be checked.
         *
         * @return True if the quaternion is approximately zero, false otherwise.
         */
        template<AnyNumber TNumber>
        OTR_INLINE static bool IsApproximatelyZero(const Quaternion<TNumber>& quaternion)
        {
            if constexpr (IntegerNumber<TNumber>)
                return quaternion[0] == 0 && quaternion[1] == 0 && quaternion[2] == 0 && quaternion[3] == 0;

            return Math::IsApproximatelyZero(quaternion[0])
                   && Math::IsApproximatelyZero(quaternion[1])
                   && Math::IsApproximatelyZero(quaternion[2])
                   && Math::IsApproximatelyZero(quaternion[3]);
        }

        /**
         * @brief Checks whether two quaternions are approximately equal.
         *
         * @tparam Tx The type of the first quaternion's coordinates.
         * @tparam Ty The type of the second quaternion's coordinates.
         *
         * @param lhs The left quaternion.
         * @param rhs The right quaternion.
         *
         * @return True if the quaternions are approximately equal, false otherwise.
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static bool AreApproximatelyEqual(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs)
        {
            if constexpr (IntegerNumber<Tx> && IntegerNumber<Ty>)
                return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2] && lhs[3] == rhs[3];

            return Math::AreApproximatelyEqual(lhs[0], rhs[0])
                   && Math::AreApproximatelyEqual(lhs[1], rhs[1])
                   && Math::AreApproximatelyEqual(lhs[1], rhs[2])
                   && Math::AreApproximatelyEqual(lhs[2], rhs[3]);
        }

        /**
         * @brief Calculates the squared magnitude of a quaternion.
         *
         * @tparam TNumber The type of the coordinates of the quaternion.
         *
         * @param quaternion The quaternion for which the squared magnitude will be calculated.
         *
         * @return The squared magnitude of the given quaternion.
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto MagnitudeSquared(const Quaternion<TNumber>& quaternion)
        {
            return Math::Square(quaternion[0]) + Math::Square(quaternion[1])
                   + Math::Square(quaternion[2]) + Math::Square(quaternion[3]);
        }

        /**
         * @brief Calculates the magnitude of a quaternion.
         *
         * @tparam TNumber The type of the coordinates of the quaternion.
         *
         * @param quaternion The quaternion for which the magnitude will be calculated.
         *
         * @return The magnitude of the given quaternion.
         *
         * @note The formula used to calculate the magnitude is:
         * @code{.cpp}
         * magnitude = sqrt(x^2 + y^2 + z^2 + w^2)
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Magnitude(const Quaternion<TNumber>& quaternion)
        {
            return Math::SquareRoot(MagnitudeSquared(quaternion));
        }

        /**
         * @brief Normalizes a quaternion.
         *
         * @tparam TNumber The type of the coordinates of the quaternion.
         *
         * @param quaternion The quaternion to be normalized.
         *
         * @return The normalized quaternion.
         *
         * @note The formula used to normalize the quaternion is:
         * @code{.cpp}
         * normalized_quaternion = quaternion / |quaternion|
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Normalise(const Quaternion<TNumber>& quaternion)
        {
            const auto magnitude = Magnitude(quaternion);
            if (magnitude < Epsilon<Double128>)
                return Quaternion<TNumber>::Zero();

            return quaternion / magnitude;
        }

        /**
         * @brief Calculates the inverse of the given quaternion.
         *
         * @tparam TNumber The data type of the quaternion coordinates.
         *
         * @param quaternion The input quaternion.
         *
         * @return The inverse of the input quaternion.
         *
         * @note The formula used to calculate the inverse is:
         * @code{.cpp}
         * inverse = conjugate(quaternion) / dot(quaternion, quaternion)
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Inverse(const Quaternion<TNumber>& quaternion)
        {
            const auto dot = Dot(quaternion, quaternion);
            if (dot > (TNumber) 0)
            {
                const auto invDot = (TNumber) 1 / dot;

                return Quaternion<TNumber>{
                    -quaternion[0] * invDot,
                    -quaternion[1] * invDot,
                    -quaternion[2] * invDot,
                    quaternion[3] * invDot
                };
            }

            return Quaternion<TNumber>::Zero();
        }

        /**
         * @brief Calculates the dot product of two quaternions.
         *
         * @tparam Tx The type of the coordinates of the first quaternion.
         * @tparam Ty The type of the coordinates of the second quaternion.
         *
         * @param lhs The first quaternion in the dot product calculation.
         * @param rhs The second quaternion in the dot product calculation.
         *
         * @return The dot product of the two input quaternion.
         *
         * @note The formula used to calculate the dot product is:
         * @code{.cpp}
         * dot_product = lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Dot(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs)
        {
            return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3];
        }

        /**
         * @brief Calculates the angle between two quaternions.
         *
         * @tparam Tx The type of the coordinates of the first quaternion.
         * @tparam Ty The type of the coordinates of the second quaternion.
         *
         * @param lhs The first quaternion.
         * @param rhs The second quaternion.
         *
         * @return The angle between the two quaternion in radians.
         *
         * @note The formula used to calculate the angle is:
         * @code{.cpp}
         * angle = acos(dot(normalise(lhs), normalise(rhs)) / (|lhs| * |rhs|))
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Angle(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs)
        {
            const auto magnitudeProduct = Magnitude(lhs) * Magnitude(rhs);
            if (magnitudeProduct < Epsilon<Double128>)
                return 0;

            return Math::Acos(Dot(lhs, rhs) / magnitudeProduct);
        }

        /**
         * @brief Performs linear interpolation between two quaternions.
         *
         * @tparam Tx The type of the coordinates of the first quaternion.
         * @tparam Ty The type of the coordinates of the second quaternion.
         * @tparam Tz The type of the linear interpolation parameter.
         *
         * @param lhs The first quaternion.
         * @param rhs The second quaternion.
         * @param t The linear interpolation parameter.
         *
         * @return The interpolated quaternion.
         *
         * @note The formula used for linear interpolation is:
         * @code{.cpp}
         * interpolated_quaternion = lhs + (rhs - lhs) * t
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto Lerp(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs, Tz t)
        {
            const auto dot = Dot(lhs, rhs);
            if (dot < 0.0)
            {
                return Quaternion<decltype(lhs[0] - t * (lhs[0] + rhs[0]))>{
                    lhs[0] - t * (lhs[0] + rhs[0]),
                    lhs[1] - t * (lhs[1] + rhs[1]),
                    lhs[2] - t * (lhs[2] + rhs[2]),
                    lhs[3] - t * (lhs[3] + rhs[3])
                };
            }

            return Quaternion<decltype(lhs[0] - t * (lhs[0] - rhs[0]))>{
                lhs[0] - t * (lhs[0] - rhs[0]),
                lhs[1] - t * (lhs[1] - rhs[1]),
                lhs[2] - t * (lhs[2] - rhs[2]),
                lhs[3] - t * (lhs[3] - rhs[3])
            };
        }

        /**
         * @brief Performs linear interpolation between two quaternions.
         *
         * @tparam Tx The type of the coordinates of the first quaternion.
         * @tparam Ty The type of the coordinates of the second quaternion.
         * @tparam Tz The type of the linear interpolation parameter.
         *
         * @param lhs The first quaternion.
         * @param rhs The second quaternion.
         * @param t The linear interpolation parameter (clamped between 0 and 1).
         *
         * @return The interpolated quaternion.
         *
         * @note The formula used for linear interpolation is:
         * @code{.cpp}
         * interpolated_quaternion = lhs + (rhs - lhs) * t
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto LerpClamped(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs, Tz t)
        {
            return Lerp(lhs, rhs, Math::Clamp(t, (Tz) 0.0, (Tz) 1.0));
        }

        /**
         * @brief Performs spherical linear interpolation between two quaternions.
         *
         * @tparam Tx The type of the coordinates of the first quaternion.
         * @tparam Ty The type of the coordinates of the second quaternion.
         * @tparam Tz The type of interpolation parameter.
         *
         * @param lhs The first quaternion.
         * @param rhs The second quaternion.
         * @param t The interpolation parameter.
         *
         * @return The result of the interpolation.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto Slerp(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs, Tz t)
        {
            const auto lhsNormalised = Normalise(lhs);
            const auto rhsNormalised = Normalise(rhs);

            const auto dotProduct = Math::Clamp(Dot(lhsNormalised, rhsNormalised), -1.0, 1.0);
            const auto sign       = Math::Sign(dotProduct);

            if (Math::IsApproximatelyZero(dotProduct - static_cast<decltype(dotProduct)>(1.0)))
            {
                return Quaternion<decltype(lhsNormalised[0] + t * sign * (rhsNormalised[0] - lhsNormalised[0]))>{
                    lhsNormalised[0] + t * sign * (rhsNormalised[0] - lhsNormalised[0]),
                    lhsNormalised[1] + t * sign * (rhsNormalised[1] - lhsNormalised[1]),
                    lhsNormalised[2] + t * sign * (rhsNormalised[2] - lhsNormalised[2]),
                    lhsNormalised[3] + t * sign * (rhsNormalised[3] - lhsNormalised[3])
                };
            }

            const auto angle  = Math::Acos(dotProduct);
            const auto scale1 = Math::Sin((1.0f - t) * angle) / Math::Sin(angle);
            const auto scale2 = Math::Sin(t * angle) / Math::Sin(angle * sign);

            return Quaternion<decltype(scale1 * lhsNormalised[0] + scale2 * rhsNormalised[0])>{
                scale1 * lhsNormalised[0] + scale2 * rhsNormalised[0],
                scale1 * lhsNormalised[1] + scale2 * rhsNormalised[1],
                scale1 * lhsNormalised[2] + scale2 * rhsNormalised[2],
                scale1 * lhsNormalised[3] + scale2 * rhsNormalised[3]
            };
        }

        /**
         * @brief Performs spherical linear interpolation between two quaternions.
         *
         * @tparam Tx The type of the coordinates of the first quaternion.
         * @tparam Ty The type of the coordinates of the second quaternion.
         * @tparam Tz The type of interpolation parameter.
         *
         * @param lhs The first quaternion.
         * @param rhs The second quaternion.
         * @param t The interpolation parameter (clamped between 0 and 1).
         *
         * @return The result of the interpolation.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto SlerpClamped(const Quaternion<Tx>& lhs, const Quaternion<Ty>& rhs, Tz t)
        {
            return Slerp(lhs, rhs, Math::Clamp(t, (Tz) 0.0, (Tz) 1.0));
        }

        /**
         * @brief Move towards a target quaternion with a maximum distance delta.
         *
         * @tparam Tx The type of the coordinates of the current quaternion.
         * @tparam Ty The type of the coordinates of the target quaternion.
         * @tparam Tz Type of the maximum angle delta.
         *
         * @param current The current quaternion.
         * @param target The target quaternion.
         * @param maxDistanceDelta The maximum angle that can be covered in one step.
         *
         * @return A new quaternion that moves towards the target quaternion by the maximum angle delta.
         *
         * @note If the current quaternion is approximately equal to the target, the target is returned.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto MoveTowards(const Quaternion<Tx>& current,
                                           const Quaternion<Ty>& target,
                                           Tz maxAngleDelta,
                                           AngleType angleType = AngleType::Radians)
        {
            if (AreApproximatelyEqual(current, target))
                return target;

            if (angleType == AngleType::Degrees)
                maxAngleDelta = Math::DegToRad(maxAngleDelta);

            const auto memberWiseMul = current[0] * target[0]
                                       + current[1] * target[1]
                                       + current[2] * target[2]
                                       + current[3] * target[3];
            const auto acos          = Math::Acos(memberWiseMul);

            if (Math::IsApproximatelyZero(acos))
                return target;

            return Slerp(current, target, Math::Min(1.0f, maxAngleDelta / acos));
        }
    };
}

#endif //OTTERENGINE_QUATERNION_H
