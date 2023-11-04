#ifndef OTTERENGINE_GRAPHICSSYSTEM_H
#define OTTERENGINE_GRAPHICSSYSTEM_H

namespace Otter::GraphicsSystem
{
    bool TryInitialise(const void* platformContext);
    void Shutdown();

    void RenderFrame();
}

#endif //OTTERENGINE_GRAPHICSSYSTEM_H
