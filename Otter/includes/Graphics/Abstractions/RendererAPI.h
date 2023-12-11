#ifndef OTTERENGINE_RENDERERAPI_H
#define OTTERENGINE_RENDERERAPI_H

#include "Core/Collections/Collection.h"
#include "Graphics/Abstractions/Shader.h"
#include "Graphics/Abstractions/Texture.h"

namespace Otter::Graphics
{
    class RendererAPI
    {
    public:
        virtual ~RendererAPI() = default;

        virtual void Initialise(const void* platformContext,
                                const Collection<Shader*>& shaders,
                                const Collection<Texture*>& textures) = 0;
        virtual void Shutdown() = 0;

        virtual bool TryBeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void DrawIndexed() = 0;

        [[nodiscard]] static RendererAPI* Create();
        static void Destroy(RendererAPI* outRenderer);

    protected:
        RendererAPI() = default;
    };
}

#endif //OTTERENGINE_RENDERERAPI_H
