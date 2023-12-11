#ifndef OTTERENGINE_MATRIX_H
#define OTTERENGINE_MATRIX_H

#include "Math/Vector.h"
#include "Math/Quaternion.h"

namespace Otter
{
    namespace Math
    {
        template<UInt8 Tx, UInt8 Ty, AnyNumber TNumber>
        requires Dimension<Tx> && Dimension<Ty> && (!UnsignedNumber<TNumber>)
        struct Matrix;

        class MatrixUtils;

        class Matrix2x2Utils;

        class Matrix3x3Utils;

        class Matrix4x4Utils;
    }

    /**
     * @brief Alias for a matrix.
     *
     * @tparam Tx The number of rows.
     * @tparam Ty The number of columns.
     * @tparam TNumber The type of the values.
     */
    template<UInt8 Tx, UInt8 Ty, AnyNumber TNumber>
        requires Dimension<Tx> && Dimension<Ty> && (!UnsignedNumber<TNumber>)
    using Matrix = Math::Matrix<Tx, Ty, TNumber>;

    /**
     * @brief Alias for a 2x2 matrix.
     *
     * @tparam TNumber The type of the values.
     */
    template<AnyNumber TNumber> requires (!UnsignedNumber<TNumber>)
    using Matrix2x2 = Math::Matrix<2, 2, TNumber>;

    /**
     * @brief Alias for a 3x3 matrix.
     *
     * @tparam TNumber The type of the values.
     */
    template<AnyNumber TNumber> requires (!UnsignedNumber<TNumber>)
    using Matrix3x3 = Math::Matrix<3, 3, TNumber>;

    /**
     * @brief Alias for a 4x4 matrix.
     *
     * @tparam TNumber The type of the values.
     */
    template<AnyNumber TNumber> requires (!UnsignedNumber<TNumber>)
    using Matrix4x4 = Math::Matrix<4, 4, TNumber>;

    /**
     * @brief Alias for a NxM matrix utilities class.
     */
    using MatNxM = Math::MatrixUtils;

    /**
     * @brief Alias for a 2x2 matrix utilities class.
     */
    using Mat2x2 = Math::Matrix2x2Utils;

    /**
     * @brief Alias for a 3x3 matrix utilities class.
     */
    using Mat3x3 = Math::Matrix3x3Utils;

    /**
     * @brief Alias for a 4x4 matrix utilities class.
     */
    using Mat4x4 = Math::Matrix4x4Utils;
}

namespace Otter::Math
{
    /**
     * @brief A struct representing a matrix.
     *
     * @tparam Tx The number of rows.
     * @tparam Ty The number of columns.
     * @tparam TNumber The type of the cells.
     *
     * @note The number of rows/columns can be 2, 3 or 4.
     * @note This struct can be used directly but it would be preferred to use the Matrix2x2, Matrix3x3 and
     * Matrix4x4 aliases instead.
     */
    template<UInt8 Tx, UInt8 Ty, AnyNumber TNumber>
    requires Dimension<Tx> && Dimension<Ty> && (!UnsignedNumber<TNumber>)
    struct Matrix final
    {
        // HELP: I usually add the member variables at the bottom of the class,
        //      but it's the only member and the class is too large.
    private:
        TNumber m_Values[Tx * Ty];

    public:
        /**
         * @brief Default constructor that initializes the matrix with all cells initialized to zero.
         */
        constexpr Matrix()
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] = static_cast<TNumber>(0.0);
        }

        /**
         * @brief Constructs a matrix object with all cells initialized to the given scalar value.
         *
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param scalar The scalar value to initialize the matrix cells with.
         */
        constexpr explicit Matrix(TNumber scalar)
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] = scalar;
        }

        /**
         * @brief Constructs a matrix object with cells initialised from an initializer list.
         *
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param list The list containing the cells to initialise the matrix with.
         */
        constexpr Matrix(InitialiserList<TNumber> list)
        {
            OTR_ASSERT_MSG(list.size() == Tx * Ty, "Initialiser list size does not match Matrix size")

            UInt64 i = 0;
            for (const TNumber& value: list)
                m_Values[i++] = value;
        }

        /**
         * @brief Copy constructor for matrix objects.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param other The matrix object to copy from.
         */
        Matrix(const Matrix<Tx, Ty, TNumber>& other)
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] = other.m_Values[i];
        }

        /**
         * @brief Move constructor for matrix objects.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param other The matrix object to move from.
         */
        Matrix(Matrix<Tx, Ty, TNumber>&& other) noexcept
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] = other.m_Values[i];
        }

        /**
         * @brief Copy assignment operator for matrix objects.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param other The matrix object to copy from.
         *
         * @return A reference to the current matrix object after the copy assignment.
         */
        Matrix<Tx, Ty, TNumber>& operator=(const Matrix<Tx, Ty, TNumber>& other)
        {
            if (this == &other)
                return *this;

            for (UInt8 i = 0; i < 3; ++i)
                m_Values[i] = other.m_Values[i];

            return *this;
        }

        /**
         * @brief Move assignment operator for matrix objects.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param other The matrix object to move from.
         *
         * @return A reference to the current matrix object after the move assignment.
         */
        Matrix<Tx, Ty, TNumber>& operator=(Matrix<Tx, Ty, TNumber>&& other) noexcept
        {
            if (this == &other)
                return *this;

            for (UInt8 i = 0; i < 3; ++i)
                m_Values[i] = other.m_Values[i];

            return *this;
        }

        /**
         * @brief Accesses the cell at the specified index.
         *
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param index The index of the cell to access.
         *
         * @return A reference to the cell at the specified index.
         */
        TNumber& operator[](UInt8 index)
        {
            OTR_ASSERT_MSG(index < Tx * Ty, "Index {0} is out of range", index)
            return m_Values[index];
        }

        /**
         * @brief Accesses the cell at the specified row and column.
         *
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param x The row of the cell to access.
         * @param y The column of the cell to access.
         *
         * @return A reference to the cell at the specified index.
         */
        TNumber& operator[](UInt8 x, UInt8 y)
        {
            OTR_ASSERT_MSG(x < Tx, "Row index {0} is out of range", x)
            OTR_ASSERT_MSG(y < Ty, "Column index {0} is out of range", y)
            return m_Values[y + x * Ty];
        }

        /**
         * @brief Accesses the cell at the specified index.
         *
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param index The index of the cell to access.
         *
         * @return A reference to the cell at the specified index.
         */
        const TNumber& operator[](UInt8 index) const
        {
            OTR_ASSERT_MSG(index < Tx * Ty, "Index {0} is out of range", index)
            return m_Values[index];
        }

        /**
         * @brief Accesses the cell at the specified row and column.
         *
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param x The row of the cell to access.
         * @param y The column of the cell to access.
         *
         * @return A reference to the cell at the specified index.
         */
        const TNumber& operator[](UInt8 x, UInt8 y) const
        {
            OTR_ASSERT_MSG(x < Tx, "Row index {0} is out of range", x)
            OTR_ASSERT_MSG(y < Ty, "Column index {0} is out of range", y)
            return m_Values[y + x * Ty];
        }

        /**
         * @brief Conversion operator that allows the matrix to be converted to a different matrix type.
         *
         * @tparam Tx The number of rows of the resulting matrix.
         * @tparam Ty The number of columns of the resulting matrix.
         * @tparam TOtherNumber The type of the cells in the resulting matrix.
         *
         * @return A new matrix with cells converted to the specified type.
         */
        template<AnyNumber TOtherNumber>
        explicit operator Matrix<Tx, Ty, TOtherNumber>() const
        {
            Matrix<Tx, Ty, TOtherNumber> result;

            for (UInt8 i = 0; i < Tx * Ty; ++i)
                result[i] = static_cast<TOtherNumber>(m_Values[i]);

            return result;
        }

        /**
         * @brief Conversion operator that allows the matrix to be converted to a different matrix type.
         *
         * @tparam TOtherX The number of rows of the resulting matrix.
         * @tparam TOtherY The number of columns of the resulting matrix.
         * @tparam TOtherNumber The type of the cells in the resulting matrix.
         *
         * @return A new matrix with cells converted to the specified type (can be a different size).
         */
        template<UInt8 TOtherX, UInt8 TOtherY, AnyNumber TOtherNumber>
        requires Dimension<TOtherX> && Dimension<TOtherY>
        explicit operator Matrix<TOtherX, TOtherY, TOtherNumber>() const
        {
            Matrix<TOtherX, TOtherY, TOtherNumber> result;

            for (UInt8 j = 0; j < TOtherY; ++j)
                for (UInt8 i = 0; i < TOtherX; ++i)
                    if (i >= Tx || j >= Ty)
                        result[i, j] = static_cast<TOtherNumber>(0.0);
                    else
                        result[i, j] = static_cast<TOtherNumber>(m_Values[j + i * Ty]);

            return result;
        }

        /**
         * @brief Adds another matrix to the current matrix, cell-wise.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The type of the cells in the current matrix.
         * @tparam TOtherNumber The type of the cells in the other matrix.
         *
         * @param other The matrix to be added to the current matrix.
         *
         * @return A reference to the updated current matrix after the addition.
         */
        template<AnyNumber TOtherNumber>
        Matrix<Tx, Ty, TNumber>& operator+=(const Matrix<Tx, Ty, TOtherNumber>& other)
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] += static_cast<TNumber>(other.m_Values[i]);

            return *this;
        }

        /**
         * @brief Adds another matrix to the current matrix, cell-wise.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param other The matrix to be added to the current matrix.
         *
         * @return A reference to the updated current matrix after the addition.
         */
        Matrix<Tx, Ty, TNumber>& operator+=(const Matrix<Tx, Ty, TNumber>& other) noexcept
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] += other.m_Values[i];

            return *this;
        }

        /**
         * @brief Subtracts another matrix from the current matrix, cell-wise.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The type of the cells in the current matrix.
         * @tparam TOtherNumber The type of the cells in the other matrix.
         *
         * @param other The matrix to be subtracted from the current matrix.
         *
         * @return A reference to the updated current matrix after the subtraction.
         */
        template<AnyNumber TOtherNumber>
        Matrix<Tx, Ty, TNumber>& operator-=(const Matrix<Tx, Ty, TOtherNumber>& other)
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] -= static_cast<TNumber>(other.m_Values[i]);

            return *this;
        }

        /**
         * @brief Subtracts another matrix from the current matrix, cell-wise.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The type of the cells in the current matrix.
         *
         * @param other The matrix to be subtracted from the current matrix.
         *
         * @return A reference to the updated current matrix after the subtraction.
         */
        Matrix<Tx, Ty, TNumber>& operator-=(const Matrix<Tx, Ty, TNumber>& other) noexcept
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] -= other.m_Values[i];

            return *this;
        }

        /**
         * @brief Multiplies the current matrix by a scalar value, cell-wise.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The type of the cells in the matrix.
         * @tparam TOtherNumber The type of the scalar value.
         *
         * @param scalar The scalar value to multiply the matrix cells with.
         *
         * @return A reference to the updated current matrix after the multiplication.
         */
        template<AnyNumber TOtherNumber>
        Matrix<Tx, Ty, TNumber>& operator*=(TOtherNumber scalar) noexcept
        {
            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] *= scalar;

            return *this;
        }

        /**
         * @brief Divides the current matrix by a scalar value, cell-wise.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The type of the cells in the matrix.
         * @tparam TOtherNumber The type of the scalar value.
         *
         * @param scalar The scalar value to divide the matrix cells with.
         *
         * @return A reference to the updated current matrix after the division.
         */
        template<AnyNumber TOtherNumber>
        Matrix<Tx, Ty, TNumber>& operator/=(TOtherNumber scalar) noexcept
        {
            OTR_ASSERT_MSG(scalar != 0, "Division by zero")

            for (UInt8 i = 0; i < Tx * Ty; ++i)
                m_Values[i] /= scalar;

            return *this;
        }

        /**
         * @brief Adds the `rhs` matrix to the `lhs` matrix, cell-wise.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The type of the cells in the left matrix.
         * @tparam TOtherNumber The type of the cells in the right matrix.
         *
         * @param lhs The left-hand side matrix.
         * @param rhs The right-hand side matrix.
         *
         * @return The result of the addition.
         */
        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator+(Matrix<Tx, Ty, TNumber>& lhs, const Matrix<Tx, Ty, TOtherNumber>& rhs)
        {
            lhs += rhs;
            return lhs;
        }

        /**
         * @brief Subtracts the `rhs` matrix from the `lhs` matrix, cell-wise.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The type of the cells in the left matrix.
         * @tparam TOtherNumber The type of the cells in the right matrix.
         *
         * @param lhs The left-hand side matrix.
         * @param rhs The right-hand side matrix.
         *
         * @return The result of the subtraction.
         */
        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator-(Matrix<Tx, Ty, TNumber>& lhs, const Matrix<Tx, Ty, TOtherNumber>& rhs)
        {
            lhs -= rhs;
            return lhs;
        }

        /**
         * @brief Multiplies the `rhs` matrix with a scalar value, cell-wise.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The type of the cells in the matrix.
         * @tparam TOtherNumber The type of the scalar value.
         *
         * @param lhs The matrix.
         * @param rhs The scalar value.
         *
         * @return The result of the multiplication.
         */
        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator*(Matrix<Tx, Ty, TNumber>& lhs, TOtherNumber rhs)
        {
            lhs *= rhs;
            return lhs;
        }

        /**
         * @brief Divides the `rhs` matrix with a scalar value, cell-wise.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The type of the cells in the matrix.
         * @tparam TOtherNumber The type of the scalar value.
         *
         * @param lhs The matrix.
         * @param rhs The scalar value.
         *
         * @return The result of the division.
         */
        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator/(Matrix<Tx, Ty, TNumber>& lhs, TOtherNumber rhs)
        {
            OTR_ASSERT_MSG(rhs != 0, "Division by zero")

            lhs /= rhs;
            return lhs;
        }

        /**
         * @brief Compares the current matrix instance with another matrix instance for equality.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The type of the cells in the current matrix.
         * @tparam TOtherNumber The type of the cells in the other matrix.
         *
         * @param other The other matrix instance to compare with.
         *
         * @return true if the current matrix instance is equal to the other matrix instance,
         *         false otherwise.
         *
         * @note The comparison behavior is different based on the data types of the cells. If both TNumber and
         * TOtherNumber are integral types, the function compares each cell of the matrix using the '==' operator.
         * If either TNumber or TOtherNumber are floating-point types, the function uses the Math::AreApproximatelyEqual
         * function to compare each cell of the matrix with a specified tolerance.
         */
        template<AnyNumber TOtherNumber>
        bool operator==(const Matrix<Tx, Ty, TOtherNumber>& other) const noexcept
        {
            if constexpr (IntegerNumber<TNumber> && IntegerNumber<TOtherNumber>)
            {
                for (UInt8 i = 0; i < Tx * Ty; ++i)
                    if (m_Values[i] != other.m_Values[i])
                        return false;

                return true;
            }

            for (UInt8 i = 0; i < Tx * Ty; ++i)
                if (!Math::AreApproximatelyEqual(m_Values[i], other.m_Values[i]))
                    return false;

            return true;
        }

        /**
         * @brief Compares the current matrix instance with another matrix instance for inequality.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The type of the cells in the current matrix.
         * @tparam TOtherNumber The type of the cells in the other matrix.
         *
         * @param other The other matrix instance to compare with.
         *
         * @return true if the current matrix instance is not equal to the other matrix instance,
         *         false otherwise.
         *
         * @note The '!=' operator is implemented in terms of the '==' operator.
         */
        template<AnyNumber TOtherNumber>
        bool operator!=(const Matrix<Tx, Ty, TOtherNumber>& other) const noexcept
        {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "Simplify"

            return !(*this == other);

#pragma clang diagnostic pop
        }

        /**
         * @brief Getter for the row at a specified index.
         *
         * @tparam Ty The number of columns and the dimension of the row vector.
         * @tparam TNumber The type of the cells and the row vector coordinates.
         *
         * @param index The index of the row to get.
         *
         * @return The row vector.
         */
        [[nodiscard]] OTR_INLINE Math::Vector<Ty, TNumber> GetRow(UInt8 index) const noexcept
        {
            OTR_ASSERT_MSG(index < Ty, "Row index {0} is out of range", index)

            auto row = Vector<Ty, TNumber>();

            for (UInt8 i = 0; i < Ty; ++i)
                row[i] = m_Values[i + index * Ty];

            return row;
        }

        /**
         * @brief Setter for the row at a specified index.
         *
         * @tparam Ty The number of columns and the dimension of the row vector.
         * @tparam TNumber The type of the cells and the row vector coordinates.
         *
         * @param index The index of the row to set.
         * @param row The row vector.
         */
        OTR_INLINE void SetRow(UInt8 index, const Math::Vector<Ty, TNumber>& row) noexcept
        {
            OTR_ASSERT_MSG(index < Ty, "Row index {0} is out of range", index)

            for (UInt8 i = 0; i < Ty; ++i)
                m_Values[i + index * Ty] = row[i];
        }

        /**
         * @brief Getter for the column at a specified index.
         *
         * @tparam Tx The number of row and the dimension of the column vector.
         * @tparam TNumber The type of the cells and the column vector coordinates.
         *
         * @param index The index of the column to get.
         *
         * @return The column vector.
         */
        [[nodiscard]] OTR_INLINE Math::Vector<Tx, TNumber> GetColumn(UInt8 index) const noexcept
        {
            OTR_ASSERT_MSG(index < Tx, "Column index {0} is out of range", index)

            auto column = Vector<Tx, TNumber>();

            for (UInt8 i = 0; i < Tx; ++i)
                column[i] = m_Values[index + i * Tx];

            return column;
        }

        /**
         * @brief Setter for the column at a specified index.
         *
         * @tparam Tx The number of rows and the dimension of the column vector.
         * @tparam TNumber The type of the cells and the column vector coordinates.
         *
         * @param index The index of the column to set.
         * @param row The column vector.
         */
        OTR_INLINE void SetColumn(UInt8 index, const Math::Vector<Tx, TNumber>& column) noexcept
        {
            OTR_ASSERT_MSG(index < Tx, "Column index {0} is out of range", index)

            for (UInt8 i = 0; i < Tx; ++i)
                m_Values[index + i * Tx] = column[i];
        }

        /**
         * @brief Returns a matrix with all cells set to zero.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The cell type.
         *
         * @return The matrix with all cells set to zero.
         */
        OTR_INLINE static constexpr Matrix<Tx, Ty, TNumber> Zero()
        {
            return Matrix<Tx, Ty, TNumber>(static_cast<TNumber>(0.0));
        }

        /**
         * @brief Returns a matrix with all cells set to zero except the diagonal cells which are set to one.
         *
         * @tparam Tx The number of rows.
         * @tparam Ty The number of columns.
         * @tparam TNumber The cell type.
         *
         * @return The matrix with all cells set to zero except the diagonal cells which are set to one.
         *
         * @note The matrix must be square.
         */
        OTR_INLINE static constexpr Matrix<Tx, Ty, TNumber> Identity()
        requires Dimension<Tx>
                 && Dimension<Ty>
                 && (Tx == Ty)
        {
            Matrix<Tx, Ty, TNumber> result;

            for (UInt8 i = 0; i < Tx; ++i)
                result[i, i] = static_cast<TNumber>(1.0);

            return result;
        }
    };

    /**
     * @brief This class provides utility functions for working with NxM matrices.
     */
    class MatrixUtils final
    {
    public:
        /**
         * @brief Multiplies two matrices.
         *
         * @tparam Tx The number of rows in the first matrix.
         * @tparam Ty The number of columns in the first matrix and the number of rows in the second matrix.
         * @tparam Tz The number of columns in the second matrix.
         * @tparam TNumber The type of the cells in the first matrix.
         * @tparam TOtherNumber The type of the cells in the second matrix.
         *
         * @param m1 The first matrix of size Tx x Ty.
         * @param m2 The second matrix of size Ty x Tz.
         *
         * @return The resulting matrix of size Tx x Tz obtained by multiplying the two input matrices.
         *
         * @note The number of columns in the first matrix must be equal to the number of rows in the second matrix.
         */
        template<UInt8 Tx, UInt8 Ty, UInt8 Tz, AnyNumber TNumber, AnyNumber TOtherNumber>
        requires Dimension<Tx> && Dimension<Ty> && Dimension<Tz>
        OTR_INLINE static decltype(auto) Multiply(const Matrix<Tx, Ty, TNumber>& m1,
                                                  const Matrix<Ty, Tz, TOtherNumber>& m2)
        {
            Matrix<Tx, Tz, decltype(TNumber{ } * TOtherNumber{ })> result;

            for (int col = 0; col < Tz; col++)
                for (int row = 0; row < Tx; row++)
                    for (int inner = 0; inner < Ty; inner++)
                        result[row, col] += m1[row, inner] * m2[inner, col];

            return result;
        }
    };

    /**
     * @brief This class provides utility functions for working with 2x2 matrices.
     */
    class Matrix2x2Utils final
    {
        template<AnyNumber TNumber> requires (!UnsignedNumber<TNumber>)
        using Mat2x2 = Matrix<2, 2, TNumber>;

    public:
        /**
         * @brief Calculates the determinant of a 2x2 matrix.
         *
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param matrix The matrix.
         *
         * @return The determinant of the matrix.
         *
         * @note The formula for calculating the determinant of a 2x2 matrix is:
         * @code{.cpp}
         * | a b |
         * | c d | = a * d - b * c
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Determinant(const Mat2x2<TNumber>& matrix)
        {
            return matrix[0, 0] * matrix[1, 1] - matrix[0, 1] * matrix[1, 0];
        }

        /**
         * @brief Transposes a 2x2 matrix.
         *
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param matrix The matrix to transpose.
         *
         * @return The transposed matrix.
         *
         * @note The formula for transposing a 2x2 matrix is:
         * @code{.cpp}
         * | a b |   | a c |
         * | c d | = | b d |
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Transpose(const Mat2x2<TNumber>& matrix)
        {
            return Mat2x2<TNumber>{
                matrix[0], matrix[2],
                matrix[1], matrix[3]
            };
        }

        /**
         * @brief Calculates the inverse of a 2x2 matrix.
         *
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param matrix The matrix.
         *
         * @return The inverse of the matrix.
         *
         * @note The formula for calculating the inverse of a 2x2 matrix is:
         * @code{.cpp}
         * | a b |   | d -b |
         * | c d | = | -c a | / (a * d - b * c)
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Inverse(const Mat2x2<TNumber>& matrix)
        {
            const auto determinant = matrix[0] * matrix[3] - matrix[1] * matrix[2];
            if (determinant == 0)
                return Mat2x2<TNumber>::Zero();

            const auto detReversed = static_cast<TNumber>(1.0) / determinant;

            return Mat2x2<TNumber>{
                matrix[3] * detReversed, -matrix[1] * detReversed,
                -matrix[2] * detReversed, matrix[0] * detReversed
            };
        }
    };

    /**
     * @brief This class provides utility functions for working with 3x3 matrices.
     */
    class Matrix3x3Utils final
    {
        template<AnyNumber TNumber> requires (!UnsignedNumber<TNumber>)
        using Mat3x3 = Matrix<3, 3, TNumber>;

    public:
        /**
         * @brief Calculates the determinant of a 3x3 matrix.
         *
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param matrix The matrix.
         *
         * @return The determinant of the matrix.
         *
         * @note The formula for calculating the determinant of a 3x3 matrix is:
         * @code{.cpp}
         * | a b c |
         * | d e f | = a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g)
         * | g h i |
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Determinant(const Mat3x3<TNumber>& matrix)
        {
            return matrix[0] * (matrix[4] * matrix[8] - matrix[5] * matrix[7])
                   - matrix[1] * (matrix[3] * matrix[8] - matrix[5] * matrix[6])
                   + matrix[2] * (matrix[3] * matrix[7] - matrix[4] * matrix[6]);
        }

        /**
         * @brief Transposes a 3x3 matrix.
         *
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param matrix The matrix to transpose.
         *
         * @return The transposed matrix.
         *
         * @note The formula for transposing a 3x3 matrix is:
         * @code{.cpp}
         * | a b c |   | a d g |
         * | d e f | = | b e h |
         * | g h i |   | c f i |
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Transpose(const Mat3x3<TNumber>& matrix)
        {
            return Mat3x3<TNumber>{
                matrix[0], matrix[3], matrix[6],
                matrix[1], matrix[4], matrix[7],
                matrix[2], matrix[5], matrix[8]
            };
        }

        /**
         * @brief Calculates the inverse of a 3x3 matrix.
         *
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param matrix The matrix.
         *
         * @return The inverse of the matrix.
         *
         * @note The formula for calculating the inverse of a 3x3 matrix is:
         * @code{.cpp}
         * | a b c |   | e i - f h  g f - d i  d h - e g |
         * | d e f | = | h c - i b  a i - c g  b g - a h | / (a * (e i - f h) - b * (d i - f g) + c * (d h - e g))
         * | g h i |   | d f - e c  b f - a i  a e - b d |
         * @endcode
         * @note The code used is the result of expanding the formula, simplifying it and reusing some of the
         * intermediate results.
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Inverse(const Mat3x3<TNumber>& matrix)
        {
            const auto m0 = matrix[4] * matrix[8] - matrix[5] * matrix[7];
            const auto m1 = matrix[3] * matrix[8] - matrix[5] * matrix[6];
            const auto m2 = matrix[3] * matrix[7] - matrix[4] * matrix[6];

            const auto determinant = matrix[0] * m0 - matrix[1] * m1 + matrix[2] * m2;
            if (determinant == 0)
                return Mat3x3<TNumber>::Zero();

            const auto detReversed = static_cast<TNumber>(1.0) / determinant;

            const auto m3 = matrix[1] * matrix[8] - matrix[2] * matrix[7];
            const auto m4 = matrix[0] * matrix[8] - matrix[2] * matrix[6];
            const auto m5 = matrix[0] * matrix[7] - matrix[1] * matrix[6];

            const auto m6 = matrix[1] * matrix[5] - matrix[2] * matrix[4];
            const auto m7 = matrix[0] * matrix[5] - matrix[2] * matrix[3];
            const auto m8 = matrix[0] * matrix[4] - matrix[1] * matrix[3];

            return Mat3x3<TNumber>{
                m0 * detReversed, -m1 * detReversed, m2 * detReversed,
                -m3 * detReversed, m4 * detReversed, -m5 * detReversed,
                m6 * detReversed, -m7 * detReversed, m8 * detReversed
            };
        }
    };

    /**
     * @brief This class provides utility functions for working with 4x4 matrices.
     */
    class Matrix4x4Utils final
    {
        template<AnyNumber TNumber> requires (!UnsignedNumber<TNumber>)
        using Mat4x4 = Matrix<4, 4, TNumber>;

    public:
        /**
         * @brief Calculates the determinant of a 4x4 matrix.
         *
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param matrix The matrix.
         *
         * @return The determinant of the matrix.
         *
         * @note Normally, the formula for calculating the determinant of a 4x4 matrix requires multiple 3x3 and 2x2
         * determinants. The code used is the result of expanding the formula and simplifying it.
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Determinant(const Mat4x4<TNumber>& matrix)
        {
            const auto d00 = matrix[10] * matrix[15] - matrix[11] * matrix[14];
            const auto d01 = matrix[9] * matrix[15] - matrix[11] * matrix[13];
            const auto d02 = matrix[9] * matrix[14] - matrix[10] * matrix[13];
            const auto d03 = matrix[8] * matrix[15] - matrix[11] * matrix[12];
            const auto d04 = matrix[8] * matrix[14] - matrix[10] * matrix[12];
            const auto d05 = matrix[8] * matrix[13] - matrix[9] * matrix[12];

            return matrix[0] * (matrix[5] * d00 - matrix[6] * d01 + matrix[7] * d02)
                   - matrix[1] * (matrix[4] * d00 - matrix[6] * d03 + matrix[7] * d04)
                   + matrix[2] * (matrix[4] * d01 - matrix[5] * d03 + matrix[7] * d05)
                   - matrix[3] * (matrix[4] * d02 - matrix[5] * d04 + matrix[6] * d05);
        }

        /**
         * @brief Transposes a 4x4 matrix.
         *
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param matrix The matrix to transpose.
         *
         * @return The transposed matrix.
         *
         * @note The formula for transposing a 4x4 matrix is:
         * @code{.cpp}
         * | a b c d |   | a e i m |
         * | e f g h | = | b f j n |
         * | i j k l |   | c g k o |
         * | m n o p |   | d h l p |
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Transpose(const Mat4x4<TNumber>& matrix)
        {
            return Mat4x4<TNumber>{
                matrix[0], matrix[4], matrix[8], matrix[12],
                matrix[1], matrix[5], matrix[9], matrix[13],
                matrix[2], matrix[6], matrix[10], matrix[14],
                matrix[3], matrix[7], matrix[11], matrix[15]
            };
        }

        /**
         * @brief Calculates the inverse of a 4x4 matrix.
         *
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param matrix The matrix.
         *
         * @return The inverse of the matrix.
         *
         * @note The inverse of a 4x4 matrix is calculated using the Laplace Expansion Theorem: Let A be an n × n
         * matrix. Let r = (r1, r2, ..., rk) be a list of k row indices, where 1 <= k < n and 0 <= r1 < r2 < ... rk < n.
         * The determinant of A is:
         * @code{.cpp}
         * det(A) = ((−1)^|r|) * Sum((-1)^|c| * det(S(A; r, c)) * det(S'(A; r, c)))
         * @endcode
         * <br>where:
         * <br>- |r| = r1 + r2 + ... + rk, |c| = c1 + c2 + ... +ck, and the summation is over all k-tuples
         * c = (c1, c2, ..., ck) for which 1 <= c1 < c2 < ... < ck < n.
         * <br>- S(A; r, c) are the sub-matrices obtained by keeping only the rows and columns of A indexed by r and c.
         * <br>- S'(A; r, c) are the sub-matrices obtained by removing the rows and columns of A indexed by r and c.
         * <br><br>In our case, we will use |r| = 0 + 1, c = (c0, c1) and the above becomes:
         * @code{.cpp}
         * det(A) = ((-1)^|0 + 1|) * Sum((-1)^|c0 + c1| * det(S(A; (0, 1), (c0, c1))) * det(S'(A; (0, 1), (c0, c1))))
         *        = - Sum((-1)^|c0 + c1| * det(S(A; (0, 1), (c0, c1))) * det(S'(A; (0, 1), (c0, c1))))
         *        = + det(S(A; (0, 1), (0, 1))) * det(S'(A; (0, 1), (0, 1)))
         *          - det(S(A; (0, 1), (0, 2))) * det(S'(A; (0, 1), (0, 2)))
         *          + det(S(A; (0, 1), (0, 3))) * det(S'(A; (0, 1), (0, 3)))
         *          + det(S(A; (0, 1), (1, 2))) * det(S'(A; (0, 1), (1, 2)))
         *          - det(S(A; (0, 1), (1, 3))) * det(S'(A; (0, 1), (1, 3)))
         *          + det(S(A; (0, 1), (2, 3))) * det(S'(A; (0, 1), (2, 3)))
         * @endcode
         * <br>The formula below gives us the inverse matrix:
         * @code{.cpp}
         * A^-1 = 1 / det(A) * adj(A)
         * @endcode
         * <br>where:
         * <br>- adj(A) = transpose(cofactor(A))
         * <br>- The cofactor of an element aij is the determinant of the sub-matrix (minor) obtained by deleting
         * the ith row and jth column.
         * <br>- The transpose of a matrix is a matrix whose rows are the columns of the original matrix.
         * <br><br>The code used is the result of expanding the formula, simplifying it and reusing some of the
         * intermediate results.
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Inverse(const Mat4x4<TNumber>& matrix)
        {
            const auto s0 = matrix[0] * matrix[5] - matrix[1] * matrix[4];
            const auto s1 = matrix[0] * matrix[6] - matrix[2] * matrix[4];
            const auto s2 = matrix[0] * matrix[7] - matrix[3] * matrix[4];
            const auto s3 = matrix[1] * matrix[6] - matrix[2] * matrix[5];
            const auto s4 = matrix[1] * matrix[7] - matrix[3] * matrix[5];
            const auto s5 = matrix[2] * matrix[7] - matrix[3] * matrix[6];

            const auto c5 = matrix[10] * matrix[15] - matrix[11] * matrix[14];
            const auto c4 = matrix[9] * matrix[15] - matrix[11] * matrix[13];
            const auto c3 = matrix[9] * matrix[14] - matrix[10] * matrix[13];
            const auto c2 = matrix[8] * matrix[15] - matrix[11] * matrix[12];
            const auto c1 = matrix[8] * matrix[14] - matrix[10] * matrix[12];
            const auto c0 = matrix[8] * matrix[13] - matrix[9] * matrix[12];

            const auto determinant = s0 * c5 - s1 * c4 + s2 * c3 + s3 * c2 - s4 * c1 + s5 * c0;
            if (determinant == 0)
                return Mat4x4<TNumber>::Zero();

            const auto detReversed = static_cast<TNumber>(1.0) / determinant;

            return Mat4x4<TNumber>{
                detReversed * (matrix[5] * c5 - matrix[6] * c4 + matrix[7] * c3),
                detReversed * -(matrix[1] * c5 - matrix[2] * c4 + matrix[3] * c3),
                detReversed * (matrix[1] * s5 - matrix[2] * s4 + matrix[3] * s3),
                detReversed * -(matrix[1] * s2 - matrix[2] * s1 + matrix[3] * s0),

                detReversed * -(matrix[4] * c5 - matrix[6] * c2 + matrix[7] * c1),
                detReversed * (matrix[0] * c5 - matrix[2] * c2 + matrix[3] * c1),
                detReversed * -(matrix[0] * s5 - matrix[2] * s2 + matrix[3] * s1),
                detReversed * (matrix[0] * s4 - matrix[2] * s1 + matrix[3] * s0),

                detReversed * (matrix[4] * c4 - matrix[5] * c2 + matrix[7] * c0),
                detReversed * -(matrix[0] * c4 - matrix[1] * c2 + matrix[3] * c0),
                detReversed * (matrix[0] * s4 - matrix[1] * s2 + matrix[3] * s0),
                detReversed * -(matrix[0] * s3 - matrix[1] * s0 + matrix[3] * s0),

                detReversed * -(matrix[4] * c3 - matrix[5] * c1 + matrix[6] * c0),
                detReversed * (matrix[0] * c3 - matrix[1] * c1 + matrix[2] * c0),
                detReversed * -(matrix[0] * s3 - matrix[1] * s1 + matrix[2] * s0),
                detReversed * (matrix[0] * s2 - matrix[1] * s0 + matrix[2] * s0)
            };
        }

        /**
         * @brief Calculates the orthographic projection matrix given the dimensions of the viewing volume.
         * The resulting matrix can be used to transform 3D coordinates to 2D coordinates on the screen.
         *
         * @tparam Tx The type of the left parameter.
         * @tparam Ty The type of the right parameter.
         * @tparam Tz The type of the bottom parameter.
         * @tparam Tu The type of the top parameter.
         * @tparam Tv The type of the near parameter.
         * @tparam Tw The type of the far parameter.
         *
         * @param left The leftmost coordinate of the viewing volume.
         * @param right The rightmost coordinate of the viewing volume.
         * @param bottom The bottom coordinate of the viewing volume.
         * @param top The top coordinate of the viewing volume.
         * @param nearClip The near clipping plane.
         * @param farClip The far clipping plane.
         *
         * @return The orthographic projection matrix.
         *
         * @note The formula for calculating the orthographic projection matrix is:
         * @code{.cpp}
         * |  2 / (r - l)           0                     0                    0                    |
         * |  0                     2 / (t - b)           0                    0                    |
         * |  0                     0                    -2 / (f - n)          0                    |
         * | -(r + l) / (r - l)    -(t + b) / (t - b)    -(f + n) / (f - n)    1                    |
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz, AnyNumber Tu, AnyNumber Tv, AnyNumber Tw>
        OTR_INLINE static auto Orthographic(Tx left, Ty right, Tz bottom, Tu top, Tv nearClip, Tw farClip)
        {
#define DECL_TYPE decltype(left + right + bottom + top + nearClip + farClip)

            auto result = Mat4x4<DECL_TYPE>::Identity();

            const auto invRl = static_cast<DECL_TYPE>(1.0 / (right - left));
            const auto invTb = static_cast<DECL_TYPE>(1.0 / (top - bottom));
            const auto invFn = static_cast<DECL_TYPE>(1.0 / (farClip - nearClip));

            result[0]  = static_cast<DECL_TYPE>(2.0 * invRl);
            result[5]  = static_cast<DECL_TYPE>(2.0 * invTb);
            result[10] = static_cast<DECL_TYPE>(-2.0 * invFn);
            result[12] = static_cast<DECL_TYPE>((left + right) * invRl);
            result[13] = static_cast<DECL_TYPE>((top + bottom) * invTb);
            result[14] = static_cast<DECL_TYPE>((farClip + nearClip) * invFn);

#undef DECL_TYPE

            return result;
        }

        /**
         * @brief Calculates the perspective projection matrix given the field of view angle, aspect ratio, near and
         * far clipping plane, and the angle type. The resulting matrix can be used to transform 3D points from world
         * space to normalized device coordinates (NDC) in a perspective projection.
         *
         * @tparam Tx The type of the field of view parameter.
         * @tparam Ty The type of the aspect ratio parameter.
         * @tparam Tz The type of the near clipping plane parameter.
         * @tparam Tw The type of the far clipping plane parameter.
         * @tparam TNumber The type of the cells in the matrix.
         *
         * @param fieldOfView The field of view angle.
         * @param aspectRatio The aspect ratio.
         * @param nearClip The near clipping plane.
         * @param farClip The far clipping plane.
         * @param angleType The angle type (radians or degrees). Defaults to radians.
         *
         * @return The perspective projection matrix.
         *
         * @note The formula for calculating the perspective projection matrix is:
         * @code{.cpp}
         * | 1 / (ar * tan(fov / 2))  0                        0                        0 |
         * | 0                        1 / tan(fov / 2)         0                        0 |
         * | 0                        0                       -(f + n) / (f - n)       -1 |
         * | 0                        0                       -(2 * f * n) / (f - n)    0 |
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz, AnyNumber Tw>
        OTR_INLINE static auto Perspective(Tx fieldOfView, Ty aspectRatio, Tz nearClip, Tw farClip,
                                           AngleType angleType = AngleType::Radians)
        {
            if (angleType == AngleType::Degrees)
                fieldOfView = Math::DegToRad(fieldOfView);

#define DECL_TYPE decltype(fieldOfView * aspectRatio * nearClip * farClip)

            auto result = Mat4x4<DECL_TYPE>::Zero();

            const auto tanHalfFov = Math::Tan(fieldOfView * 0.5);

            result[0]  = static_cast<DECL_TYPE>(1.0 / (aspectRatio * tanHalfFov));
            result[5]  = static_cast<DECL_TYPE>(1.0 / tanHalfFov);
            result[10] = static_cast<DECL_TYPE>(-(farClip + nearClip) / (farClip - nearClip));
            result[11] = static_cast<DECL_TYPE>(-1.0);
            result[14] = static_cast<DECL_TYPE>(-(2.0 * farClip * nearClip) / (farClip - nearClip));

#undef DECL_TYPE

            return result;
        }

        /**
         * @brief Calculates the look-at transformation matrix given the position, target, and up vectors.
         *
         * @tparam Tx The type of the coordinates of the position vector.
         * @tparam Ty The type of the coordinates of the target vector.
         * @tparam Tz The type of the coordinates of the up vector.
         *
         * @param position The position vector of the camera.
         * @param target The target vector specifying the point the camera is looking at.
         * @param up The up vector specifying the orientation of the camera.
         *
         * @return The look-at transformation matrix.
         *
         * @note The formula for calculating the look-at transformation matrix is:
         * @code{.cpp}
         * |  xAxis.x                  yAxis.x                -zAxis.x                 0 |
         * |  xAxis.y                  yAxis.y                -zAxis.y                 0 |
         * |  xAxis.z                  yAxis.z                -zAxis.z                 0 |
         * | -dot(xAxis, position)    -dot(yAxis, position)    dot(zAxis, position)    1 |
         * @endcode
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto LookAt(Vector3D<Tx> position, Vector3D<Ty> target, Vector3D<Tz> up)
        {
            const auto zAxis = Vec3D::Normalise(position - target);
            const auto xAxis = Vec3D::Normalise(Vec3D::Cross(up, zAxis));
            const auto yAxis = Vec3D::Cross(zAxis, xAxis);

#define DECL_TYPE decltype(position[0] + target[0] + up[0])
#define CELL(axis) Vec3D::Dot(axis, position)

            return Mat4x4<DECL_TYPE>{
                xAxis.x, yAxis.x, -zAxis.x, static_cast<DECL_TYPE>(0.0),
                xAxis.y, yAxis.y, -zAxis.y, static_cast<DECL_TYPE>(0.0),
                xAxis.z, yAxis.z, -zAxis.z, static_cast<DECL_TYPE>(0.0),
                -CELL(xAxis), -CELL(yAxis), CELL(zAxis), static_cast<DECL_TYPE>(1.0)
            };

#undef DECL_TYPE
#undef CELL
        }

        /**
         * @brief Translates a 3D vector to a 4x4 matrix representing translation.
         *
         * @tparam TNumber The type of the numbers in the vector and matrix.
         *
         * @param translation The 3D vector representing translation.
         *
         * @return The 4x4 matrix representing translation.
         *
         * @note The formula for calculating the translation matrix is:
         * @code{.cpp}
         * | 1   0   0   0 |
         * | 0   1   0   0 |
         * | 0   0   1   0 |
         * | x   y   z   1 |
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Translation(const Vector3D<TNumber>& translation)
        {
            auto result = Mat4x4<TNumber>::Identity();

            result[12] = translation[0];
            result[13] = translation[1];
            result[14] = translation[2];

            return result;
        }

        /**
         * @brief Translates a 3D vector to a 4x4 matrix representing translation.
         *
         * @tparam TNumber The type of the numbers in the vector and matrix.
         *
         * @param translation The 3D vector representing translation.
         *
         * @return The 4x4 matrix representing translation.
         *
         * @note The formula for calculating the translation matrix is:
         * @code{.cpp}
         * | 1   0   0   0 |
         * | 0   1   0   0 |
         * | 0   0   1   0 |
         * | x   y   z   1 |
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Translation(Vector3D<TNumber>&& translation)
        {
            auto result = Mat4x4<TNumber>::Identity();

            result[12] = translation[0];
            result[13] = translation[1];
            result[14] = translation[2];

            return result;
        }

        /**
         * @brief Calculates the rotation matrix corresponding to the given quaternion.
         *
         * @tparam TNumber The type of the numbers in the quaternion and matrix.
         *
         * @param quaternion The quaternion representing the rotation.
         *
         * @return The rotation matrix.
         *
         * @note The formula for calculating the rotation matrix is:
         * @code{.cpp}
         * | 1 - 2 * (y * y + z * z)    2 * (x * y - w * z)        2 * (x * z + w * y)        0 |
         * | 2 * (x * y + w * z)        1 - 2 * (x * x + z * z)    2 * (y * z - w * x)        0 |
         * | 2 * (x * z - w * y)        2 * (y * z + w * x)        1 - 2 * (x * x + y * y)    0 |
         * | 0                          0                          0                          1 |
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Rotation(const Quaternion<TNumber>& quaternion)
        {
            const auto quaternionNormalised = Quat::Normalise(quaternion);

            const auto x   = quaternionNormalised[0];
            const auto y   = quaternionNormalised[1];
            const auto z   = quaternionNormalised[2];
            const auto w   = quaternionNormalised[3];
            const auto two = static_cast<TNumber>(2.0);

            auto result  = Mat4x4<TNumber>::Identity();

            result[0, 0] -= two * (y * y + z * z);
            result[0, 1] = two * (x * y - w * z);
            result[0, 2] = two * (x * z + w * y);

            result[1, 0] = two * (x * y + w * z);
            result[1, 1] -= two * (x * x + z * z);
            result[1, 2] = two * (y * z - w * x);

            result[2, 0] = two * (x * z - w * y);
            result[2, 1] = two * (y * z + w * x);
            result[2, 2] -= two * (x * x + y * y);

            return result;
        }

        /**
         * @brief Calculates the rotation matrix corresponding to the given quaternion.
         *
         * @tparam TNumber The type of the numbers in the quaternion and matrix.
         *
         * @param quaternion The quaternion representing the rotation.
         *
         * @return The rotation matrix.
         *
         * @note The formula for calculating the rotation matrix is:
         * @code{.cpp}
         * | 1 - 2 * (y * y + z * z)    2 * (x * y - w * z)        2 * (x * z + w * y)        0 |
         * | 2 * (x * y + w * z)        1 - 2 * (x * x + z * z)    2 * (y * z - w * x)        0 |
         * | 2 * (x * z - w * y)        2 * (y * z + w * x)        1 - 2 * (x * x + y * y)    0 |
         * | 0                          0                          0                          1 |
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Rotation(Quaternion<TNumber>&& quaternion)
        {
            const auto quaternionNormalised = Normalise(quaternion);

            const auto x   = quaternionNormalised[0];
            const auto y   = quaternionNormalised[1];
            const auto z   = quaternionNormalised[2];
            const auto w   = quaternionNormalised[3];
            const auto two = static_cast<TNumber>(2.0);

            auto result  = Mat4x4<TNumber>::Identity();

            result[0, 0] -= two * (y * y + z * z);
            result[0, 1] = two * (x * y - w * z);
            result[0, 2] = two * (x * z + w * y);

            result[1, 0] = two * (x * y + w * z);
            result[1, 1] -= two * (x * x + z * z);
            result[1, 2] = two * (y * z - w * x);

            result[2, 0] = two * (x * z - w * y);
            result[2, 1] = two * (y * z + w * x);
            result[2, 2] -= two * (x * x + y * y);

            return result;
        }

        /**
         *  @brief Applies rotation transformation to a 4x4 matrix.
         *
         *  @tparam TNumber The data type of the angle and matrix cells.
         *
         *  @param angle The angle of rotation, in either radians or degrees depending on the angleType parameter.
         *  @param axis The axis of rotation.
         *  @param angleType The type of angle provided (radians or degrees). Default is radians.
         *
         *  @return The resulting 4x4 matrix after rotation transformation.
         *
         *  @note The formula for calculating the rotation matrix is different depending on the axis of rotation.
         *  <br>For rotation around the X axis:
         *  @code{.cpp}
         *  | 1    0             0             0 |
         *  | 0    cos(angle)   -sin(angle)    0 |
         *  | 0    sin(angle)    cos(angle)    0 |
         *  | 0    0             0             1 |
         *  @endcode
         *  <br>For rotation around the Y axis:
         *  @code{.cpp}
         *  | cos(angle)    0    sin(angle)    0 |
         *  | 0             1    0             0 |
         *  |-sin(angle)    0    cos(angle)    0 |
         *  | 0             0    0             1 |
         *  @endcode
         *  <br>For rotation around the Z axis:
         *  @code{.cpp}
         *  | cos(angle)   -sin(angle)    0    0 |
         *  | sin(angle)    cos(angle)    0    0 |
         *  | 0             0             1    0 |
         *  | 0             0             0    1 |
         *  @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Rotation(TNumber angle, Axis axis, AngleType angleType = AngleType::Radians)
        {
            if (angleType == AngleType::Degrees)
                angle = Math::DegToRad(angle);

            const auto cosAngle = Math::Cos(angle);
            const auto sinAngle = Math::Sin(angle);

            auto result = Mat4x4<TNumber>::Identity();

            switch (axis)
            {
                case Axis::X:
                    result[1, 1] = cosAngle;
                    result[1, 2] = -sinAngle;
                    result[2, 1] = sinAngle;
                    result[2, 2] = cosAngle;
                    break;
                case Axis::Y:
                    result[0, 0] = cosAngle;
                    result[0, 2] = sinAngle;
                    result[2, 0] = -sinAngle;
                    result[2, 2] = cosAngle;
                    break;
                case Axis::Z:
                    result[0, 0] = cosAngle;
                    result[0, 1] = -sinAngle;
                    result[1, 0] = sinAngle;
                    result[1, 1] = cosAngle;
                    break;
            }

            return result;
        }

        /**
         * @brief Creates a 4x4 scaling matrix using the given scale vector.
         *
         * @tparam TNumber The type of the coordinates of the scale vector.
         *
         * @param scale The scale vector.
         *
         * @return The scaling matrix as a 4x4 matrix.
         *
         * @note The formula for calculating the scaling matrix is:
         * @code{.cpp}
         * | x   0   0   0 |
         * | 0   y   0   0 |
         * | 0   0   z   0 |
         * | 0   0   0   1 |
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Scale(const Vector3D<TNumber>& scale)
        {
            auto result = Mat4x4<TNumber>::Identity();

            result[0]  = scale[0];
            result[5]  = scale[1];
            result[10] = scale[2];

            return result;
        }

        /**
         * @brief Creates a 4x4 scaling matrix using the given scale vector.
         *
         * @tparam TNumber The type of the coordinates of the scale vector.
         *
         * @param scale The scale vector.
         *
         * @return The scaling matrix as a 4x4 matrix.
         *
         * @note The formula for calculating the scaling matrix is:
         * @code{.cpp}
         * | x   0   0   0 |
         * | 0   y   0   0 |
         * | 0   0   z   0 |
         * | 0   0   0   1 |
         * @endcode
         */
        template<AnyNumber TNumber>
        OTR_INLINE static auto Scale(Vector3D<TNumber>&& scale)
        {
            auto result = Mat4x4<TNumber>::Identity();

            result[0]  = scale[0];
            result[5]  = scale[1];
            result[10] = scale[2];

            return result;
        }

        /**
         * @brief Apply TRS (Translation, Rotation, Scale) transformation to create a transformation matrix.
         *
         * @tparam Tx The type of the coordinates of the translation vector.
         * @tparam Ty The type of the coordinates of the rotation quaternion.
         * @tparam Tz The type of the coordinates of the scale vector.
         *
         * @param translation The translation vector representing the object's translation along the x, y, and z axes.
         * @param rotation The rotation quaternion representing the object's rotation.
         * @param scale The scale vector representing the object's scaling along the x, y, and z axes.
         *
         * @return The resulting transformation matrix.
         *
         * @note The formula for calculating the transformation matrix is T * R * S, where T is the translation matrix,
         * R is the rotation matrix, and S is the scaling matrix.
         */
        template<AnyNumber Tx, AnyNumber Ty, AnyNumber Tz>
        OTR_INLINE static auto TRS(const Vector3D<Tx>& translation,
                                   const Quaternion<Ty>& rotation,
                                   const Vector3D<Tz>& scale)
        {
            return MatrixUtils::Multiply(
                MatrixUtils::Multiply(Translation(translation),
                                      Rotation(rotation)),
                Scale(scale));
        }
    };
}

#endif //OTTERENGINE_MATRIX_H
