#include "Otter.PCH.h"

#if OTR_GRAPHICS_NULL

#include "Graphics/Abstractions/Renderer.h"
#include "Graphics/Abstractions/Shader.h"
#include "Graphics/Abstractions/DataBuffer.h"

namespace Otter::Graphics
{
    // HELP: Renderer
    Renderer* Renderer::Create()
    {
        return nullptr;
    }

    void Renderer::Destroy(Renderer* outRenderer)
    {
        outRenderer = nullptr;
    }

    // HELP: Shader
    Shader* Shader::Create(const char* const filePath)
    {
        return nullptr;
    }

    // HELP: DataBuffer
    DataBuffer* DataBuffer::Create(BufferType type)
    {
        return nullptr;
    }

    void DataBuffer::Destroy(BufferType type, DataBuffer* outBuffer)
    {
        outBuffer = nullptr;
    }
}

#endif