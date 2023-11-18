#ifndef OTTERENGINE_RECTANGLE_H
#define OTTERENGINE_RECTANGLE_H

#include "Core/Defines.h"
#include "Math/Math.Concepts.h"
#include "Math/Vector.h"

namespace Otter
{
    template<AnyNumber TNumber>
    struct Rectangle final
    {
    public:
        union
        {
            struct
            {
                TNumber X;
                TNumber Y;
                TNumber Width;
                TNumber Height;
            };

            struct
            {
                Vector<2, TNumber> Position;
                Vector<2, TNumber> Size;
            };

            TNumber Data[4];
        };

        Rectangle()
            : X(0), Y(0), Width(0), Height(0)
        {
        }
        ~Rectangle() = default;

        OTR_DISABLE_HEAP_ALLOCATION

        constexpr Rectangle(const Vector<2, TNumber>& position, const Vector<2, TNumber>& size)
            : Position(position), Size(size)
        {
            OTR_DEBUG_BLOCK(
                if (position.GetX() < 0 || position.GetY() < 0)
                {
                    OTR_LOG_WARNING("Negative values for the position of a rectangle might have unexpected results")
                }

                if (size.GetX() < 0 || size.GetY() < 0)
                {
                    OTR_LOG_WARNING("Negative values for the size of a rectangle might have unexpected results")
                }
            )
        }

        constexpr Rectangle(const TNumber& x, const TNumber& y, const TNumber& width, const TNumber& height)
            : X(x), Y(y), Width(width), Height(height)
        {
            OTR_DEBUG_BLOCK(
                if (x < 0 || y < 0)
                {
                    OTR_LOG_WARNING("Negative values for the position of a rectangle might have unexpected results")
                }

                if (width < 0 || height < 0)
                {
                    OTR_LOG_WARNING("Negative values for the size of a rectangle might have unexpected results")
                }
            )
        }

        Rectangle(const Rectangle& other)
        {
            Position = other.Position;
            Size     = other.Size;
        }

        Rectangle(Rectangle&& other) noexcept
        {
            Position = std::move(other.Position);
            Size     = std::move(other.Size);
        }

        Rectangle& operator=(const Rectangle& other)
        {
            Position = other.Position;
            Size     = other.Size;

            return *this;
        }

        Rectangle& operator=(Rectangle&& other) noexcept
        {
            Position = std::move(other.Position);
            Size     = std::move(other.Size);

            return *this;
        }

        OTR_INLINE bool operator==(const Rectangle& other) const
        {
            return Position == other.Position && Size == other.Size;
        }

        OTR_INLINE bool operator!=(const Rectangle& other) const
        {
            return !(*this == other);
        }

        OTR_INLINE static constexpr Rectangle Zero() { return { 0, 0, 0, 0 }; }
        OTR_INLINE static constexpr Rectangle Unit() { return { 0, 0, 1, 1 }; }

        OTR_INLINE Vector<2, TNumber> GetCenter() const { return { X + Width * 0.5f, Y + Height * 0.5f }; }

        OTR_INLINE Vector<2, TNumber> GetTopLeft() const { return { X, Y }; }
        OTR_INLINE Vector<2, TNumber> GetTopRight() const { return { X + Width, Y }; }
        OTR_INLINE Vector<2, TNumber> GetBottomLeft() const { return { X, Y + Height }; }
        OTR_INLINE Vector<2, TNumber> GetBottomRight() const { return { X + Width, Y + Height }; }

        OTR_INLINE Vector<2, TNumber> GetMinX() const { return { X }; }
        OTR_INLINE Vector<2, TNumber> GetMaxX() const { return { X + Width }; }

        OTR_INLINE Vector<2, TNumber> GetMinY() const { return { Y }; }
        OTR_INLINE Vector<2, TNumber> GetMaxY() const { return { Y + Height }; }

        OTR_INLINE bool Contains(const Vector<2, TNumber>& point) const
        {
            return point.GetX() >= X && point.GetX() <= X + Width
                   && point.GetY() >= Y && point.GetY() <= Y + Height;
        }

        OTR_INLINE bool Overlaps(const Rectangle& other, bool allowInverse = false) const
        {
            if (allowInverse)
            {
                return (other.X < X + Width) && (other.X + other.Width > X)
                       && (other.Y < Y + Height) && (other.Y + other.Height > Y);
            }

            return (other.X < X + Width) && (other.X + other.Width > X)
                   && (other.Y < Y + Height) && (other.Y + other.Height > Y)
                   && (other.X < X) && (other.X + other.Width > X + Width)
                   && (other.Y < Y) && (other.Y + other.Height > Y + Height);
        }
    };
}

#endif //OTTERENGINE_RECTANGLE_H
