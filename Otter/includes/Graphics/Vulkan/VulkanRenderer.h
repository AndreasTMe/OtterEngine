#ifndef OTTERENGINE_VULKANRENDERER_H
#define OTTERENGINE_VULKANRENDERER_H

namespace Otter::Graphics::Vulkan
{
    void Initialise(const void* platformContext);
    void Shutdown();

    void RenderFrame();
}

#endif //OTTERENGINE_VULKANRENDERER_H
