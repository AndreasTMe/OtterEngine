#ifndef OTTERENGINE_TEXTURE_H
#define OTTERENGINE_TEXTURE_H

#include "Core/Defines.h"
#include "Core/BaseTypes.h"

namespace Otter::Graphics
{
    class Texture
    {
    public:
        virtual ~Texture() = default;

        virtual void Bind() = 0;
        virtual void Unbind() = 0;

        [[nodiscard]] static Texture* Create(const char* filePath);
        static void Destroy(Texture* outTexture);

        [[nodiscard]] OTR_INLINE UInt32 GetWidth() const { return m_Width; }
        [[nodiscard]] OTR_INLINE UInt32 GetHeight() const { return m_Height; }

    protected:
        explicit Texture(const char* const filePath)
            : m_FilePath(filePath), m_Width(0), m_Height(0)
        {
        }

        const char* m_FilePath;

        UInt32 m_Width;
        UInt32 m_Height;
    };
}

#endif //OTTERENGINE_TEXTURE_H
