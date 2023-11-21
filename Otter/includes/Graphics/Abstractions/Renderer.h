#ifndef OTTERENGINE_RENDERER_H
#define OTTERENGINE_RENDERER_H

#include "Core/Defines.h"
#include "Core/Collections/Collection.h"
#include "Graphics/Abstractions/Shader.h"
#include "Graphics/Abstractions/Texture.h"

namespace Otter::Graphics
{
    class Renderer
    {
    public:
        virtual ~Renderer() = default;

        virtual void Initialise(const void* platformContext,
                                const Collection<Shader*>& shaders,
                                const Collection<Texture*>& textures) = 0;
        virtual void Shutdown() = 0;

        virtual bool TryBeginFrame() = 0;
        virtual void EndFrame() = 0;

        virtual void DrawIndexed() = 0;

        [[nodiscard]] static Renderer* Create();
        static void Destroy(Renderer* outRenderer);

    protected:
        Renderer() = default;
    };
}

#endif //OTTERENGINE_RENDERER_H
