#ifndef OTTERENGINE_QUATERNION_H
#define OTTERENGINE_QUATERNION_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Math/Math.Concepts.h"
#include "Math/Core.h"

namespace Otter
{
    template<AnyNumber TNumber>
    struct Quaternion final
    {
    public:
        constexpr Quaternion()
        {
            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = static_cast<TNumber>(0.0);
        }

        constexpr explicit Quaternion(TNumber scalar)
        {
            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = scalar;
        }

        constexpr Quaternion(InitialiserList<TNumber> list)
        {
            OTR_ASSERT_MSG(list.size() == 4, "Initialiser list size does not match Quaternion size")

            UInt64 i = 0;
            for (const TNumber& value: list)
                m_Values[i++] = value;
        }

        Quaternion(const Quaternion<TNumber>& other)
        {
            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = other.m_Values[i];
        }

        Quaternion(Quaternion<TNumber>&& other) noexcept
        {
            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = other.m_Values[i];
        }

        Quaternion<TNumber>& operator=(const Quaternion<TNumber>& other)
        {
            if (this == &other)
                return *this;

            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = other.m_Values[i];

            return *this;
        }

        Quaternion<TNumber>& operator=(Quaternion<TNumber>&& other) noexcept
        {
            if (this == &other)
                return *this;

            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = other.m_Values[i];

            return *this;
        }

        TNumber& operator[](UInt8 index)
        {
            OTR_ASSERT_MSG(index < 4, "Index {0} is out of range", index)
            return m_Values[index];
        }

        const TNumber& operator[](UInt8 index) const
        {
            OTR_ASSERT_MSG(index < 4, "Index {0} is out of range", index)
            return m_Values[index];
        }

        template<AnyNumber TOtherNumber>
        explicit operator Quaternion<TOtherNumber>() const
        {
            Quaternion<TOtherNumber> result;

            for (UInt8 i = 0; i < 4; ++i)
                result[i] = static_cast<TOtherNumber>(m_Values[i]);

            return result;
        }

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

        template<AnyNumber TOtherNumber>
        Quaternion<TNumber>& operator*=(const TOtherNumber& scalar) noexcept
        {
            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] *= scalar;

            return *this;
        }

        template<AnyNumber TOtherNumber>
        Quaternion<TNumber>& operator/=(const TOtherNumber& scalar) noexcept
        {
            OTR_ASSERT_MSG(scalar != 0, "Division by zero")

            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] /= scalar;

            return *this;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator*(Quaternion<TNumber> lhs, const Quaternion<TOtherNumber>& rhs)
        {
            lhs *= rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator*(Quaternion<TNumber> lhs, const TOtherNumber& rhs)
        {
            lhs *= rhs;
            return lhs;
        }

        template<AnyNumber TOtherNumber>
        friend decltype(auto) operator/(Quaternion<TNumber> lhs, const TOtherNumber& rhs)
        {
            OTR_ASSERT_MSG(rhs != 0, "Division by zero")

            lhs /= rhs;
            return lhs;
        }

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

        template<AnyNumber TOtherNumber>
        bool operator!=(const Quaternion<TOtherNumber>& other) const noexcept
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

        [[nodiscard]] OTR_INLINE TNumber GetZ() const noexcept { return m_Values[2]; }
        OTR_INLINE void SetZ(const TNumber& z) noexcept { m_Values[2] = z; }

        [[nodiscard]] OTR_INLINE TNumber GetW() const noexcept { return m_Values[3]; }
        OTR_INLINE void SetW(const TNumber& w) noexcept { m_Values[3] = w; }

        OTR_INLINE static constexpr Quaternion<TNumber> Zero() noexcept
        {
            return Quaternion<TNumber>(static_cast<TNumber>(0.0));
        }

        OTR_INLINE static constexpr Quaternion<TNumber> One() noexcept
        {
            return Quaternion<TNumber>(static_cast<TNumber>(1.0));
        }

        OTR_INLINE static constexpr Quaternion<TNumber> Identity() noexcept
        {
            return Quaternion<TNumber>{ static_cast<TNumber>(0.0),
                                        static_cast<TNumber>(0.0),
                                        static_cast<TNumber>(0.0),
                                        static_cast<TNumber>(1.0) };
        }

    private:
        TNumber m_Values[4];
    };
}

#include "Math/Quaternion.h"

#endif //OTTERENGINE_QUATERNION_H
