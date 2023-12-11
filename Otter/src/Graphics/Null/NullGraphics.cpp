#if OTR_GRAPHICS_NULL

#include "Graphics/Abstractions/Renderer.h"
#include "Graphics/Abstractions/Shader.h"
#include "Graphics/Abstractions/DataBuffer.h"

namespace Otter::Graphics
{
    // HELP: Renderer
    Renderer* Renderer::Create()
    {
        OTR_LOG_FATAL("NullGraphics is not a valid renderer!");
        return nullptr;
    }

    void Renderer::Destroy(Renderer* outRenderer)
    {
        OTR_LOG_FATAL("NullGraphics is not a valid renderer!");
        outRenderer = nullptr;
    }

    // HELP: Shader
    Shader* Shader::Create(const char* const filePath)
    {
        OTR_LOG_FATAL("NullGraphics is not a valid renderer!");
        return nullptr;
    }

    void Shader::Destroy(Shader* outShader)
    {
        OTR_LOG_FATAL("NullGraphics is not a valid renderer!");
        outShader = nullptr;
    }

    // HELP: Texture
    Texture* Texture::Create(const char* filePath)
    {
        OTR_LOG_FATAL("NullGraphics is not a valid renderer!");
        return nullptr;
    }

    void Texture::Destroy(Texture* outTexture)
    {
        OTR_LOG_FATAL("NullGraphics is not a valid renderer!");
        outTexture = nullptr;
    }
}

#endif