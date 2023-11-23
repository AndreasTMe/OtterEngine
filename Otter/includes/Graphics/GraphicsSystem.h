#ifndef OTTERENGINE_GRAPHICSSYSTEM_H
#define OTTERENGINE_GRAPHICSSYSTEM_H

#include "Graphics/Abstractions/RendererAPI.h"

namespace Otter
{
    class GraphicsSystem final
    {
    public:
        OTR_DISABLE_OBJECT_COPIES(GraphicsSystem)
        OTR_DISABLE_OBJECT_MOVES(GraphicsSystem)

        static bool TryInitialise(const void* platformContext);
        static void Shutdown();

        static void RenderFrame();

    private:
        OTR_DISABLE_CONSTRUCTION(GraphicsSystem)

        static Graphics::RendererAPI* s_Renderer;
    };
}

#endif //OTTERENGINE_GRAPHICSSYSTEM_H
