#ifndef OTTERENGINE_SPRITE_H
#define OTTERENGINE_SPRITE_H

#include "Core/Collections/Span.h"
#include "Math/Vector.h"
#include "Math/Geometry/Rectangle.h"

namespace Otter
{
    class Sprite
    {
        template<AnyNumber TNumber>
        using Rectangle = Math::Rectangle<TNumber>;

    public:
        Sprite() = default;
        ~Sprite() = default;

        Sprite(const Rectangle<Float32>& bounds, const Vector4D<Float32>& color)
            : m_Bounds(bounds), m_Color(color)
        {
        }

        Sprite(const Vector2D<Float32>& position, const Vector2D<Float32>& size, const Vector4D<Float32>& color)
            : m_Bounds({ position - size * 0.5f, size }), m_Color(color)
        {
        }

        Sprite(const Sprite& other)
        {
            m_Bounds = other.m_Bounds;
            m_Color  = other.m_Color;
        }

        Sprite(Sprite&& other) noexcept
        {
            m_Bounds = std::move(other.m_Bounds);
            m_Color  = std::move(other.m_Color);
        }

        Sprite& operator=(const Sprite& other) = default;

        Sprite& operator=(Sprite&& other) noexcept
        {
            m_Bounds = std::move(other.m_Bounds);
            m_Color  = std::move(other.m_Color);

            return *this;
        }

        OTR_INLINE Rectangle<Float32> GetBounds() const { return m_Bounds; }
        OTR_INLINE void SetBounds(const Rectangle<Float32>& bounds) { m_Bounds = bounds; }

        OTR_INLINE Vector4D<Float32> GetColor() const { return m_Color; }
        OTR_INLINE void SetColor(const Vector4D<Float32>& color) { m_Color = color; }

        OTR_INLINE Vector2D<Float32> GetCenter() const { return m_Bounds.GetCenter(); }

        OTR_INLINE Span<Vector2D<Float32>, 4> GetVertices() const
        {
            Span<Vector2D<Float32>, 4> vertices{ };

            vertices[0] = m_Bounds.GetBottomLeft();
            vertices[1] = m_Bounds.GetBottomRight();
            vertices[2] = m_Bounds.GetTopRight();
            vertices[3] = m_Bounds.GetTopLeft();

            return vertices;
        }

        OTR_INLINE Span<Vector2D<Float32>, 4> GetTexCoords() const
        {
            return {{ 0.0f, 1.0f },
                    { 1.0f, 1.0f },
                    { 1.0f, 0.0f },
                    { 0.0f, 0.0f }};
        }

    private:
        Rectangle<Float32> m_Bounds;
        Vector4D<Float32>  m_Color;
    };
}

#endif //OTTERENGINE_SPRITE_H
