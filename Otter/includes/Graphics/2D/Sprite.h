#ifndef OTTERENGINE_SPRITE_H
#define OTTERENGINE_SPRITE_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Collections/Span.h"
#include "Math/Vector.h"

namespace Otter
{
    class Sprite
    {
    public:
        Sprite() = default;
        ~Sprite() = default;

        OTR_DISABLE_HEAP_ALLOCATION

        Sprite(const Vector<2, Float32>& position, const Vector<2, Float32>& size, const Vector<4, Float32>& color)
            : m_Position(position), m_Size(size), m_Color(color)
        {
        }

        Sprite(const Sprite& other)
        {
            m_Position = other.m_Position;
            m_Size     = other.m_Size;
            m_Color    = other.m_Color;
        }

        Sprite(Sprite&& other) noexcept
        {
            m_Position = std::move(other.m_Position);
            m_Size     = std::move(other.m_Size);
            m_Color    = std::move(other.m_Color);
        }

        Sprite& operator=(const Sprite& other) = default;

        Sprite& operator=(Sprite&& other) noexcept
        {
            m_Position = std::move(other.m_Position);
            m_Size     = std::move(other.m_Size);
            m_Color    = std::move(other.m_Color);

            return *this;
        }

        OTR_INLINE Vector<2, Float32> GetPosition() const { return m_Position; }
        OTR_INLINE void SetPosition(const Vector<2, Float32>& position) { m_Position = position; }

        OTR_INLINE Vector<2, Float32> GetSize() const { return m_Size; }
        OTR_INLINE void SetSize(const Vector<2, Float32>& size) { m_Size = size; }

        OTR_INLINE Vector<4, Float32> GetColor() const { return m_Color; }
        OTR_INLINE void SetColor(const Vector<4, Float32>& color) { m_Color = color; }

        OTR_INLINE Span<Vector<2, Float32>, 4> GetVertices() const
        {
            Span<Vector<2, Float32>, 4> vertices{ };

            vertices[0] = { m_Position[0] - m_Size.GetX() * 0.5f, m_Position[1] - m_Size.GetY() * 0.5f };
            vertices[1] = { m_Position[0] + m_Size.GetX() * 0.5f, m_Position[1] - m_Size.GetY() * 0.5f };
            vertices[2] = { m_Position[0] + m_Size.GetX() * 0.5f, m_Position[1] + m_Size.GetY() * 0.5f };
            vertices[3] = { m_Position[0] - m_Size.GetX() * 0.5f, m_Position[1] + m_Size.GetY() * 0.5f };

            return vertices;
        }

    private:
        Vector<2, Float32> m_Position;
        Vector<2, Float32> m_Size;
        Vector<4, Float32> m_Color;
    };
}

#endif //OTTERENGINE_SPRITE_H
