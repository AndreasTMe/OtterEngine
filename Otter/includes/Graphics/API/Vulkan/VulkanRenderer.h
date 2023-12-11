#ifndef OTTERENGINE_VULKANRENDERER_H
#define OTTERENGINE_VULKANRENDERER_H

#include "Graphics/Abstractions/RendererAPI.h"
#include "Graphics/API/Vulkan/Types/VulkanTypes.Device.h"
#include "Graphics/API/Vulkan/Types/VulkanTypes.Swapchain.h"
#include "Graphics/API/Vulkan/Types/VulkanTypes.Descriptors.h"
#include "Graphics/API/Vulkan/VulkanShader.h"
#include "Graphics/API/Vulkan/VulkanTexture.h"
#include "Graphics/API/Vulkan/VulkanDataBuffer.h"

namespace Otter::Graphics::Vulkan
{
    class VulkanRenderer final : public RendererAPI
    {
    public:
        VulkanRenderer() : RendererAPI() { }
        ~VulkanRenderer() final = default;

        void Initialise(const void* platformContext,
                        const Collection<Shader*>& shaders,
                        const Collection<Texture*>& textures) final;
        void Shutdown() final;

        bool TryBeginFrame() final;
        void EndFrame() final;

        void DrawIndexed() final;

    private:

        VkAllocationCallbacks* m_Allocator = VK_NULL_HANDLE;

        VkInstance   m_Instance = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface  = VK_NULL_HANDLE;
#if !OTR_RUNTIME
        VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
#endif
        VulkanDevicePair    m_DevicePair;
        VulkanSwapchain     m_Swapchain;
        List<VkImage>       m_SwapchainImages;
        List<VkImageView>   m_SwapchainImageViews;
        List<VkFramebuffer> m_SwapchainFrameBuffers;
        VkRenderPass        m_RenderPass     = VK_NULL_HANDLE;
        VulkanDescriptor    m_Descriptor;
        VkPipelineLayout    m_PipelineLayout = VK_NULL_HANDLE;
        VkPipeline          m_Pipeline       = VK_NULL_HANDLE;

        VulkanVertexBuffer * m_VertexBuffer  = nullptr;
        VulkanIndexBuffer  * m_IndexBuffer   = nullptr;
        VulkanUniformBuffer* m_UniformBuffer = nullptr;

        // HELP: VkInstance related
        static void CreateVulkanInstance(const VkAllocationCallbacks* allocator, VkInstance* outInstance);
        static void DestroyVulkanInstance(const VkAllocationCallbacks* allocator, VkInstance* outInstance);

        // HELP: VkSurfaceKHR related
        static void CreateSurface(const void* platformContext,
                                  const VkInstance& instance,
                                  const VkAllocationCallbacks* allocator,
                                  VkSurfaceKHR* outSurface);
        static void DestroySurface(const VkInstance& instance,
                                   const VkAllocationCallbacks* allocator,
                                   VkSurfaceKHR* outSurface);

        // HELP: VkPhysicalDevice/VkDevice related
        static void SelectPhysicalDevice(const VkInstance& instance,
                                         const VkSurfaceKHR& surface,
                                         VulkanDevicePair* outDevicePair);
        static void CreateLogicalDevice(const VkAllocationCallbacks* allocator, VulkanDevicePair* outDevicePair);
        static void CreateDevicePairs(const VkInstance& instance,
                                      const VkSurfaceKHR& surface,
                                      const VkAllocationCallbacks* allocator,
                                      VulkanDevicePair* outDevicePair);
        static void DestroyDevicePairs(const VkAllocationCallbacks* allocator, VulkanDevicePair* outDevicePair);

        // HELP: VkSwapchainKHR related
        void CreateSwapchains();
        void RecreateSwapchains();
        void DestroySwapchains();

        // HELP: VkRenderPass related
        static void CreateRenderPass(const VkDevice& logicalDevice,
                                     const VkAllocationCallbacks* allocator,
                                     const VulkanSwapchain& swapchain,
                                     VkRenderPass* outRenderPass);
        static void DestroyRenderPass(const VkDevice& logicalDevice,
                                      const VkAllocationCallbacks* allocator,
                                      VkRenderPass* outRenderPass);

        // HELP: VkCommandPool/VkCommandBuffer related
        static void CreateCommandPool(const VulkanDevicePair& devicePair,
                                      const VkAllocationCallbacks* allocator,
                                      VkCommandPool* outCommandPool);
        static void DestroyCommandPool(const VulkanDevicePair& devicePair,
                                       const VkAllocationCallbacks* allocator,
                                       VkCommandPool* outCommandPool);
        static void CreateCommandBuffers(const VkDevice& logicalDevice,
                                         const VkCommandPool& commandPool,
                                         UInt32 commandBufferCount,
                                         List<VkCommandBuffer>& outCommandBuffers);
        static void DestroyCommandBuffers(List<VkCommandBuffer>& outCommandBuffers);

        // HELP: VkDescriptorSet related
        void CreateDescriptorSetLayout();
        void CreateDescriptorPool();
        void CreateDescriptorSets();
        void DestroyVulkanDescriptorData();

        // HELP: VkPipeline related
        void CreatePipelines();

        // HELP: VkBuffer related
        void CreateVertexBuffer();
        void CreateIndexBuffer();
        void CreateUniformBuffer();

        // HELP: VkSemaphore/VkFence related
        void CreateSyncObjects();
        void DestroySyncObjects();

#if !OTR_RUNTIME
        // HELP: VkDebugUtilsMessengerEXT related
        static void CreateVulkanDebugMessenger(const VkInstance& instance,
                                               const VkAllocationCallbacks* allocator,
                                               VkDebugUtilsMessengerEXT* outDebugMessenger);
        static void DestroyVulkanDebugMessenger(const VkInstance& instance,
                                                const VkAllocationCallbacks* allocator,
                                                VkDebugUtilsMessengerEXT* outDebugMessenger);
        static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        static VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                      [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
                                      const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                                      void* userData);

        static void GetRequiredInstanceValidationLayers(List<const char*>& layers);
#endif
    };
}

#endif //OTTERENGINE_VULKANRENDERER_H
