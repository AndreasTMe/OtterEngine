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

        OTR_INLINE static GraphicsSystem& GetInstance()
        {
            static GraphicsSystem instance;
            return instance;
        }

        bool TryInitialise(const void* platformContext);
        void Shutdown();

        void RenderFrame();

    private:
        OTR_WITH_DEFAULT_CONSTRUCTOR(GraphicsSystem)

        Graphics::RendererAPI* m_Renderer;
    };
}

#define OTR_GRAPHICS_SYSTEM Otter::GraphicsSystem::GetInstance()

#endif //OTTERENGINE_GRAPHICSSYSTEM_H
