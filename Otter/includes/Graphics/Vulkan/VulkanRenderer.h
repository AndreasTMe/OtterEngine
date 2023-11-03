#ifndef OTTERENGINE_VULKANRENDERER_H
#define OTTERENGINE_VULKANRENDERER_H

#include "Graphics/AbstractRenderer.h"
#include "Graphics/Vulkan/VulkanBase.h"

namespace Otter::Graphics::Vulkan
{
    class VulkanRenderer final : public AbstractRenderer
    {
    public:
        bool TryInitialise(const void* windowHandle) final;
        void Shutdown() final;

    private:
        VkInstance m_Instance = VK_NULL_HANDLE;
    };
}

#endif //OTTERENGINE_VULKANRENDERER_H
