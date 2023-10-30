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
        Quaternion()
        {
            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = 0;
        }

        explicit Quaternion(TNumber scalar)
        {
            for (UInt8 i = 0; i < 4; ++i)
                m_Values[i] = scalar;
        }

        template<typename... TArgs>
        requires (sizeof...(TArgs) == 3 && (AnyNumber<TArgs>&& ...))
        explicit Quaternion(TNumber x, TArgs... args)
        {
            m_Values[0] = x;

            UInt8 i = 1;
            ([&]
            {
                m_Values[i++] = args;
            }(), ...);
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

        // TODO: Quaternion ==, !=, +, -, *, /, +=, -=, *=, /=

        [[nodiscard]] TNumber GetX() const noexcept { return m_Values[0]; }
        void SetX(const TNumber& x) noexcept { m_Values[0] = x; }

        [[nodiscard]] TNumber GetY() const noexcept { return m_Values[1]; }
        void SetY(const TNumber& y) noexcept { m_Values[1] = y; }

        [[nodiscard]] TNumber GetZ() const noexcept { return m_Values[2]; }
        void SetZ(const TNumber& z) noexcept { m_Values[2] = z; }

        [[nodiscard]] TNumber GetW() const noexcept { return m_Values[3]; }
        void SetW(const TNumber& w) noexcept { m_Values[3] = w; }

    private:
        TNumber m_Values[4];
    };
}

#endif //OTTERENGINE_QUATERNION_H
