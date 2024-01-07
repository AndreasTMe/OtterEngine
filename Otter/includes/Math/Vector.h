#ifndef OTTERENGINE_VECTOR_H
#define OTTERENGINE_VECTOR_H

#include "Math/Core.h"

namespace Otter
{
    namespace Math
    {
        template<UInt8 TDimension, AnyNumber TNumber> requires Dimension<TDimension>
        struct Vector;

        class Vector2DUtils;

        class Vector3DUtils;

        class Vector4DUtils;
    }

    /**
     * @brief Alias for a 2D vector.
     *
     * @tparam TNumber The type of the coordinates in the vector.
     */
    template<AnyNumber TNumber>
    using Vector2D = Math::Vector<2, TNumber>;

    /**
     * @brief Alias for a 3D vector.
     *
     * @tparam TNumber The type of the coordinates in the vector.
     */
    template<AnyNumber TNumber>
    using Vector3D = Math::Vector<3, TNumber>;

    /**
     * @brief Alias for a 4D vector.
     *
     * @tparam TNumber The type of the coordinates in the vector.
     */
    template<AnyNumber TNumber>
    using Vector4D = Math::Vector<4, TNumber>;

    /**
     * @brief Alias for a 2D vector utilities class.
     */
    using Vec2D = Math::Vector2DUtils;

    /**
     * @brief Alias for a 3D vector utilities class.
     */
    using Vec3D = Math::Vector3DUtils;

    /**
     * @brief Alias for a 4D vector utilities class.
     */
    using Vec4D = Math::Vector4DUtils;
}

namespace Otter::Math
{
    /**
     * @brief A struct representing a vector in n-dimensional space.
     *
     * @tparam TDimension The dimension of the vector.
     * @tparam TNumber The type of the vector's coordinates.
     *
     * @note The dimension of the vector can be 2, 3 or 4.
     * @note This struct can be used directly but it would be preferred to use the Vector2D, Vector3D and
     * Vector4D aliases instead.
     */
    template<UInt8 TDimension, AnyNumber TNumber> requires Dimension<TDimension>
    struct Vector final
    {
        // HELP: I usually add the member variables at the bottom of the class,
        //      but it's the only member and the class is too large.
    private:
        TNumber m_Values[TDimension];

    public:
        /**
         * @brief Default constructor that initializes the vector with zero coordinates.
         */
        constexpr Vector()
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = static_cast<TNumber>(0.0);
        }

        /**
         * @brief Constructs a vector object with all coordinates initialized to the given scalar value.
         *
         * @tparam TNumber The type of the coordinates in the vector.
         *
         * @param scalar The scalar value to initialize the vector coordinates with.
         */
        constexpr explicit Vector(TNumber scalar)
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = scalar;
        }

        /**
         * @brief Constructs a vector object with coordinates initialised from an initializer list.
         *
         * @tparam TNumber The type of the coordinates in the vector.
         *
         * @param list The list containing the coordinates to initialise the vector with.
         */
        constexpr Vector(InitialiserList<TNumber> list)
        {
            OTR_ASSERT(list.size() == TDimension, "Initialiser list size does not match Vector size")

            UInt64 i = 0;
            for (const TNumber& value: list)
                m_Values[i++] = value;
        }

        /**
         * @brief Copy constructor for vector objects.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The type of the coordinates in the vector.
         *
         * @param other The vector object to copy from.
         */
        Vector(const Vector<TDimension, TNumber>& other)
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = other.m_Values[i];
        }

        /**
         * @brief Move constructor for vector objects.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The type of the coordinates in the vector.
         *
         * @param other The vector object to move from.
         */
        Vector(Vector<TDimension, TNumber>&& other) noexcept
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = other.m_Values[i];
        }

        /**
         * @brief Copy assignment operator for vector objects.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The type of the coordinates in the vector.
         *
         * @param other The vector object to copy from.
         *
         * @return A reference to the current vector object after the copy assignment.
         */
        Vector<TDimension, TNumber>& operator=(const Vector<TDimension, TNumber>& other)
        {
            if (this == &other)
                return *this;

            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = other.m_Values[i];

            return *this;
        }

        /**
         * @brief Move assignment operator for vector objects.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The type of the coordinates in the vector.
         *
         * @param other The vector object to move from.
         *
         * @return A reference to the current vector object after the move assignment.
         */
        Vector<TDimension, TNumber>& operator=(Vector<TDimension, TNumber>&& other) noexcept
        {
            if (this == &other)
                return *this;

            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = other.m_Values[i];

            return *this;
        }

        /**
         * @brief Accesses the coordinate at the specified index.
         *
         * @tparam TNumber The type of the coordinates in the vector.
         *
         * @param index The index of the coordinate to access.
         *
         * @return A reference to the coordinate at the specified index.
         */
        TNumber& operator[](UInt8 index)
        {
            OTR_ASSERT(index < TDimension, "Index {0} is out of range", index)
            return m_Values[index];
        }

        /**
         * @brief Accesses the coordinate at the specified index.
         *
         * @tparam TNumber The type of the coordinates in the vector.
         *
         * @param index The index of the coordinate to access.
         *
         * @return A reference to the coordinate at the specified index.
         */
        const TNumber& operator[](UInt8 index) const
        {
            OTR_ASSERT(index < TDimension, "Index {0} is out of range", index)
            return m_Values[index];
        }

        /**
         * @brief Conversion operator that allows the vector to be converted to a different vector type.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TOtherNumber The type of the coordinates in the resulting vector.
         *
         * @return A new vector with coordinates converted to the specified type.
         */
        template<AnyNumber TOtherNumber>
        explicit operator Vector<TDimension, TOtherNumber>() const
        {
            Vector<TDimension, TOtherNumber> result;

            for (UInt8 i = 0; i < TDimension; ++i)
                result.m_Values[i] = static_cast<TOtherNumber>(m_Values[i]);

            return result;
        }

        /**
         * @brief Adds another vector to the current vector, coordinate-wise.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The type of the coordinates in the current vector.
         * @tparam TOtherNumber The type of the coordinates in the other vector.
         *
         * @param other The vector to be added to the current vector.
         *
         * @return A reference to the updated current vector after the addition.
         */
        template<AnyNumber TOtherNumber>
        Vector<TDimension, TNumber>& operator+=(const Vector<TDimension, TOtherNumber>& other)
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] += static_cast<TNumber>(other.m_Values[i]);

            return *this;
        }

        /**
         * @brief Adds another vector to the current vector, coordinate-wise.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The type of the coordinates in the vector.
         *
         * @param other The vector to be added to the current vector.
         *
         * @return A reference to the updated current vector after the addition.
         */
        Vector<TDimension, TNumber>& operator+=(const Vector<TDimension, TNumber>& other) noexcept
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] += other.m_Values[i];

            return *this;
        }

        /**
         * @brief Subtracts another vector from the current vector, coordinate-wise.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The type of the coordinates in the current vector.
         * @tparam TOtherNumber The type of the coordinates in the other vector.
         *
         * @param other The vector to be subtracted from the current vector.
         *
         * @return A reference to the updated current vector after the subtraction.
         */
        template<AnyNumber TOtherNumber>
        Vector<TDimension, TNumber>& operator-=(const Vector<TDimension, TOtherNumber>& other)
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] -= static_cast<TNumber>(other.m_Values[i]);

            return *this;
        }

        /**
         * @brief Subtracts another vector from the current vector, coordinate-wise.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The type of the coordinates in the vector.
         *
         * @param other The vector to be subtracted from the current vector.
         *
         * @return A reference to the updated current vector after the subtraction.
         */
        Vector<TDimension, TNumber>& operator-=(const Vector<TDimension, TNumber>& other) noexcept
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] -= other.m_Values[i];

            return *this;
        }

        /**
         * @brief Multiplies the current vector by a scalar value, coordinate-wise.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The type of the coordinates in the vector.
         * @tparam TOtherNumber The type of the scalar value.
         *
         * @param scalar The scalar value to multiply the vector coordinates with.
         *
         * @return A reference to the updated current vector after the multiplication.
         */
        template<AnyNumber TOtherNumber>
        Vector<TDimension, TNumber>& operator*=(TOtherNumber scalar) noexcept
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] *= scalar;

            return *this;
        }

        /**
         * @brief Divides the current vector by a scalar value, coordinate-wise.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The type of the coordinates in the vector.
         * @tparam TOtherNumber The type of the scalar value.
         *
         * @param scalar The scalar value to divide the vector coordinates with.
         *
         * @return A reference to the updated current vector after the division.
         */
        template<AnyNumber TOtherNumber>
        Vector<TDimension, TNumber>& operator/=(TOtherNumber scalar) noexcept
        {
            OTR_ASSERT(scalar != 0, "Division by zero")

            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] /= scalar;

            return *this;
        }

        /**
         * @brief Adds the `rhs` vector to the `lhs` vector, coordinate-wise.
         *
         * @tparam TDimension The dimension of the vectors.
         * @tparam TNumber The type of the coordinates in the left vector.
         * @tparam TOtherNumber The type of the coordinates in the right vector.
         *
         * @param lhs The left-hand side vector.
         * @param rhs The right-hand side vector.
         *
         * @return The result of the addition.
         */
        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator+(Vector<TDimension, TNumber> lhs,
                                        const Vector<TDimension, TOtherNumber>& rhs)
        {
            lhs += rhs;
            return lhs;
        }

        /**
         * @brief Subtracts the `rhs` vector from the `lhs` vector, coordinate-wise.
         *
         * @tparam TDimension The dimension of the vectors.
         * @tparam TNumber The type of the coordinates in the left vector.
         * @tparam TOtherNumber The type of the coordinates in the right vector.
         *
         * @param lhs The left-hand side vector.
         * @param rhs The right-hand side vector.
         *
         * @return The result of the subtraction.
         */
        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator-(Vector<TDimension, TNumber> lhs,
                                        const Vector<TDimension, TOtherNumber>& rhs)
        {
            lhs -= rhs;
            return lhs;
        }

        /**
         * @brief Multiplies the `lhs` vector with a scalar value, coordinate-wise.
         *
         * @tparam TDimension The dimension of the vectors.
         * @tparam TNumber The type of the coordinates in the vector.
         * @tparam TOtherNumber The type of the scalar value.
         *
         * @param lhs The vector.
         * @param rhs The scalar value.
         *
         * @return The result of the multiplication.
         */
        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator*(Vector<TDimension, TNumber> lhs, TOtherNumber rhs)
        {
            lhs *= rhs;
            return lhs;
        }

        /**
         * @brief Divides the `lhs` vector with a scalar value, coordinate-wise.
         *
         * @tparam TDimension The dimension of the vectors.
         * @tparam TNumber The type of the coordinates in the vector.
         * @tparam TOtherNumber The type of the scalar value.
         *
         * @param lhs The vector.
         * @param rhs The scalar value.
         *
         * @return The result of the division.
         */
        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator/(Vector<TDimension, TNumber> lhs, TOtherNumber rhs)
        {
            OTR_ASSERT(rhs != 0, "Division by zero")

            lhs /= rhs;
            return lhs;
        }

        /**
         * @brief Compares the current vector instance with another vector instance for equality.
         *
         * @tparam TDimension The dimension of the vectors.
         * @tparam TNumber The type of the coordinates in the current vector.
         * @tparam TOtherNumber The type of the coordinates in the other vector.
         *
         * @param other The other vector instance to compare with.
         *
         * @return true if the current vector instance is equal to the other vector instance,
         *         false otherwise.
         *
         * @note The comparison behavior is different based on the data types of the coordinates. If both TNumber and
         * TOtherNumber are integral types, the function compares each coordinate of the vectors using the '==' operator.
         * If either TNumber or TOtherNumber are floating-point types, the function uses the Math::AreApproximatelyEqual
         * function to compare each coordinate of the vectors with a specified tolerance.
         */
        template<AnyNumber TOtherNumber>
        bool operator==(const Vector<TDimension, TOtherNumber>& other) const noexcept
        {
            if constexpr (IntegerNumber<TNumber> && IntegerNumber<TOtherNumber>)
            {
                for (UInt8 i = 0; i < TDimension; ++i)
                    if (m_Values[i] != other.m_Values[i])
                        return false;

                return true;
            }

            for (UInt8 i = 0; i < TDimension; ++i)
                if (!Math::AreApproximatelyEqual(m_Values[i], other.m_Values[i]))
                    return false;

            return true;
        }

        /**
         * @brief Compares the current vector instance with another vector instance for inequality.
         *
         * @tparam TDimension The dimension of the vectors.
         * @tparam TNumber The type of the coordinates in the current vector.
         * @tparam TOtherNumber The type of the coordinates in the other vector.
         *
         * @param other The other vector instance to compare with.
         *
         * @return true if the current vector instance is not equal to the other vector instance,
         *         false otherwise.
         *
         * @note The '!=' operator is implemented in terms of the '==' operator.
         */
        template<AnyNumber TOtherNumber>
        bool operator!=(const Vector<TDimension, TOtherNumber>& other) const noexcept
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
         * @tparam TNumber The coordinate value type.
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
         * @tparam TNumber The coordinate value type.
         *
         * @return The Z coordinate value.
         *
         * @note This function is only available for 3D and 4D vectors.
         */
        [[nodiscard]] OTR_INLINE TNumber GetZ() const noexcept requires (TDimension >= 3) { return m_Values[2]; }

        /**
         * @brief Setter for the Z coordinate value.
         *
         * @tparam TNumber The coordinate value type.
         *
         * @param x The value to set.
         *
         * @note This function is only available for 3D and 4D vectors.
         */
        OTR_INLINE void SetZ(TNumber z) noexcept requires (TDimension >= 3) { m_Values[2] = z; }

        /**
         * @brief Getter for the W coordinate value.
         *
         * @tparam TNumber The coordinate value type.
         *
         * @return The W coordinate value.
         *
         * @note This function is only available for 4D vectors.
         */
        [[nodiscard]] OTR_INLINE TNumber GetW() const noexcept requires (TDimension == 4) { return m_Values[3]; }

        /**
         * @brief Setter for the W coordinate value.
         *
         * @tparam TNumber The coordinate value type.
         *
         * @param x The value to set.
         *
         * @note This function is only available for 4D vectors.
         */
        OTR_INLINE void SetW(TNumber w) noexcept requires (TDimension == 4) { m_Values[3] = w; }

        /**
         * @brief Returns a vector with the X coordinate set to -1.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The coordinate type.
         *
         * @return The vector with the X coordinate set to -1.
         *
         * @note This function is only available for 2D and 3D vectors.
         */
        OTR_INLINE static constexpr Vector<TDimension, TNumber> Left() requires (TDimension < 4)
        {
            if constexpr (TDimension == 2)
                return Vector<2, TNumber>{ static_cast<TNumber>(-1.0),
                                           static_cast<TNumber>(0.0) };
            else
                return Vector<3, TNumber>{ static_cast<TNumber>(-1.0),
                                           static_cast<TNumber>(0.0),
                                           static_cast<TNumber>(0.0) };
        }

        /**
         * @brief Returns a vector with the X coordinate set to 1.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The coordinate type.
         *
         * @return The vector with the X coordinate set to 1.
         *
         * @note This function is only available for 2D and 3D vectors.
         */
        OTR_INLINE static constexpr Vector<TDimension, TNumber> Right() requires (TDimension < 4)
        {
            if constexpr (TDimension == 2)
                return Vector<2, TNumber>{ static_cast<TNumber>(1.0),
                                           static_cast<TNumber>(0.0) };
            else
                return Vector<3, TNumber>{ static_cast<TNumber>(1.0),
                                           static_cast<TNumber>(0.0),
                                           static_cast<TNumber>(0.0) };
        }

        /**
         * @brief Returns a vector with the Y coordinate set to -1.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The coordinate type.
         *
         * @return The vector with the Y coordinate set to -1.
         *
         * @note This function is only available for 2D and 3D vectors.
         */
        OTR_INLINE static constexpr Vector<TDimension, TNumber> Down() requires (TDimension < 4)
        {
            if constexpr (TDimension == 2)
                return Vector<2, TNumber>{ static_cast<TNumber>(0.0),
                                           static_cast<TNumber>(-1.0) };
            else
                return Vector<3, TNumber>{ static_cast<TNumber>(0.0),
                                           static_cast<TNumber>(-1.0),
                                           static_cast<TNumber>(0.0) };
        }

        /**
         * @brief Returns a vector with the Y coordinate set to 1.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The coordinate type.
         *
         * @return The vector with the Y coordinate set to 1.
         *
         * @note This function is only available for 2D and 3D vectors.
         */
        OTR_INLINE static constexpr Vector<TDimension, TNumber> Up() requires (TDimension < 4)
        {
            if constexpr (TDimension == 2)
                return Vector<2, TNumber>{ static_cast<TNumber>(0.0),
                                           static_cast<TNumber>(1.0) };
            else
                return Vector<3, TNumber>{ static_cast<TNumber>(0.0),
                                           static_cast<TNumber>(1.0),
                                           static_cast<TNumber>(0.0) };
        }

        /**
         * @brief Returns a vector with the Z coordinate set to -1.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The coordinate type.
         *
         * @return The vector with the Z coordinate set to -1.
         *
         * @note This function is only available for 3D vectors.
         */
        OTR_INLINE static constexpr Vector<TDimension, TNumber> Back() requires (TDimension == 3)
        {
            return Vector<3, TNumber>{ static_cast<TNumber>(0.0),
                                       static_cast<TNumber>(0.0),
                                       static_cast<TNumber>(-1.0) };
        }

        /**
         * @brief Returns a vector with the Z coordinate set to 1.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The coordinate type.
         *
         * @return The vector with the Z coordinate set to 1.
         *
         * @note This function is only available for 3D vectors.
         */
        OTR_INLINE static constexpr Vector<TDimension, TNumber> Forward() requires (TDimension == 3)
        {
            return Vector<3, TNumber>{ static_cast<TNumber>(0.0),
                                       static_cast<TNumber>(0.0),
                                       static_cast<TNumber>(1.0) };
        }

        /**
         * @brief Returns a vector with all coordinates set to zero.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The coordinate type.
         *
         * @return The vector with all coordinates set to zero.
         */
        OTR_INLINE static constexpr Vector<TDimension, TNumber> Zero()
        {
            return Vector<TDimension, TNumber>(static_cast<TNumber>(0.0));
        }

        /**
         * @brief Returns a vector with all coordinates set to one.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The coordinate type.
         *
         * @return The vector with all coordinates set to one.
         */
        OTR_INLINE static constexpr Vector<TDimension, TNumber> One()
        {
            return Vector<TDimension, TNumber>(static_cast<TNumber>(1.0));
        }

        /**
         * @brief Returns a vector with all coordinates set to positive infinity.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The coordinate type.
         *
         * @return The vector with all coordinates set to positive infinity.
         */
        OTR_INLINE static constexpr Vector<TDimension, TNumber> PositiveInfinity()
        {
            return Vector<TDimension, TNumber>(Math::PositiveInfinity<TNumber>());
        }

        /**
         * @brief Returns a vector with all coordinates set to negative infinity.
         *
         * @tparam TDimension The dimension of the vector.
         * @tparam TNumber The coordinate type.
         *
         * @return The vector with all coordinates set to negative infinity.
         */
        OTR_INLINE static constexpr Vector<TDimension, TNumber> NegativeInfinity()
        {
            return Vector<TDimension, TNumber>(Math::NegativeInfinity<TNumber>());
        }
    };

    /**
     * @brief This class provides utility functions for working with 2D vectors.
     */
    class Vector2DUtils final
    {
        template<AnyNumber TNumber>
        using Vec2D = Vector<2, TNumber>;

    public:
        /**
         * @brief Checks whether a 2D vector is approximately zero.
         *
         * @tparam TNumber The type of the coordinates of the vector.
         *
         * @param vector The vector to be checked.
         *
         * @return True if the vector is approximately zero, false otherwise.
         */
        template<AnyNumber TNumber>
        OTR_INLINE static bool IsApproximatelyZero(const Vec2D<TNumber>& vector)
        {
            if constexpr (IntegerNumber<TNumber>)
                return vector[0] == 0 && vector[1] == 0;

            return Math::IsApproximatelyZero(vector[0]) && Math::IsApproximatelyZero(vector[1]);
        }

        /**
         * @brief Checks whether two 2D vectors are approximately equal.
         *
         * @tparam Tx The type of the first vector's coordinates.
         * @tparam Ty The type of the second vector's coordinates.
         *
         * @param lhs The left vector.
         * @param rhs The right vector.
         *
         * @return True if the vectors are approximately equal, false otherwise.
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static bool AreApproximatelyEqual(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs)
        {
            if constexpr (IntegerNumber<Tx> && IntegerNumber<Ty>)
                return lhs[0] == rhs[0] && lhs[1] == rhs[1];

            return Math::AreApproximatelyEqual(lhs[0], rhs[0]) && Math::AreApproximatelyEqual(lhs[1], rhs[1]);
        }

        /**
         * @brief Calculates the squared magnitude of a 2D vector.
         *
         * @tparam TNumber The type of the coordinates of the vector.
         *
         * @param vector The 2D vector for which the squared magnitude will be calculated.
         *
         * @return The squared magnitude of the given vector.
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto MagnitudeSquared(const Vec2D<TNumber>& vector)
        {
            return Math::Square(vector[0]) + Math::Square(vector[1]);
        }

        /**
         * @brief Calculates the magnitude of a 2D vector.
         *
         * @tparam TNumber The type of the coordinates of the vector.
         *
         * @param vector The 2D vector for which the magnitude will be calculated.
         *
         * @return The magnitude of the given vector.
         *
         * @note The formula used to calculate the magnitude is:
         * @code{.cpp}
         * magnitude = sqrt(x^2 + y^2)
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Magnitude(const Vec2D<TNumber>& vector)
        {
            return Math::SquareRoot(MagnitudeSquared(vector));
        }

        /**
         * @brief This function clamps the magnitude of a 2D vector.
         *
         * @tparam TNumber The type of the coordinates of the vector.
         * @tparam TMaxMagnitude The type of the maximum magnitude.
         *
         * @param vector The 2D vector.
         * @param maxMagnitude The maximum magnitude.
         *
         * @return The clamped vector.
         */
        template<AnyNumber TNumber, AnyNumber TMaxMagnitude>
        OTR_INLINE static auto ClampMagnitude(const Vec2D<TNumber>& vector, const TMaxMagnitude& maxMagnitude)
        {
            const auto magnitudeSquared = MagnitudeSquared(vector);
            if (magnitudeSquared > Math::Square(maxMagnitude))
                return vector * Math::Min(maxMagnitude / Math::SquareRoot(magnitudeSquared), 1);

            return vector;
        }

        /**
         * @brief Normalizes a 2D vector.
         *
         * @tparam TNumber The type of the coordinates of the vector.
         *
         * @param vector The 2D vector to be normalized.
         *
         * @return The normalized vector.
         *
         * @note The formula used to normalize the vector is:
         * @code{.cpp}
         * normalized_vector = vector / |vector|
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Normalise(const Vec2D<TNumber>& vector)
        {
            const auto magnitude = Magnitude(vector);
            if (magnitude < Epsilon<Double128>)
                return Vec2D<TNumber>::Zero();

            return vector / magnitude;
        }

        /**
         * @brief Calculates the dot product of two 2D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first 2D vector in the dot product calculation.
         * @param rhs The second 2D vector in the dot product calculation.
         *
         * @return The dot product of the two input vectors.
         *
         * @note The formula used to calculate the dot product is:
         * @code{.cpp}
         * dot_product = lhs.x * rhs.x + lhs.y * rhs.y
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Dot(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs)
        {
            return lhs[0] * rhs[0] + lhs[1] * rhs[1];
        }

        /**
         * @brief Calculates the squared distance between two 2D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first 2D vector.
         * @param rhs The second 2D vector.
         *
         * @return The squared distance between the two vectors.
         *
         * @note The formula used to calculate the squared distance is:
         * @code{.cpp}
         * distance_squared = (lhs.x - rhs.x)^2 + (lhs.y - rhs.y)^2
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto DistanceSquared(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs)
        {
            return MagnitudeSquared(lhs - rhs);
        }

        /**
         * @brief Calculates the distance between two 2D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first 2D vector.
         * @param rhs The second 2D vector.
         *
         * @return The distance between the two vectors.
         *
         * @note The formula used to calculate the distance is:
         * @code{.cpp}
         * distance = sqrt((lhs.x - rhs.x)^2 + (lhs.y - rhs.y)^2)
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Distance(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs)
        {
            return Magnitude(lhs - rhs);
        }

        /**
         * @brief Performs linear interpolation between two 2D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         * @tparam Tz The type of the linear interpolation parameter.
         *
         * @param lhs The first vector.
         * @param rhs The second vector.
         * @param t The linear interpolation parameter.
         *
         * @return The interpolated vector.
         *
         * @note The formula used for linear interpolation is:
         * @code{.cpp}
         * interpolated_vector = lhs + (rhs - lhs) * t
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto Lerp(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs, Tz t)
        {
            return lhs + (rhs - lhs) * t;
        }

        /**
         * @brief Performs clamped linear interpolation between two 2D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         * @tparam Tz The type of the linear interpolation parameter.
         *
         * @param lhs The first vector.
         * @param rhs The second vector.
         * @param t The linear interpolation parameter (clamped between 0 and 1).
         *
         * @return The interpolated vector.
         *
         * @note The formula used for linear interpolation is:
         * @code{.cpp}
         * interpolated_vector = lhs + (rhs - lhs) * t
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto LerpClamped(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs, Tz t)
        {
            return Lerp(lhs, rhs, Math::Clamp(t, (Tz) 0.0, (Tz) 1.0));
        }

        /**
         * @brief Calculates the coordinate-wise maximum of two 2D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first input vector.
         * @param rhs The second input vector.
         *
         * @return The coordinate-wise maximum of the two input vectors.
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Max(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs)
        {
            return Vec2D<decltype(Math::Max(lhs[0], rhs[0]))>{ Math::Max(lhs[0], rhs[0]),
                                                               Math::Max(lhs[1], rhs[1]) };
        }

        /**
         * @brief Calculates the coordinate-wise minimum of two 2D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first input vector.
         * @param rhs The second input vector.
         *
         * @return The coordinate-wise minimum of the two input vectors.
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Min(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs)
        {
            return Vec2D<decltype(Math::Min(lhs[0], rhs[0]))>{ Math::Min(lhs[0], rhs[0]),
                                                               Math::Min(lhs[1], rhs[1]) };
        }

        /**
         * @brief Clamps a 2D vector to a specified range.
         *
         * @tparam Tx The type of the coordinates of the input vector.
         * @tparam Ty The type of the coordinates of the minimum vector.
         * @tparam Tz The type of the coordinates of the maximum vector.
         *
         * @param value The input vector to be clamped.
         * @param min The minimum vector specifying the lower bounds of the range.
         * @param max The maximum vector specifying the upper bounds of the range.
         *
         * @return The clamped vector.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto Clamp(const Vec2D<Tx>& value, const Vec2D<Ty>& min, const Vec2D<Tz>& max)
        {
            return Vec2D<decltype(Math::Clamp(value[0], min[0], max[0]))>{ Math::Clamp(value[0], min[0], max[0]),
                                                                           Math::Clamp(value[1], min[1], max[1]) };
        }

        /**
         * @brief Calculates the reflection of a 2D vector off a normal vector.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param vector The input vector to be reflected.
         * @param normal The normal vector.
         *
         * @return The reflected vector.
         *
         * @note The formula used to calculate the reflected vector is:
         * @code{.cpp}
         * reflected_vector = vector - 2 * dot(vector, normal) * normal
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Reflect(const Vec2D<Tx>& vector, const Vec2D<Ty>& normal)
        {
            return vector - 2 * Dot(vector, normal) * normal;
        }

        /**
         * @brief Calculates the angle between two 2D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first vector.
         * @param rhs The second vector.
         *
         * @return The angle between the two vectors in radians.
         *
         * @note The formula used to calculate the angle is:
         * @code{.cpp}
         * angle = acos(dot(lhs, rhs) / (|lhs| * |rhs|))
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Angle(const Vec2D<Tx>& lhs, const Vec2D<Ty>& rhs)
        {
            const auto magnitudeProduct = Magnitude(lhs) * Magnitude(rhs);
            if (magnitudeProduct < Epsilon<Double128>)
                return 0;

            return Math::Acos(Dot(lhs, rhs) / magnitudeProduct);
        }

        /**
         * @brief Calculates the signed angle between two 2D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param from The source vector.
         * @param to The target vector.
         *
         * @return The signed angle between the two vectors.
         *
         * @note The signed angle is the angle from -> to, in radians, positive for counter-clockwise angles
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto AngleSigned(const Vec2D<Tx>& from, const Vec2D<Ty>& to)
        {
            return Math::Sign(from[0] * to[1] - from[1] * to[0]) * Angle(from, to);
        }

        /**
         * @brief Calculates the smooth step interpolation of two 2D vectors.
         *
         * @tparam Tx The type of the coordinates of the minimum bounds vector.
         * @tparam Ty The type of the coordinates of the maximum bounds vector.
         * @tparam Tz The type of the coordinates of the value vector.
         *
         * @param min The minimum bounds vector.
         * @param max The maximum bounds vector.
         * @param value The value vector.
         *
         * @return The smooth step interpolated vector.
         *
         * @note The formula used to calculate the smooth step interpolation is:
         * @code{.cpp}
         * smooth_step_interpolation = t * t * (3 - 2 * t)
         * @endcode
         * where t is the normalized value between the minimum and maximum bounds. The formula is used for each coordinate.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto SmoothStep(const Vec2D<Tx>& min, const Vec2D<Ty>& max, const Vec2D<Tz>& value)
        {
            return Vec2D<decltype(Math::SmoothStep(min[0], max[0], value[0]))>{
                Math::SmoothStep(min[0], max[0], value[0]),
                Math::SmoothStep(min[1], max[1], value[1])
            };
        }

        /**
         * @brief Calculates the inverse smooth step value of a given smoothened 2D vector between a minimum and
         * maximum bound.
         *
         * @tparam Tx The type of the coordinates of the minimum bounds vector.
         * @tparam Ty The type of the coordinates of the maximum bounds vector.
         * @tparam Tz The type of the coordinates of the smoothened value vector.
         *
         * @param min The minimum bounds vector.
         * @param max The maximum bounds vector.
         * @param smoothenedValue The smoothened vector.
         *
         * @return The calculated inverse smooth step 2D vector.
         *
         * @note The inverse smooth step formula is:
         * @code{.cpp}
         * inverse_smooth_step = min + t * t * (3 - 2 * t) * (max - min)
         * @endcode
         * If the minimum and maximum bounds are approximately equal, it returns the minimum bound. The formula
         * is used for each coordinate.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto InverseSmoothStep(const Vec2D<Tx>& min,
                                                 const Vec2D<Ty>& max,
                                                 const Vec2D<Tz>& smoothenedValue)
        {
            return Vec2D<decltype(Math::InverseSmoothStep(min, max, smoothenedValue[0]))>{
                Math::InverseSmoothStep(min[0], max[0], smoothenedValue[0]),
                Math::InverseSmoothStep(min[1], max[1], smoothenedValue[1])
            };
        }

        /**
         * @brief Move towards a target 2D vector with a maximum distance delta.
         *
         * @tparam Tx The type of the coordinates of the current vector.
         * @tparam Ty The type of the coordinates of the target vector.
         * @tparam Tz Type of the maximum distance delta.
         *
         * @param current The current vector.
         * @param target The target vector.
         * @param maxDistanceDelta The maximum distance that can be covered in one step.
         *
         * @return A new vector that moves towards the target vector by the maximum distance delta.
         *
         * @note If the current vector is approximately equal to the target, the target is returned.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto MoveTowards(const Vec2D<Tx>& current, const Vec2D<Ty>& target, Tz maxDistanceDelta)
        {
            if (AreApproximatelyEqual(current, target))
                return target;

            const auto difference       = { target[0] - current[0],
                                            target[1] - current[1] };
            const auto magnitudeSquared = MagnitudeSquared(difference);

            if (Math::IsApproximatelyZero(magnitudeSquared))
                return target;

            const auto distance = Math::SquareRoot(magnitudeSquared);
            if (distance >= maxDistanceDelta)
                return target;

            const auto factor = maxDistanceDelta / distance;

            return Vec2D<decltype(current[0] + difference[0] * factor)>{
                current[0] + difference[0] * factor,
                current[1] + difference[1] * factor
            };
        }

        /**
         * @brief Function to rotate a 2D vector towards a target vector by a specified angle.
         *
         * @tparam Tx The type of the coordinates of the current vector.
         * @tparam Ty The type of the coordinates of the target vector.
         * @tparam Tz Type of the maximum delta angle.
         *
         * @param current The current vector.
         * @param target The target vector to rotate towards.
         * @param maxDeltaAngle The maximum change in angle allowed.
         * @param angleType The type of angle (radians or degrees). Defaults to radians.
         *
         * @return The rotated vector.
         *
         * @note If the current vector is approximately equal to the target vector, the target vector is returned.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto RotateTowards(const Vec2D<Tx>& current,
                                             const Vec2D<Ty>& target,
                                             Tz maxDeltaAngle,
                                             AngleType angleType = AngleType::Radians)
        {
            if (AreApproximatelyEqual(current, target))
                return target;

            if (angleType == AngleType::Degrees)
                maxDeltaAngle = Math::DegToRad(maxDeltaAngle);

            const auto currentAngle = Math::Atan2(current.y, current.x);
            const auto targetAngle  = Math::Atan2(target.y, target.x);
            const auto deltaAngle   = Math::Clamp(Math::NormaliseAngle(targetAngle - currentAngle),
                                                  -maxDeltaAngle,
                                                  maxDeltaAngle);

            const auto rotatedAngle = currentAngle + deltaAngle;
            const auto length       = Magnitude(current);

            return Vec2D<decltype(length * Math::Cos(rotatedAngle))>{
                length * Math::Cos(rotatedAngle),
                length * Math::Sin(rotatedAngle)
            };
        }
    };

    /**
     * @brief This class provides utility functions for working with 3D vectors.
     */
    class Vector3DUtils final
    {
        template<AnyNumber TNumber>
        using Vec3D = Vector<3, TNumber>;

    public:
        /**
         * @brief Checks whether a 3D vector is approximately zero.
         *
         * @tparam TNumber The type of the coordinates of the vector.
         *
         * @param vector The vector to be checked.
         *
         * @return True if the vector is approximately zero, false otherwise.
         */
        template<AnyNumber TNumber>
        OTR_INLINE static bool IsApproximatelyZero(const Vec3D<TNumber>& vector)
        {
            if constexpr (IntegerNumber<TNumber>)
                return vector[0] == 0 && vector[1] == 0 && vector[2] == 0;

            return Math::IsApproximatelyZero(vector[0])
                   && Math::IsApproximatelyZero(vector[1])
                   && Math::IsApproximatelyZero(vector[2]);
        }

        /**
         * @brief Checks whether two 3D vectors are approximately equal.
         *
         * @tparam Tx The type of the first vector's coordinates.
         * @tparam Ty The type of the second vector's coordinates.
         *
         * @param lhs The left vector.
         * @param rhs The right vector.
         *
         * @return True if the vectors are approximately equal, false otherwise.
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static bool AreApproximatelyEqual(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs)
        {
            if constexpr (IntegerNumber<Tx> && IntegerNumber<Ty>)
                return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2];

            return Math::AreApproximatelyEqual(lhs[0], rhs[0])
                   && Math::AreApproximatelyEqual(lhs[1], rhs[1])
                   && Math::AreApproximatelyEqual(lhs[2], rhs[2]);
        }

        /**
         * @brief Calculates the squared magnitude of a 3D vector.
         *
         * @tparam TNumber The type of the coordinates of the vector.
         *
         * @param vector The 3D vector for which the squared magnitude will be calculated.
         *
         * @return The squared magnitude of the given vector.
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto MagnitudeSquared(const Vec3D<TNumber>& vector)
        {
            return Math::Square(vector[0]) + Math::Square(vector[1]) + Math::Square(vector[2]);
        }

        /**
         * @brief Calculates the magnitude of a 3D vector.
         *
         * @tparam TNumber The type of the coordinates of the vector.
         *
         * @param vector The 3D vector for which the magnitude will be calculated.
         *
         * @return The magnitude of the given vector.
         *
         * @note The formula used to calculate the magnitude is:
         * @code{.cpp}
         * magnitude = sqrt(x^2 + y^2 + z^2)
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Magnitude(const Vec3D<TNumber>& vector)
        {
            return Math::SquareRoot(MagnitudeSquared(vector));
        }

        /**
         * @brief This function clamps the magnitude of a 3D vector.
         *
         * @tparam TNumber The type of the coordinates of the vector.
         * @tparam TMaxMagnitude The type of the maximum magnitude.
         *
         * @param vector The 3D vector.
         * @param maxMagnitude The maximum magnitude.
         *
         * @return The clamped vector.
         */
        template<AnyNumber TNumber, AnyNumber TMaxMagnitude>
        OTR_INLINE static auto ClampMagnitude(const Vec3D<TNumber>& vector, const TMaxMagnitude& maxMagnitude)
        {
            const auto magnitudeSquared = MagnitudeSquared(vector);
            if (magnitudeSquared > Math::Square(maxMagnitude))
                return vector * Math::Min(maxMagnitude / Math::SquareRoot(magnitudeSquared), 1);

            return vector;
        }

        /**
         * @brief Normalizes a 3D vector.
         *
         * @tparam TNumber The type of the coordinates of the vector.
         *
         * @param vector The 3D vector to be normalized.
         *
         * @return The normalized vector.
         *
         * @note The formula used to normalize the vector is:
         * @code{.cpp}
         * normalized_vector = vector / |vector|
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Normalise(const Vec3D<TNumber>& vector)
        {
            const auto magnitude = Magnitude(vector);
            if (magnitude < Epsilon<Double128>)
                return Vec3D<TNumber>::Zero();

            return vector / magnitude;
        }

        /**
         * @brief Calculates the dot product of two 3D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first 3D vector in the dot product calculation.
         * @param rhs The second 3D vector in the dot product calculation.
         *
         * @return The dot product of the two input vectors.
         *
         * @note The formula used to calculate the dot product is:
         * @code{.cpp}
         * dot_product = lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Dot(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs)
        {
            return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2];
        }

        /**
         * @brief Calculates the cross product of two 3D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first input vector.
         * @param rhs The second input vector.
         *
         * @return The cross product of the two input vectors.
         *
         * @note The formula used to calculate the cross product is:
         * @code{.cpp}
         * cross_product = (lhs.y * rhs.z - lhs.z * rhs.y,
         *                  lhs.z * rhs.x - lhs.x * rhs.z,
         *                  lhs.x * rhs.y - lhs.y * rhs.x)
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Cross(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs)
        {
            return Vec3D<decltype(lhs[0] * rhs[0] - lhs[0] * rhs[0] + lhs[0] * rhs[0])>{
                lhs[1] * rhs[2] - lhs[2] * rhs[1],
                lhs[2] * rhs[0] - lhs[0] * rhs[2],
                lhs[0] * rhs[1] - lhs[1] * rhs[0]
            };
        }

        /**
         * @brief Calculates the squared distance between two 3D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first 3D vector.
         * @param rhs The second 3D vector.
         *
         * @return The squared distance between the two vectors.
         *
         * @note The formula used to calculate the squared distance is:
         * @code{.cpp}
         * distance_squared = (lhs.x - rhs.x)^2 + (lhs.y - rhs.y)^2 + (lhs.z - rhs.z)^2
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto DistanceSquared(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs)
        {
            return MagnitudeSquared(lhs - rhs);
        }

        /**
         * @brief Calculates the distance between two 3D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first 3D vector.
         * @param rhs The second 3D vector.
         *
         * @return The distance between the two vectors.
         *
         * @note The formula used to calculate the distance is:
         * @code{.cpp}
         * distance = sqrt((lhs.x - rhs.x)^2 + (lhs.y - rhs.y)^2 + (lhs.z - rhs.z)^2)
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Distance(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs)
        {
            return Magnitude(lhs - rhs);
        }

        /**
         * @brief Performs linear interpolation between two 3D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         * @tparam Tz The type of the linear interpolation parameter.
         *
         * @param lhs The first vector.
         * @param rhs The second vector.
         * @param t The linear interpolation parameter.
         *
         * @return The interpolated vector.
         *
         * @note The formula used for linear interpolation is:
         * @code{.cpp}
         * interpolated_vector = lhs + (rhs - lhs) * t
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto Lerp(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs, Tz t)
        {
            return lhs + (rhs - lhs) * t;
        }

        /**
         * @brief Performs clamped linear interpolation between two 3D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         * @tparam Tz The type of the linear interpolation parameter.
         *
         * @param lhs The first vector.
         * @param rhs The second vector.
         * @param t The linear interpolation parameter (clamped between 0 and 1).
         *
         * @return The interpolated vector.
         *
         * @note The formula used for linear interpolation is:
         * @code{.cpp}
         * interpolated_vector = lhs + (rhs - lhs) * t
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto LerpClamped(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs, Tz t)
        {
            return Lerp(lhs, rhs, Math::Clamp(t, (Tz) 0.0, (Tz) 1.0));
        }

        /**
         * @brief Performs spherical linear interpolation between two 3D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         * @tparam Tz The type of interpolation parameter.
         *
         * @param lhs The first vector.
         * @param rhs The second vector.
         * @param t The interpolation parameter.
         *
         * @return The result of the interpolation.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto Slerp(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs, Tz t)
        {
            auto dotClamped = Math::Clamp(Dot(lhs, rhs), -1.0, 1.0);
            auto theta      = Math::Acos(dotClamped) * t;
            auto relative   = Normalise(rhs - lhs * dotClamped);

            return ((lhs * Math::Cos(theta)) + (relative * Math::Sin(theta)));
        }

        /**
         * @brief Performs spherical linear interpolation between two 3D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         * @tparam Tz The type of linear interpolation parameter.
         *
         * @param lhs The first vector.
         * @param rhs The second vector.
         * @param t The linear interpolation parameter (clamped between 0 and 1).
         *
         * @return The result of the interpolation.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto SlerpClamped(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs, Tz t)
        {
            return Slerp(lhs, rhs, Math::Clamp(t, (Tz) 0.0, (Tz) 1.0));
        }

        /**
         * @brief Calculates the coordinate-wise maximum of two 3D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first input vector.
         * @param rhs The second input vector.
         *
         * @return The coordinate-wise maximum of the two input vectors.
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Max(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs)
        {
            return Vec3D<decltype(Math::Max(lhs[0], rhs[0]))>{ Math::Max(lhs[0], rhs[0]),
                                                               Math::Max(lhs[1], rhs[1]),
                                                               Math::Max(lhs[2], rhs[2]) };
        }

        /**
         * @brief Calculates the coordinate-wise minimum of two 3D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first input vector.
         * @param rhs The second input vector.
         *
         * @return The coordinate-wise minimum of the two input vectors.
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Min(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs)
        {
            return Vec3D<decltype(Math::Min(lhs[0], rhs[0]))>{ Math::Min(lhs[0], rhs[0]),
                                                               Math::Min(lhs[1], rhs[1]),
                                                               Math::Min(lhs[2], rhs[2]) };
        }

        /**
         * @brief Clamps a 3D vector to a specified range.
         *
         * @tparam Tx The type of the coordinates of the input vector.
         * @tparam Ty The type of the coordinates of the minimum vector.
         * @tparam Tz The type of the coordinates of the maximum vector.
         *
         * @param value The input vector to be clamped.
         * @param min The minimum vector specifying the lower bounds of the range.
         * @param max The maximum vector specifying the upper bounds of the range.
         *
         * @return The clamped vector.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto Clamp(const Vec3D<Tx>& value, const Vec3D<Ty>& min, const Vec3D<Tz>& max)
        {
            return Vec3D<decltype(Math::Clamp(value[0], min[0], max[0]))>{ Math::Clamp(value[0], min[0], max[0]),
                                                                           Math::Clamp(value[1], min[1], max[1]),
                                                                           Math::Clamp(value[2], min[2], max[2]) };
        }

        /**
         * @brief Calculates the reflection of a 3D vector off a normal vector.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param vector The input vector to be reflected.
         * @param normal The normal vector.
         *
         * @return The reflected vector.
         *
         * @note The formula used to calculate the reflected vector is:
         * @code{.cpp}
         * reflected_vector = vector - 2 * dot(vector, normal) * normal
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Reflect(const Vec3D<Tx>& vector, const Vec3D<Ty>& normal)
        {
            return vector - 2 * Dot(vector, normal) * normal;
        }

        /**
         * @brief Projects a 3D vector onto a given normal vector
         *
         * @tparam Tx The type of the coordinates of the vector.
         * @tparam Ty The type of the coordinates of the normal vector.
         *
         * @param vector The vector to be projected
         * @param normal The normal vector onto which the projection is performed
         *
         * @return The projected vector
         *
         * @note The formula used to calculate the projected vector is:
         * @code{.cpp}
         * projected_vector = dot(vector, normal) / |normal| * normal
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Project(const Vec3D<Tx>& vector, const Vec3D<Ty>& normal)
        {
            const auto magnitudeSquared = MagnitudeSquared(normal);
            if (magnitudeSquared < Epsilon<Double128>)
                return Vec3D<decltype(vector[0] * normal[0])>::Zero();

            return (Dot(vector, normal) / magnitudeSquared) * normal;
        }

        /**
         * @brief Calculates the angle between two 3D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first vector.
         * @param rhs The second vector.
         *
         * @return The angle between the two vectors in radians.
         *
         * @note The formula used to calculate the angle is:
         * @code{.cpp}
         * angle = acos(dot(lhs, rhs) / (|lhs| * |rhs|))
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Angle(const Vec3D<Tx>& lhs, const Vec3D<Ty>& rhs)
        {
            const auto magnitudeProduct = Magnitude(lhs) * Magnitude(rhs);
            if (magnitudeProduct < Epsilon<Double128>)
                return 0;

            return Math::Acos(Dot(lhs, rhs) / magnitudeProduct);
        }

        /**
         * @brief Calculates the signed angle between two 3D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param from The source vector.
         * @param to The target vector.
         *
         * @return The signed angle between the two vectors.
         *
         * @note The signed angle is the angle from -> to, in radians, positive for counter-clockwise angles
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto AngleSigned(const Vec3D<Tx>& from, const Vec3D<Ty>& to, const Vec3D<Ty>& axis)
        {
            return Math::Sign(Dot(axis, Cross(from, to))) * Angle(from, to);
        }

        /**
         * @brief Calculates the smooth step interpolation of two 3D vectors.
         *
         * @tparam Tx The type of the coordinates of the minimum bounds vector.
         * @tparam Ty The type of the coordinates of the maximum bounds vector.
         * @tparam Tz The type of the coordinates of the value vector.
         *
         * @param min The minimum bounds vector.
         * @param max The maximum bounds vector.
         * @param value The value vector.
         *
         * @return The smooth step interpolated vector.
         *
         * @note The formula used to calculate the smooth step interpolation is:
         * @code{.cpp}
         * smooth_step_interpolation = t * t * (3 - 2 * t)
         * @endcode
         * where t is the normalized value between the minimum and maximum bounds. The formula is used for each coordinate.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto SmoothStep(const Vec3D<Tx>& min, const Vec3D<Ty>& max, const Vec3D<Tz>& value)
        {
            return Vec3D<decltype(Math::SmoothStep(min[0], max[0], value[0]))>{
                Math::SmoothStep(min[0], max[0], value[0]),
                Math::SmoothStep(min[1], max[1], value[1]),
                Math::SmoothStep(min[2], max[2], value[2])
            };
        }

        /**
         * @brief Calculates the inverse smooth step value of a given smoothened 3D vector between a minimum and
         * maximum bound.
         *
         * @tparam Tx The type of the coordinates of the minimum bounds vector.
         * @tparam Ty The type of the coordinates of the maximum bounds vector.
         * @tparam Tz The type of the coordinates of the smoothened value vector.
         *
         * @param min The minimum bounds vector.
         * @param max The maximum bounds vector.
         * @param smoothenedValue The smoothened vector.
         *
         * @return The calculated inverse smooth step 3D vector.
         *
         * @note The inverse smooth step formula is:
         * @code{.cpp}
         * inverse_smooth_step = min + t * t * (3 - 2 * t) * (max - min)
         * @endcode
         * If the minimum and maximum bounds are approximately equal, it returns the minimum bound. The formula
         * is used for each coordinate.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static constexpr auto InverseSmoothStep(const Vec3D<Tx>& min,
                                                           const Vec3D<Ty>& max,
                                                           const Vec3D<Tz>& smoothenedValue)
        {
            return Vec3D<decltype(Math::InverseSmoothStep(min, max, smoothenedValue[0]))>{
                Math::InverseSmoothStep(min[0], max[0], smoothenedValue[0]),
                Math::InverseSmoothStep(min[1], max[1], smoothenedValue[1]),
                Math::InverseSmoothStep(min[2], max[2], smoothenedValue[2])
            };
        }

        /**
         * @brief Move towards a target 3D vector with a maximum distance delta.
         *
         * @tparam Tx The type of the coordinates of the current vector.
         * @tparam Ty The type of the coordinates of the target vector.
         * @tparam Tz Type of the maximum distance delta.
         *
         * @param current The current vector.
         * @param target The target vector.
         * @param maxDistanceDelta The maximum distance that can be covered in one step.
         *
         * @return A new vector that moves towards the target vector by the maximum distance delta.
         *
         * @note If the current vector is approximately equal to the target, the target is returned.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto MoveTowards(const Vec3D<Tx>& current, const Vec3D<Ty>& target, Tz maxDistanceDelta)
        {
            if (AreApproximatelyEqual(current, target))
                return target;

            const auto difference       = { target[0] - current[0],
                                            target[1] - current[1],
                                            target[2] - current[2] };
            const auto magnitudeSquared = MagnitudeSquared(difference);

            if (Math::IsApproximatelyZero(magnitudeSquared))
                return target;

            const auto distance = Math::SquareRoot(magnitudeSquared);
            if (distance <= maxDistanceDelta)
                return target;

            const auto factor = maxDistanceDelta / distance;

            return Vec3D<decltype(current[0] + difference[0] * factor)>{
                current[0] + difference[0] * factor,
                current[1] + difference[1] * factor,
                current[2] + difference[2] * factor
            };
        }

        /**
         * @brief Function to rotate a 3D vector towards a target vector by a specified angle.
         *
         * @tparam Tx The type of the coordinates of the current vector.
         * @tparam Ty The type of the coordinates of the target vector.
         * @tparam Tz Type of the maximum delta angle.
         *
         * @param current The current vector.
         * @param target The target vector to rotate towards.
         * @param maxDeltaAngle The maximum change in angle allowed.
         * @param angleType The type of angle (radians or degrees). Defaults to radians.
         *
         * @return The rotated vector.
         *
         * @note If the current vector is approximately equal to the target vector, the target vector is returned.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto RotateTowards(const Vec3D<Tx>& current,
                                             const Vec3D<Ty>& target,
                                             Tz maxDeltaAngle,
                                             AngleType angleType = AngleType::Radians)
        {
            if (AreApproximatelyEqual(current, target))
                return target;

            if (angleType == AngleType::Degrees)
                maxDeltaAngle = Math::DegToRad(maxDeltaAngle);

            const auto axis  = Cross(current, target);
            const auto angle = Math::Acos(Math::Clamp(Dot(current, target), -1.0, 1.0));
            maxDeltaAngle = Math::Clamp(maxDeltaAngle, -angle, maxDeltaAngle);

            const auto sinHalfAngle = std::sin(maxDeltaAngle / 2.0);
            const auto cosHalfAngle = std::cos(maxDeltaAngle / 2.0);

            return Vec3D<decltype(current[0] * cosHalfAngle + axis[0] * sinHalfAngle)>{
                cosHalfAngle * current[0] + sinHalfAngle * axis[0],
                cosHalfAngle * current[1] + sinHalfAngle * axis[1],
                cosHalfAngle * current[2] + sinHalfAngle * axis[2]
            };
        }

        /**
         * @brief Projects a 3D vector onto a plane defined by its normal.
         *
         * @tparam Tx The type of the coordinates of the vector.
         * @tparam Ty The type of the coordinates of the plane normal.
         *
         * @param vector The input vector to be projected onto the plane.
         * @param planeNormal The normal vector of the plane onto which the input vector is projected.
         *
         * @return The projected vector onto the plane.
         *
         * @note The formula used to calculate the projected vector is:
         * @code{.cpp}
         * projected_vector = vector - dot(vector, plane_normal) * plane_normal
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto ProjectOnPlane(const Vec3D<Tx>& vector, const Vec3D<Ty>& planeNormal)
        {
            if (IsApproximatelyZero(vector) || IsApproximatelyZero(planeNormal))
                return vector;

            if (AreApproximatelyEqual(vector, planeNormal))
                return Vec3D<decltype(vector[0] - planeNormal[0])>::Zero();

            planeNormal = Normalise(planeNormal);
            const auto dot = Dot(vector, planeNormal);

            return Vec3D<decltype(vector[0] - dot * planeNormal[0])>{
                vector[0] - dot * planeNormal[0],
                vector[1] - dot * planeNormal[1],
                vector[2] - dot * planeNormal[2]
            };
        }

        /**
         * @brief Takes three vectors (normal, tangent, biNormal) by reference and modifies them to be orthogonal
         * and normalized.
         *
         * @tparam Tx The type of the coordinates of the normal vector
         * @tparam Ty The type of the coordinates of the tangent vector
         * @tparam Tz The type of the coordinates of the biNormal vector
         *
         * @param normal A pointer to the normal vector
         * @param tangent A pointer to the tangent vector
         * @param biNormal A pointer to the biNormal vector
         */
        template<FloatingPointNumber Tx, FloatingPointNumber Ty, FloatingPointNumber Tz>
        OTR_INLINE static void OrthoNormalise(Vec3D<Tx>* normal, Vec3D<Ty>* tangent, Vec3D<Tz>* biNormal)
        {
            if (!normal || !tangent)
                return;

            if (normal == Vec3D<Tx>::Zero() || tangent == Vec3D<Ty>::Zero())
            {
                *normal  = Vec3D<Tx>::Zero();
                *tangent = Vec3D<Ty>::Zero();

                if (biNormal)
                    *biNormal = Vec3D<Tz>::Zero();

                return;
            }

            auto normalDeref   = *normal;
            auto tangentDeref  = *tangent;
            auto biNormalDeref = biNormal ? *biNormal : Vec3D<Tz>::Zero();

            // Step 1: Normal
            normalDeref  = Normalise(normalDeref);

            if (!biNormal || biNormal == Vec3D<Ty>::Zero())
            {
                // Step 2: BiNormal
                biNormalDeref = Cross(normalDeref, tangentDeref);
                // Step 3: Tangent
                tangentDeref  = Cross(biNormalDeref, normalDeref);

                normal   = &normalDeref;
                tangent  = &tangentDeref;
                biNormal = &biNormalDeref;

                return;
            }

            // Step 2: Tangent
            tangentDeref -= normalDeref * Dot(normalDeref, tangentDeref);
            tangentDeref = Normalise(tangentDeref);

            // Step 3: BiNormal
            const auto cross = Cross(normalDeref, tangentDeref);
            biNormalDeref -= normalDeref * Dot(normalDeref, biNormalDeref);
            biNormalDeref -= tangentDeref * Dot(tangentDeref, biNormalDeref);
            biNormalDeref -= cross * Dot(cross, biNormalDeref);
            biNormalDeref    = Normalise(biNormalDeref);

            normal   = &normalDeref;
            tangent  = &tangentDeref;
            biNormal = &biNormalDeref;
        }
    };

    /**
     * @brief This class provides utility functions for working with 4D vectors.
     */
    class Vector4DUtils final
    {
        template<AnyNumber TNumber>
        using Vec4D = Vector<4, TNumber>;

    public:
        /**
         * @brief Checks whether a 4D vector is approximately zero.
         *
         * @tparam TNumber The type of the coordinates of the vector.
         *
         * @param vector The vector to be checked.
         *
         * @return True if the vector is approximately zero, false otherwise.
         */
        template<AnyNumber TNumber>
        OTR_INLINE static bool IsApproximatelyZero(const Vec4D<TNumber>& vector)
        {
            if constexpr (IntegerNumber<TNumber>)
                return vector[0] == 0 && vector[1] == 0 && vector[2] == 0 && vector[3] == 0;

            return Math::IsApproximatelyZero(vector[0])
                   && Math::IsApproximatelyZero(vector[1])
                   && Math::IsApproximatelyZero(vector[2])
                   && Math::IsApproximatelyZero(vector[3]);
        }

        /**
         * @brief Checks whether two 4D vectors are approximately equal.
         *
         * @tparam Tx The type of the first vector's coordinates.
         * @tparam Ty The type of the second vector's coordinates.
         *
         * @param lhs The left vector.
         * @param rhs The right vector.
         *
         * @return True if the vectors are approximately equal, false otherwise.
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static bool AreApproximatelyEqual(const Vec4D<Tx>& lhs, const Vec4D<Ty>& rhs)
        {
            if constexpr (IntegerNumber<Tx> && IntegerNumber<Ty>)
                return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2] && lhs[3] == rhs[3];

            return Math::AreApproximatelyEqual(lhs[0], rhs[0])
                   && Math::AreApproximatelyEqual(lhs[1], rhs[1])
                   && Math::AreApproximatelyEqual(lhs[2], rhs[2])
                   && Math::AreApproximatelyEqual(lhs[3], rhs[3]);
        }

        /**
         * @brief Calculates the squared magnitude of a 4D vector.
         *
         * @tparam TNumber The type of the coordinates of the vector.
         *
         * @param vector The 4D vector for which the squared magnitude will be calculated.
         *
         * @return The squared magnitude of the given vector.
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto MagnitudeSquared(const Vec4D<TNumber>& vector)
        {
            return Math::Square(vector[0]) + Math::Square(vector[1]) + Math::Square(vector[2]);
        }

        /**
         * @brief Calculates the magnitude of a 4D vector.
         *
         * @tparam TNumber The type of the coordinates of the vector.
         *
         * @param vector The 4D vector for which the magnitude will be calculated.
         *
         * @return The magnitude of the given vector.
         *
         * @note The formula used to calculate the magnitude is:
         * @code{.cpp}
         * magnitude = sqrt(x^2 + y^2 + z^2 + w^2)
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Magnitude(const Vec4D<TNumber>& vector)
        {
            return Math::SquareRoot(MagnitudeSquared(vector));
        }

        /**
         * @brief This function clamps the magnitude of a 4D vector.
         *
         * @tparam TNumber The type of the coordinates of the vector.
         * @tparam TMaxMagnitude The type of the maximum magnitude.
         *
         * @param vector The 4D vector.
         * @param maxMagnitude The maximum magnitude.
         *
         * @return The clamped vector.
         */
        template<AnyNumber TNumber, AnyNumber TMaxMagnitude>
        OTR_INLINE static auto ClampMagnitude(const Vec4D<TNumber>& vector, const TMaxMagnitude& maxMagnitude)
        {
            const auto magnitudeSquared = MagnitudeSquared(vector);
            if (magnitudeSquared > Math::Square(maxMagnitude))
                return vector * Math::Min(maxMagnitude / Math::SquareRoot(magnitudeSquared), 1);

            return vector;
        }

        /**
         * @brief Normalizes a 4D vector.
         *
         * @tparam TNumber The type of the coordinates of the vector.
         *
         * @param vector The 4D vector to be normalized.
         *
         * @return The normalized vector.
         *
         * @note The formula used to normalize the vector is:
         * @code{.cpp}
         * normalized_vector = vector / |vector|
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Normalise(const Vec4D<TNumber>& vector)
        {
            const auto magnitude = Magnitude(vector);
            if (magnitude < Epsilon<Double128>)
                return Vec4D<TNumber>::Zero();

            return vector / magnitude;
        }

        /**
         * @brief Calculates the dot product of two 4D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first 4D vector in the dot product calculation.
         * @param rhs The second 4D vector in the dot product calculation.
         *
         * @return The dot product of the two input vectors.
         *
         * @note The formula used to calculate the dot product is:
         * @code{.cpp}
         * dot_product = lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z + lhs.w * rhs.w
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Dot(const Vec4D<Tx>& lhs, const Vec4D<Ty>& rhs)
        {
            return lhs[0] * rhs[0] + lhs[1] * rhs[1] + lhs[2] * rhs[2] + lhs[3] * rhs[3];
        }

        /**
         * @brief Calculates the squared distance between two 4D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first 4D vector.
         * @param rhs The second 4D vector.
         *
         * @return The squared distance between the two vectors.
         *
         * @note The formula used to calculate the squared distance is:
         * @code{.cpp}
         * distance_squared = (lhs.x - rhs.x)^2 + (lhs.y - rhs.y)^2 + (lhs.z - rhs.z)^2 + (lhs.w - rhs.w)^2
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto DistanceSquared(const Vec4D<Tx>& lhs, const Vec4D<Ty>& rhs)
        {
            return MagnitudeSquared(lhs - rhs);
        }

        /**
         * @brief Calculates the distance between two 4D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first 4D vector.
         * @param rhs The second 4D vector.
         *
         * @return The distance between the two vectors.
         *
         * @note The formula used to calculate the distance is:
         * @code{.cpp}
         * distance = sqrt((lhs.x - rhs.x)^2 + (lhs.y - rhs.y)^2 + (lhs.z - rhs.z)^2 + (lhs.w - rhs.w)^2)
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Distance(const Vec4D<Tx>& lhs, const Vec4D<Ty>& rhs)
        {
            return Magnitude(lhs - rhs);
        }

        /**
         * @brief Performs linear interpolation between two 4D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         * @tparam Tz The type of the linear interpolation parameter.
         *
         * @param lhs The first vector.
         * @param rhs The second vector.
         * @param t The linear interpolation parameter.
         *
         * @return The interpolated vector.
         *
         * @note The formula used for linear interpolation is:
         * @code{.cpp}
         * interpolated_vector = lhs + (rhs - lhs) * t
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto Lerp(const Vec4D<Tx>& lhs, const Vec4D<Ty>& rhs, Tz t)
        {
            return lhs + (rhs - lhs) * t;
        }

        /**
         * @brief Performs clamped linear interpolation between two 4D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         * @tparam Tz The type of the linear interpolation parameter.
         *
         * @param lhs The first vector.
         * @param rhs The second vector.
         * @param t The linear interpolation parameter.
         *
         * @return The interpolated vector.
         *
         * @note The formula used for linear interpolation is:
         * @code{.cpp}
         * interpolated_vector = lhs + (rhs - lhs) * t
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto LerpClamped(const Vec4D<Tx>& lhs, const Vec4D<Ty>& rhs, Tz t)
        {
            return Lerp(lhs, rhs, Math::Clamp(t, (Tz) 0.0, (Tz) 1.0));
        }

        /**
         * @brief Calculates the coordinate-wise maximum of two 4D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first input vector.
         * @param rhs The second input vector.
         *
         * @return The coordinate-wise maximum of the two input vectors.
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Max(const Vec4D<Tx>& lhs, const Vec4D<Ty>& rhs)
        {
            return Vec4D<decltype(Math::Max(lhs[0], rhs[0]))>{ Math::Max(lhs[0], rhs[0]),
                                                               Math::Max(lhs[1], rhs[1]),
                                                               Math::Max(lhs[2], rhs[2]),
                                                               Math::Max(lhs[3], rhs[3]) };
        }

        /**
         * @brief Calculates the coordinate-wise minimum of two 4D vectors.
         *
         * @tparam Tx The type of the coordinates of the first vector.
         * @tparam Ty The type of the coordinates of the second vector.
         *
         * @param lhs The first input vector.
         * @param rhs The second input vector.
         *
         * @return The coordinate-wise minimum of the two input vectors.
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Min(const Vec4D<Tx>& lhs, const Vec4D<Ty>& rhs)
        {
            return Vec4D<decltype(Math::Min(lhs[0], rhs[0]))>{ Math::Min(lhs[0], rhs[0]),
                                                               Math::Min(lhs[1], rhs[1]),
                                                               Math::Min(lhs[2], rhs[2]),
                                                               Math::Min(lhs[3], rhs[3]) };
        }

        /**
         * @brief Clamps a 4D vector to a specified range.
         *
         * @tparam Tx The type of the coordinates of the input vector.
         * @tparam Ty The type of the coordinates of the minimum vector.
         * @tparam Tz The type of the coordinates of the maximum vector.
         *
         * @param value The input vector to be clamped.
         * @param min The minimum vector specifying the lower bounds of the range.
         * @param max The maximum vector specifying the upper bounds of the range.
         *
         * @return The clamped vector.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto Clamp(const Vec4D<Tx>& value, const Vec4D<Ty>& min, const Vec4D<Tz>& max)
        {
            return Vec4D<decltype(Math::Clamp(value[0], min[0], max[0]))>{ Math::Clamp(value[0], min[0], max[0]),
                                                                           Math::Clamp(value[1], min[1], max[1]),
                                                                           Math::Clamp(value[2], min[2], max[2]),
                                                                           Math::Clamp(value[3], min[3], max[3]) };
        }

        /**
         * @brief Projects a 4D vector onto a given normal vector
         *
         * @tparam Tx The type of the coordinates of the vector.
         * @tparam Ty The type of the coordinates of the normal vector.
         *
         * @param vector The vector to be projected
         * @param normal The normal vector onto which the projection is performed
         *
         * @return The projected vector
         *
         * @note The formula used to calculate the projected vector is:
         * @code{.cpp}
         * projected_vector = dot(vector, normal) / |normal| * normal
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty>
        OTR_INLINE static auto Project(const Vec4D<Tx>& vector, const Vec4D<Ty>& normal)
        {
            const auto magnitudeSquared = MagnitudeSquared(normal);
            if (magnitudeSquared < Epsilon<Double128>)
                return Vec4D<decltype(vector[0] * normal[0])>::Zero();

            return (Dot(vector, normal) / magnitudeSquared) * normal;
        }

        /**
         * @brief Move towards a target 4D vector with a maximum distance delta.
         *
         * @tparam Tx The type of the coordinates of the current vector.
         * @tparam Ty The type of the coordinates of the target vector.
         * @tparam Tz Type of the maximum distance delta.
         *
         * @param current The current vector.
         * @param target The target vector.
         * @param maxDistanceDelta The maximum distance that can be covered in one step.
         *
         * @return A new vector that moves towards the target vector by the maximum distance delta.
         *
         * @note If the current vector is approximately equal to the target, the target is returned.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto MoveTowards(const Vec4D<Tx>& current, const Vec4D<Ty>& target, Tz maxDistanceDelta)
        {
            if (AreApproximatelyEqual(current, target))
                return target;

            const auto difference       = { target[0] - current[0],
                                            target[1] - current[1],
                                            target[2] - current[2],
                                            target[3] - current[3] };
            const auto magnitudeSquared = MagnitudeSquared(difference);

            if (Math::IsApproximatelyZero(magnitudeSquared))
                return target;

            const auto distance = Math::SquareRoot(magnitudeSquared);
            if (distance <= maxDistanceDelta)
                return target;

            const auto factor = maxDistanceDelta / distance;

            return Vector<3, decltype(current[0] + difference[0] * factor)>{
                current[0] + difference[0] * factor,
                current[1] + difference[1] * factor,
                current[2] + difference[2] * factor,
                current[3] + difference[3] * factor
            };
        }
    };
}

#endif //OTTERENGINE_VECTOR_H
