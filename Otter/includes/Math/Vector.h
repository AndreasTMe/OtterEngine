#ifndef OTTERENGINE_VECTOR_H
#define OTTERENGINE_VECTOR_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Math/Math.Concepts.h"
#include "Math/Core.h"

namespace Otter
{
    template<UInt8 TDimension, AnyNumber TNumber> requires Dimension<TDimension>
    struct Vector final
    {
    public:
        constexpr Vector()
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = static_cast<TNumber>(0.0);
        }

        constexpr explicit Vector(TNumber scalar)
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = scalar;
        }

        constexpr Vector(InitialiserList<TNumber> list)
        {
            OTR_ASSERT_MSG(list.size() == TDimension, "Initialiser list size does not match Vector size")

            UInt64 i = 0;
            for (const TNumber& value: list)
                m_Values[i++] = value;
        }

        Vector(const Vector<TDimension, TNumber>& other)
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = other.m_Values[i];
        }

        Vector(Vector<TDimension, TNumber>&& other) noexcept
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = other.m_Values[i];
        }

        Vector<TDimension, TNumber>& operator=(const Vector<TDimension, TNumber>& other)
        {
            if (this == &other)
                return *this;

            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = other.m_Values[i];

            return *this;
        }

        Vector<TDimension, TNumber>& operator=(Vector<TDimension, TNumber>&& other) noexcept
        {
            if (this == &other)
                return *this;

            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] = other.m_Values[i];

            return *this;
        }

        TNumber& operator[](UInt8 index)
        {
            OTR_ASSERT_MSG(index < TDimension, "Index {0} is out of range", index)
            return m_Values[index];
        }

        const TNumber& operator[](UInt8 index) const
        {
            OTR_ASSERT_MSG(index < TDimension, "Index {0} is out of range", index)
            return m_Values[index];
        }

        template<AnyNumber TOtherNumber>
        explicit operator Vector<TDimension, TOtherNumber>() const
        {
            Vector<TDimension, TOtherNumber> result;

            for (UInt8 i = 0; i < TDimension; ++i)
                result.m_Values[i] = static_cast<TOtherNumber>(m_Values[i]);

            return result;
        }

        template<AnyNumber TOtherNumber>
        Vector<TDimension, TNumber>& operator+=(const Vector<TDimension, TOtherNumber>& other)
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] += static_cast<TNumber>(other.m_Values[i]);

            return *this;
        }

        Vector<TDimension, TNumber>& operator+=(const Vector<TDimension, TNumber>& other) noexcept
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] += other.m_Values[i];

            return *this;
        }

        template<AnyNumber TOtherNumber>
        Vector<TDimension, TNumber>& operator-=(const Vector<TDimension, TOtherNumber>& other)
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] -= static_cast<TNumber>(other.m_Values[i]);

            return *this;
        }

        Vector<TDimension, TNumber>& operator-=(const Vector<TDimension, TNumber>& other) noexcept
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] -= other.m_Values[i];

            return *this;
        }

        template<AnyNumber TOtherNumber>
        Vector<TDimension, TNumber>& operator*=(const TOtherNumber& scalar) noexcept
        {
            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] *= scalar;

            return *this;
        }

        template<AnyNumber TOtherNumber>
        Vector<TDimension, TNumber>& operator/=(const TOtherNumber& scalar) noexcept
        {
            OTR_ASSERT_MSG(scalar != 0, "Division by zero")

            for (UInt8 i = 0; i < TDimension; ++i)
                m_Values[i] /= scalar;

            return *this;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator+(Vector<TDimension, TNumber> lhs,
                                        const Vector<TDimension, TOtherNumber>& rhs)
        {
            lhs += rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator-(Vector<TDimension, TNumber> lhs,
                                        const Vector<TDimension, TOtherNumber>& rhs)
        {
            lhs -= rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator*(Vector<TDimension, TNumber> lhs, const TOtherNumber& rhs)
        {
            lhs *= rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator/(Vector<TDimension, TNumber> lhs, const TOtherNumber& rhs)
        {
            OTR_ASSERT_MSG(rhs != 0, "Division by zero")

            lhs /= rhs;
            return lhs;
        }

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

        template<AnyNumber TOtherNumber>
        bool operator!=(const Vector<TDimension, TOtherNumber>& other) const noexcept
        {
#pragma clang diagnostic push
#pragma ide diagnostic ignored "Simplify"

            return !(*this == other);

#pragma clang diagnostic pop
        }

        [[nodiscard]] OTR_INLINE TNumber GetX() const noexcept { return m_Values[0]; }
        OTR_INLINE void SetX(const TNumber& x) noexcept { m_Values[0] = x; }

        [[nodiscard]] OTR_INLINE TNumber GetY() const noexcept { return m_Values[1]; }
        OTR_INLINE void SetY(const TNumber& y) noexcept { m_Values[1] = y; }

        [[nodiscard]] OTR_INLINE TNumber GetZ() const noexcept requires (TDimension >= 3) { return m_Values[2]; }
        OTR_INLINE void SetZ(const TNumber& z) noexcept requires (TDimension >= 3) { m_Values[2] = z; }

        [[nodiscard]] OTR_INLINE TNumber GetW() const noexcept requires (TDimension == 4) { return m_Values[3]; }
        OTR_INLINE void SetW(const TNumber& w) noexcept requires (TDimension == 4) { m_Values[3] = w; }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> Left() requires (TDimension < 4)
        {
            if constexpr (TDimension == 2)
                return Vector<2, TNumber>(static_cast<TNumber>(-1.0),
                                          static_cast<TNumber>(0.0));
            else
                return Vector<3, TNumber>(static_cast<TNumber>(-1.0),
                                          static_cast<TNumber>(0.0),
                                          static_cast<TNumber>(0.0));
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> Right() requires (TDimension < 4)
        {
            if constexpr (TDimension == 2)
                return Vector<2, TNumber>(static_cast<TNumber>(1.0), static_cast<TNumber>(0.0));
            else
                return Vector<3, TNumber>(static_cast<TNumber>(1.0),
                                          static_cast<TNumber>(0.0),
                                          static_cast<TNumber>(0.0));
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> Down() requires (TDimension < 4)
        {
            if constexpr (TDimension == 2)
                return Vector<2, TNumber>(static_cast<TNumber>(0.0), static_cast<TNumber>(-1.0));
            else
                return Vector<3, TNumber>(static_cast<TNumber>(0.0),
                                          static_cast<TNumber>(-1.0),
                                          static_cast<TNumber>(0.0));
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> Up() requires (TDimension < 4)
        {
            if constexpr (TDimension == 2)
                return Vector<2, TNumber>(static_cast<TNumber>(0.0), static_cast<TNumber>(1.0));
            else
                return Vector<3, TNumber>(static_cast<TNumber>(0.0),
                                          static_cast<TNumber>(1.0),
                                          static_cast<TNumber>(0.0));
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> Back() requires (TDimension == 3)
        {
            return Vector<3, TNumber>(static_cast<TNumber>(0.0), static_cast<TNumber>(0.0), static_cast<TNumber>(-1.0));
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> Forward() requires (TDimension == 3)
        {
            return Vector<3, TNumber>(static_cast<TNumber>(0.0), static_cast<TNumber>(0.0), static_cast<TNumber>(1.0));
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> One()
        {
            return Vector<TDimension, TNumber>(static_cast<TNumber>(1.0));
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> Zero()
        {
            return Vector<TDimension, TNumber>(static_cast<TNumber>(0.0));
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> PositiveInfinity()
        {
            return Vector<TDimension, TNumber>(Math::PositiveInfinity<TNumber>());
        }

        OTR_INLINE static constexpr Vector<TDimension, TNumber> NegativeInfinity()
        {
            return Vector<TDimension, TNumber>(Math::NegativeInfinity<TNumber>());
        }

    private:
        TNumber m_Values[TDimension];
    };
}

#include "Math/Math.Vector2.h"
#include "Math/Math.Vector3.h"
#include "Math/Math.Vector4.h"

#endif //OTTERENGINE_VECTOR_H
