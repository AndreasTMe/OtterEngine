#ifndef OTTERENGINE_VULKANRENDERER_H
#define OTTERENGINE_VULKANRENDERER_H

#include "Graphics/AbstractRenderer.h"
#include "VulkanContext.h"

namespace Otter::Graphics::Vulkan
{
    class VulkanRenderer final : public AbstractRenderer
    {
    public:
        void Initialise(const void* platformContext) final;
        void Shutdown() final;

    private:
        VulkanContext* m_Context = nullptr;

        void CreateVulkanInstance();
        void SetupVulkanDebugMessenger();

        static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        static VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                      VkDebugUtilsMessageTypeFlagsEXT messageType,
                                      const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                                      void* userData);
    };
}

#endif //OTTERENGINE_VULKANRENDERER_H
