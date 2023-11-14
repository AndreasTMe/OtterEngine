#ifndef OTTERENGINE_VULKANRENDERER_H
#define OTTERENGINE_VULKANRENDERER_H

#include "Core/Defines.h"
#include "Graphics/AbstractRenderer.h"
#include "Graphics/Vulkan/VulkanBase.Includes.h"
#include "Graphics/Vulkan/Types/VulkanTypes.Buffer.h"
#include "Graphics/Vulkan/Types/VulkanTypes.Device.h"
#include "Graphics/Vulkan/Types/VulkanTypes.Swapchain.h"

namespace Otter::Graphics::Vulkan
{
    class Renderer final : public AbstractRenderer
    {
    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR_AND_FINAL_DESTRUCTOR(Renderer)

        void Initialise(const void* platformContext) final;
        void Shutdown() final;

        void RenderFrame() final;

    private:
        VkAllocationCallbacks* m_Allocator = VK_NULL_HANDLE;

        VkInstance   m_Instance = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface  = VK_NULL_HANDLE;
#if !OTR_RUNTIME
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
#endif
        VulkanDevicePair     m_DevicePair;
        VulkanSwapchain      m_Swapchain;
        List <VkImage>       m_SwapchainImages;
        List <VkImageView>   m_SwapchainImageViews;
        List <VkFramebuffer> m_SwapchainFrameBuffers;

        VkRenderPass     m_RenderPass     = VK_NULL_HANDLE;
        VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
        VkPipeline       m_Pipeline       = VK_NULL_HANDLE;

        VulkanBuffer m_IndexBuffer{ };
        VulkanBuffer m_VertexBuffer{ };

        // HELP: VkInstance related
        void CreateVulkanInstance(const VkAllocationCallbacks* allocator, VkInstance* outInstance);
        void DestroyVulkanInstance(const VkAllocationCallbacks* allocator, VkInstance* outInstance);

        // HELP: VkSurfaceKHR related
        void CreateSurface(const void* platformContext,
                           const VkInstance& instance,
                           const VkAllocationCallbacks* allocator,
                           VkSurfaceKHR* outSurface);
        void DestroySurface(const VkInstance& instance,
                            const VkAllocationCallbacks* allocator,
                            VkSurfaceKHR* outSurface);

        // HELP: VkPhysicalDevice/VkDevice related
        void SelectPhysicalDevice(const VkInstance& instance,
                                  const VkSurfaceKHR& surface,
                                  VulkanDevicePair* outDevicePair);
        void CreateLogicalDevice(const VkAllocationCallbacks* allocator, VulkanDevicePair* outDevicePair);
        void CreateDevicePairs(const VkInstance& instance,
                               const VkSurfaceKHR& surface,
                               const VkAllocationCallbacks* allocator,
                               VulkanDevicePair* outDevicePair);
        void DestroyDevicePairs(const VkAllocationCallbacks* allocator, VulkanDevicePair* outDevicePair);

        // HELP: VkSwapchainKHR related
        void CreateSwapchains();
        void RecreateSwapchains();
        void DestroySwapchains();

        // HELP: VkRenderPass related
        void CreateRenderPass(const VkDevice& logicalDevice,
                              const VkAllocationCallbacks* allocator,
                              const VulkanSwapchain& swapchain,
                              VkRenderPass* outRenderPass);
        void DestroyRenderPass(const VkDevice& logicalDevice,
                               const VkAllocationCallbacks* allocator,
                               VkRenderPass* outRenderPass);

        // HELP: VkCommandPool/VkCommandBuffer related
        void CreateCommandPool(const VulkanDevicePair& devicePair,
                               const VkAllocationCallbacks* allocator,
                               VkCommandPool* outCommandPool);
        void DestroyCommandPool(const VulkanDevicePair& devicePair,
                                const VkAllocationCallbacks* allocator,
                                VkCommandPool* outCommandPool);
        void CreateCommandBuffers(const VkDevice& logicalDevice,
                                  const VkCommandPool& commandPool,
                                  const UInt32& commandBufferCount,
                                  List <VkCommandBuffer>& outCommandBuffers);
        void DestroyCommandBuffers(List <VkCommandBuffer>& outCommandBuffers);

        // HELP: VkPipeline related
        void CreatePipelines();

        // HELP: VkBuffer related
        void CreateVertexBuffer();
        void CreateIndexBuffer();

        // HELP: VkSemaphore/VkFence related
        void CreateSyncObjects();
        void DestroySyncObjects();

#if !OTR_RUNTIME
        // HELP: VkDebugUtilsMessengerEXT related
        void CreateVulkanDebugMessenger(const VkInstance& instance,
                                        const VkAllocationCallbacks* allocator,
                                        VkDebugUtilsMessengerEXT* outDebugMessenger);
        void DestroyVulkanDebugMessenger(const VkInstance& instance,
                                         const VkAllocationCallbacks* allocator,
                                         VkDebugUtilsMessengerEXT* outDebugMessenger);
        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        static VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                      [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
                                      const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                                      void* userData);

        void GetRequiredInstanceValidationLayers(List<const char*>& layers);
#endif
    };
}

#endif //OTTERENGINE_VULKANRENDERER_H
