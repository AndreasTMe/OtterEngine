#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanRenderer.h"
#include "Graphics/Vulkan/Types/VulkanTypes.Context.h"
#include "Graphics/Vulkan/Types/VulkanTypes.Shader.h"
#include "Graphics/Vulkan/VulkanExtensions.h"
#include "Graphics/Vulkan/VulkanQueues.h"
#include "Graphics/Vulkan/VulkanSwapchains.h"
#include "Graphics/Vulkan/VulkanShaders.h"
#include "Math/Core.h"

namespace Otter::Graphics::Vulkan
{
    namespace Internal
    {
        // HELP: VkInstance related
        void CreateVulkanInstance();
        void DestroyVulkanInstance();

        // HELP: VkSurfaceKHR related
        void CreateSurface(const void* platformContext);
        void DestroySurface();

        // HELP: VkPhysicalDevice & VkDevice related
        void CreateDevicePairs();
        void DestroyDevicePairs();
        void SelectPhysicalDevice();
        void CreateLogicalDevice();

        // HELP: VkSwapchainKHR related
        void CreateSwapchains();
        void RecreateSwapchains();
        void DestroySwapchains();

        // HELP: VkRenderPass related
        void CreateRenderPass();
        void DestroyRenderPass();

        // HELP: VkCommandPool & VkCommandBuffer related
        void CreateCommandBuffers();
        void DestroyCommandBuffers();

        // HELP: VkSemaphore & VkFence related
        void CreateSyncObjects();
        void DestroySyncObjects();

        // HELP: VkPipeline related
        void CreatePipelines();
        void DestroyPipelines();

#if !OTR_RUNTIME
        // HELP: VkDebugUtilsMessenger related
        void CreateVulkanDebugMessenger();
        void DestroyVulkanDebugMessenger();

        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        VkBool32 DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                               [[maybe_unused]] VkDebugUtilsMessageTypeFlagsEXT messageType,
                               const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                               void* userData);

        void GetRequiredInstanceValidationLayers(List<const char*>& layers);
#endif
    }

    static VulkanContext* gs_Context = nullptr;

    List <VkImage>       g_SwapchainImages;
    List <VkImageView>   g_SwapchainImageViews;
    List <VkFramebuffer> g_SwapchainFrameBuffers;

    // TODO: Remove from here, temporary handles
    VkPipeline       g_Pipeline       = VK_NULL_HANDLE;
    VkPipelineLayout g_PipelineLayout = VK_NULL_HANDLE;

#define OTR_VULKAN_SYNC_TIMEOUT 1000000000

    void Initialise(const void* const platformContext)
    {
        gs_Context = New<VulkanContext>();

        Internal::CreateVulkanInstance();

#if !OTR_RUNTIME
        Internal::CreateVulkanDebugMessenger();
#endif

        Internal::CreateSurface(platformContext);
        Internal::CreateDevicePairs();
        Internal::CreateSwapchains();
        Internal::CreateRenderPass();
        CreateSwapchainFrameBuffers(gs_Context->DevicePair.LogicalDevice,
                                    gs_Context->Allocator,
                                    gs_Context->Swapchain.Extent,
                                    g_SwapchainImageViews,
                                    gs_Context->RenderPass,
                                    g_SwapchainFrameBuffers);

        Internal::CreateCommandBuffers();

        Internal::CreateSyncObjects();

        Internal::CreatePipelines();
    }

    void Shutdown()
    {
        vkDeviceWaitIdle(gs_Context->DevicePair.LogicalDevice);

        Internal::DestroyCommandBuffers();

        Internal::DestroyPipelines();
        Internal::DestroySyncObjects();
        Internal::DestroyRenderPass();
        Internal::DestroySwapchains();
        Internal::DestroyDevicePairs();
        Internal::DestroySurface();

#if !OTR_RUNTIME
        Internal::DestroyVulkanDebugMessenger();
#endif

        Internal::DestroyVulkanInstance();

        Delete(gs_Context);
    }

    void RenderFrame()
    {
        const auto currentFrame = gs_Context->Swapchain.CurrentFrame;

        OTR_VULKAN_VALIDATE(vkWaitForFences(gs_Context->DevicePair.LogicalDevice,
                                            1,
                                            &gs_Context->DevicePair.RenderInFlightFences[0],
                                            VK_TRUE,
                                            OTR_VULKAN_SYNC_TIMEOUT))
        OTR_VULKAN_VALIDATE(vkResetFences(gs_Context->DevicePair.LogicalDevice,
                                          1,
                                          &gs_Context->DevicePair.RenderInFlightFences[0]))

        UInt32 imageIndex;
        OTR_VULKAN_VALIDATE(vkAcquireNextImageKHR(gs_Context->DevicePair.LogicalDevice,
                                                  gs_Context->Swapchain.Handle,
                                                  OTR_VULKAN_SYNC_TIMEOUT,
                                                  gs_Context->DevicePair.ImageAvailableSemaphores[0],
                                                  VK_NULL_HANDLE,
                                                  &imageIndex))

        OTR_VULKAN_VALIDATE(vkResetCommandBuffer(gs_Context->DevicePair.CommandBuffers[0],
                                                 0))

        VkCommandBufferBeginInfo beginInfo{ };
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        OTR_VULKAN_VALIDATE(vkBeginCommandBuffer(gs_Context->DevicePair.CommandBuffers[0],
                                                 &beginInfo))

        VkClearValue clearColor = {{{ 0.3f, 0.2f, 0.5f, 1.0f }}}; // TODO: Remove this later

        VkRenderPassBeginInfo renderPassInfo{ };
        renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass        = gs_Context->RenderPass;
        renderPassInfo.framebuffer       = g_SwapchainFrameBuffers[imageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = gs_Context->Swapchain.Extent;
        renderPassInfo.clearValueCount   = 1;
        renderPassInfo.pClearValues      = &clearColor;

        vkCmdBeginRenderPass(gs_Context->DevicePair.CommandBuffers[0],
                             &renderPassInfo,
                             VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(gs_Context->DevicePair.CommandBuffers[0],
                          VK_PIPELINE_BIND_POINT_GRAPHICS,
                          g_Pipeline);
        vkCmdDraw(gs_Context->DevicePair.CommandBuffers[0], 3, 1, 0, 0);

        vkCmdEndRenderPass(gs_Context->DevicePair.CommandBuffers[0]);

        OTR_VULKAN_VALIDATE(vkEndCommandBuffer(gs_Context->DevicePair.CommandBuffers[0]))

        VkPipelineStageFlags waitStages[]       = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSemaphore          waitSemaphores[]   = { gs_Context->DevicePair.ImageAvailableSemaphores[0] };
        VkSemaphore          signalSemaphores[] = { gs_Context->DevicePair.RenderFinishedSemaphores[0] };

        VkSubmitInfo submitInfo{ };
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &gs_Context->DevicePair.CommandBuffers[0];
        submitInfo.pWaitDstStageMask    = waitStages;
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pWaitSemaphores      = waitSemaphores;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = signalSemaphores;

        OTR_VULKAN_VALIDATE(vkQueueSubmit(gs_Context->DevicePair.GraphicsQueueFamily.Handle,
                                          1,
                                          &submitInfo,
                                          gs_Context->DevicePair.RenderInFlightFences[0]))

        VkSwapchainKHR swapChains[] = { gs_Context->Swapchain.Handle };

        VkPresentInfoKHR presentInfo{ };
        presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores    = signalSemaphores;
        presentInfo.swapchainCount     = 1;
        presentInfo.pSwapchains        = swapChains;
        presentInfo.pImageIndices      = &imageIndex;

        OTR_VULKAN_VALIDATE(vkQueuePresentKHR(gs_Context->DevicePair.PresentationQueueFamily.Handle,
                                              &presentInfo))

        gs_Context->Swapchain.CurrentFrame = (currentFrame + 1) % gs_Context->Swapchain.MaxFramesInFlight;
    }

    namespace Internal
    {
        void CreateVulkanInstance()
        {
            OTR_LOG_TRACE("Creating Vulkan instance...")

            VkApplicationInfo appInfo{ };
            appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            appInfo.pApplicationName   = "Otter Engine";
            appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
            appInfo.pEngineName        = "Otter Engine";
            appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
            appInfo.apiVersion         = VK_API_VERSION_1_0;

            VkInstanceCreateInfo createInfo{ };
            createInfo.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            createInfo.pApplicationInfo = &appInfo;

            OTR_LOG_TRACE("Getting required instance extensions and layers...")

            List<const char*> extensions;
            GetRequiredInstanceExtensions(extensions);

            createInfo.enabledExtensionCount   = extensions.GetCount();
            createInfo.ppEnabledExtensionNames = extensions.GetData();

#if !OTR_RUNTIME
            OTR_LOG_TRACE("Creating Vulkan debug messenger...")

            VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{ };
            PopulateDebugMessengerCreateInfo(debugCreateInfo);

            List<const char*> layers;
            GetRequiredInstanceValidationLayers(layers);

            createInfo.enabledLayerCount   = layers.GetCount();
            createInfo.ppEnabledLayerNames = layers.GetData();
            createInfo.pNext               = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
#else
            createInfo.enabledLayerCount   = 0;
            createInfo.ppEnabledLayerNames = VK_NULL_HANDLE;
            createInfo.pNext               = VK_NULL_HANDLE;
#endif

            OTR_VULKAN_VALIDATE(vkCreateInstance(&createInfo, gs_Context->Allocator, &gs_Context->Instance))
        }

        void DestroyVulkanInstance()
        {
            vkDestroyInstance(gs_Context->Instance, gs_Context->Allocator);
        }

        void CreateSurface(const void* const platformContext)
        {
#if OTR_PLATFORM_WINDOWS
            const auto* const platformContextData       = ((PlatformContext*) platformContext)->Data;
            const auto* const windowsPlatformWindowData =
                          (Otter::Internal::WindowsPlatformWindowData*) platformContextData;

            VkWin32SurfaceCreateInfoKHR createInfo = { };
            createInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
            createInfo.hwnd      = windowsPlatformWindowData->WindowHandle;
            createInfo.hinstance = windowsPlatformWindowData->InstanceHandle;

            OTR_VULKAN_VALIDATE(vkCreateWin32SurfaceKHR(gs_Context->Instance,
                                                        &createInfo,
                                                        gs_Context->Allocator,
                                                        &gs_Context->Surface))
#else
            OTR_LOG_FATAL("'CreateSurface' not supported for this platform")
#endif
        }

        void DestroySurface()
        {
            vkDestroySurfaceKHR(gs_Context->Instance, gs_Context->Surface, gs_Context->Allocator);
        }

        void CreateDevicePairs()
        {
            OTR_INTERNAL_ASSERT_MSG(gs_Context->Instance != VK_NULL_HANDLE,
                                    "Instance must be initialized before creating device pairs")
            OTR_INTERNAL_ASSERT_MSG(gs_Context->Surface != VK_NULL_HANDLE,
                                    "Surface must be initialized before creating device pairs")

            OTR_LOG_TRACE("Creating Vulkan device pairs...")

            SelectPhysicalDevice();
            CreateLogicalDevice();
        }

        void DestroyDevicePairs()
        {
            OTR_INTERNAL_ASSERT_MSG(gs_Context->DevicePair.LogicalDevice != VK_NULL_HANDLE,
                                    "Logical device must be initialized before destroying device pairs")

            vkDestroyDevice(gs_Context->DevicePair.LogicalDevice, gs_Context->Allocator);
        }

        void SelectPhysicalDevice()
        {
            UInt32 deviceCount = 0;
            OTR_VULKAN_VALIDATE(vkEnumeratePhysicalDevices(gs_Context->Instance,
                                                           &deviceCount,
                                                           VK_NULL_HANDLE))

            OTR_INTERNAL_ASSERT_MSG(deviceCount > 0, "Failed to find GPUs with Vulkan support")

            VkPhysicalDevice queriedDevices[deviceCount];
            OTR_VULKAN_VALIDATE(vkEnumeratePhysicalDevices(gs_Context->Instance, &deviceCount, queriedDevices))

            for (const auto& queriedDevice: queriedDevices)
            {
                VkPhysicalDeviceProperties deviceProperties;
                vkGetPhysicalDeviceProperties(queriedDevice, &deviceProperties);

                OTR_LOG_TRACE("Checking GPU: {0}", deviceProperties.deviceName)

                // HELP: Require a discrete GPU
                if (deviceProperties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                {
                    OTR_LOG_TRACE("{0} is not a discrete GPU. Moving to the next one...", deviceProperties.deviceName)
                    continue;
                }

                // HELP: Require a GPU with graphics and presentation queues
                UInt32 graphicsFamily = UINT32_MAX;
                UInt32 presentFamily  = UINT32_MAX;
                QueryQueueFamilies(gs_Context->Surface, queriedDevice, graphicsFamily, presentFamily);

                if (graphicsFamily == UINT32_MAX || presentFamily == UINT32_MAX)
                {
                    OTR_LOG_TRACE("{0} is not suitable due to missing Graphics and/or Presentation queue. "
                                  "Moving to the next one...", deviceProperties.deviceName)

                    continue;
                }

                // HELP: Require a GPU with all required extensions
                if (!DeviceSupportsRequiredExtensions(queriedDevice))
                {
                    OTR_LOG_TRACE("{0} is not suitable due to missing required extensions. "
                                  "Moving to the next one...", deviceProperties.deviceName)
                    continue;
                }

                // HELP: Require a GPU with swapchain support
                SwapchainSupportInfo swapchainSupportInfo;
                QuerySwapchainSupport(gs_Context->Surface, queriedDevice, swapchainSupportInfo);

                bool isSwapchainSupported = swapchainSupportInfo.SurfaceFormats.GetCount() > 0
                                            && swapchainSupportInfo.PresentModes.GetCount() > 0;

                if (!isSwapchainSupported)
                {
                    OTR_LOG_TRACE("{0} is not suitable due to missing swapchain support. "
                                  "Moving to the next one...", deviceProperties.deviceName)
                    continue;
                }

                gs_Context->DevicePair.PhysicalDevice                = queriedDevice;
                gs_Context->DevicePair.GraphicsQueueFamily.Index     = graphicsFamily;
                gs_Context->DevicePair.PresentationQueueFamily.Index = presentFamily;

                OTR_LOG_TRACE("Found a suitable GPU: {0}", deviceProperties.deviceName)

                break;
            }

            OTR_INTERNAL_ASSERT_MSG(gs_Context->DevicePair.PhysicalDevice != VK_NULL_HANDLE,
                                    "Failed to find a suitable GPU")
        }

        void CreateLogicalDevice()
        {
            List <VkDeviceQueueCreateInfo> queueCreateInfos;

            HashSet <UInt32> uniqueQueueFamilies(2);
            uniqueQueueFamilies.TryAdd(gs_Context->DevicePair.GraphicsQueueFamily.Index);
            uniqueQueueFamilies.TryAdd(gs_Context->DevicePair.PresentationQueueFamily.Index);

            float queuePriority = 1.0f;

            Action<const UInt32&> addQueueCreateInfo;
            addQueueCreateInfo += [&](UInt32 queueFamily)
            {
                VkDeviceQueueCreateInfo queueCreateInfo{ };
                queueCreateInfo.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queueCreateInfo.queueFamilyIndex = queueFamily;
                queueCreateInfo.queueCount       = 1;
                queueCreateInfo.pQueuePriorities = &queuePriority;
                queueCreateInfos.Add(queueCreateInfo);
            };
            uniqueQueueFamilies.ForEach(addQueueCreateInfo);
            addQueueCreateInfo.ClearDestructive();

            // TODO: Should be configuration driven, all features are disabled by default for now
            VkPhysicalDeviceFeatures deviceFeatures{ };

            List<const char*> requiredExtensions;
            GetDeviceRequiredExtensions(requiredExtensions);

            VkDeviceCreateInfo createInfo{ };
            createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            createInfo.queueCreateInfoCount    = queueCreateInfos.GetCount();
            createInfo.pQueueCreateInfos       = queueCreateInfos.GetData();
            createInfo.pEnabledFeatures        = &deviceFeatures;
            createInfo.enabledExtensionCount   = requiredExtensions.GetCount();
            createInfo.ppEnabledExtensionNames = requiredExtensions.GetData();
            createInfo.enabledLayerCount       = 0;
            createInfo.ppEnabledLayerNames     = VK_NULL_HANDLE;

            OTR_VULKAN_VALIDATE(vkCreateDevice(gs_Context->DevicePair.PhysicalDevice,
                                               &createInfo,
                                               VK_NULL_HANDLE,
                                               &gs_Context->DevicePair.LogicalDevice))

            OTR_INTERNAL_ASSERT_MSG(gs_Context->DevicePair.LogicalDevice != VK_NULL_HANDLE,
                                    "Failed to create logical device")

            vkGetDeviceQueue(gs_Context->DevicePair.LogicalDevice,
                             gs_Context->DevicePair.GraphicsQueueFamily.Index,
                             0,
                             &gs_Context->DevicePair.GraphicsQueueFamily.Handle);
            vkGetDeviceQueue(gs_Context->DevicePair.LogicalDevice,
                             gs_Context->DevicePair.PresentationQueueFamily.Index,
                             0,
                             &gs_Context->DevicePair.PresentationQueueFamily.Handle);

            OTR_INTERNAL_ASSERT_MSG(gs_Context->DevicePair.GraphicsQueueFamily.Handle != VK_NULL_HANDLE,
                                    "Failed to get graphics queue")
            OTR_INTERNAL_ASSERT_MSG(gs_Context->DevicePair.PresentationQueueFamily.Handle != VK_NULL_HANDLE,
                                    "Failed to get presentation queue")
        }

        void CreateSwapchains()
        {
            OTR_LOG_TRACE("Creating Vulkan swapchains...")

            CreateSingleSwapchain(gs_Context->Surface,
                                  gs_Context->DevicePair,
                                  gs_Context->Allocator,
                                  gs_Context->Swapchain);

            CreateSwapchainImages(gs_Context->DevicePair.LogicalDevice,
                                  gs_Context->Swapchain.Handle,
                                  g_SwapchainImages);

            CreateSwapchainImageViews(gs_Context->DevicePair.LogicalDevice,
                                      gs_Context->Allocator,
                                      g_SwapchainImages,
                                      gs_Context->Swapchain.SurfaceFormat.format,
                                      g_SwapchainImageViews);
        }

        void RecreateSwapchains()
        {
            OTR_LOG_TRACE("Recreating Vulkan swapchain...")

            DestroySwapchains();

            CreateSingleSwapchain(gs_Context->Surface,
                                  gs_Context->DevicePair,
                                  gs_Context->Allocator,
                                  gs_Context->Swapchain);

            CreateSwapchainImages(gs_Context->DevicePair.LogicalDevice,
                                  gs_Context->Swapchain.Handle,
                                  g_SwapchainImages);

            CreateSwapchainImageViews(gs_Context->DevicePair.LogicalDevice,
                                      gs_Context->Allocator,
                                      g_SwapchainImages,
                                      gs_Context->Swapchain.SurfaceFormat.format,
                                      g_SwapchainImageViews);

            CreateSwapchainFrameBuffers(gs_Context->DevicePair.LogicalDevice,
                                        gs_Context->Allocator,
                                        gs_Context->Swapchain.Extent,
                                        g_SwapchainImageViews,
                                        gs_Context->RenderPass,
                                        g_SwapchainFrameBuffers);
        }

        void DestroySwapchains()
        {
            OTR_INTERNAL_ASSERT_MSG(gs_Context->DevicePair.LogicalDevice != VK_NULL_HANDLE,
                                    "Logical device must be initialized before destroying its swapchain")

            for (auto& swapchainFrameBuffer: g_SwapchainFrameBuffers)
            {
                vkDestroyFramebuffer(gs_Context->DevicePair.LogicalDevice,
                                     swapchainFrameBuffer,
                                     gs_Context->Allocator);
            }
            g_SwapchainFrameBuffers.ClearDestructive();

            for (auto& swapchainImageView: g_SwapchainImageViews)
            {
                vkDestroyImageView(gs_Context->DevicePair.LogicalDevice,
                                   swapchainImageView,
                                   gs_Context->Allocator);
            }
            g_SwapchainImageViews.ClearDestructive();
            g_SwapchainImages.ClearDestructive();

            vkDestroySwapchainKHR(gs_Context->DevicePair.LogicalDevice,
                                  gs_Context->Swapchain.Handle,
                                  gs_Context->Allocator);
        }

        void CreateRenderPass()
        {
            VkAttachmentDescription colorAttachment{ };
            colorAttachment.format         = gs_Context->Swapchain.SurfaceFormat.format;
            colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
            colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
            colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
            colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

            VkAttachmentReference colorAttachmentRef{ };
            colorAttachmentRef.attachment = 0;
            colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

            VkSubpassDescription subPass{ };
            subPass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
            subPass.colorAttachmentCount = 1;
            subPass.pColorAttachments    = &colorAttachmentRef;

            VkSubpassDependency dependency{ };
            dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
            dependency.dstSubpass    = 0;
            dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.srcAccessMask = 0;
            dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

            VkRenderPassCreateInfo renderPassInfo{ };
            renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
            renderPassInfo.attachmentCount = 1;
            renderPassInfo.pAttachments    = &colorAttachment;
            renderPassInfo.subpassCount    = 1;
            renderPassInfo.pSubpasses      = &subPass;
            renderPassInfo.dependencyCount = 1;
            renderPassInfo.pDependencies   = &dependency;

            OTR_VULKAN_VALIDATE(vkCreateRenderPass(gs_Context->DevicePair.LogicalDevice,
                                                   &renderPassInfo,
                                                   gs_Context->Allocator,
                                                   &gs_Context->RenderPass))
        }

        void DestroyRenderPass()
        {
            vkDestroyRenderPass(gs_Context->DevicePair.LogicalDevice,
                                gs_Context->RenderPass,
                                gs_Context->Allocator);
        }

        void CreateCommandBuffers()
        {
            VkCommandPoolCreateInfo commandPoolCreateInfo{ };
            commandPoolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            commandPoolCreateInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
            commandPoolCreateInfo.queueFamilyIndex = gs_Context->DevicePair.GraphicsQueueFamily.Index;
            commandPoolCreateInfo.pNext            = VK_NULL_HANDLE;

            OTR_VULKAN_VALIDATE(vkCreateCommandPool(gs_Context->DevicePair.LogicalDevice,
                                                    &commandPoolCreateInfo,
                                                    gs_Context->Allocator,
                                                    &gs_Context->DevicePair.GraphicsCommandPool))

            VkCommandBufferAllocateInfo commandBufferAllocateInfo{ };
            commandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            commandBufferAllocateInfo.commandPool        = gs_Context->DevicePair.GraphicsCommandPool;
            commandBufferAllocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            commandBufferAllocateInfo.commandBufferCount = gs_Context->Swapchain.MaxFramesInFlight;
            commandBufferAllocateInfo.pNext              = VK_NULL_HANDLE;

            VkCommandBuffer tempCommandBuffers[gs_Context->Swapchain.MaxFramesInFlight];
            OTR_VULKAN_VALIDATE(vkAllocateCommandBuffers(gs_Context->DevicePair.LogicalDevice,
                                                         &commandBufferAllocateInfo,
                                                         tempCommandBuffers))

            Collections::New<VkCommandBuffer>(tempCommandBuffers,
                                              gs_Context->Swapchain.MaxFramesInFlight,
                                              gs_Context->DevicePair.CommandBuffers);
        }

        void DestroyCommandBuffers()
        {
            OTR_INTERNAL_ASSERT_MSG(gs_Context->DevicePair.GraphicsCommandPool != VK_NULL_HANDLE,
                                    "Vulkan command pool is null!")
            OTR_INTERNAL_ASSERT_MSG(gs_Context->DevicePair.LogicalDevice != VK_NULL_HANDLE,
                                    "Vulkan logical device is null!")

            vkDestroyCommandPool(gs_Context->DevicePair.LogicalDevice,
                                 gs_Context->DevicePair.GraphicsCommandPool,
                                 gs_Context->Allocator);

            gs_Context->DevicePair.CommandBuffers.ClearDestructive();
        }

        void CreateSyncObjects()
        {
            gs_Context->DevicePair.ImageAvailableSemaphores.Reserve(gs_Context->Swapchain.MaxFramesInFlight);
            gs_Context->DevicePair.RenderFinishedSemaphores.Reserve(gs_Context->Swapchain.MaxFramesInFlight);
            gs_Context->DevicePair.RenderInFlightFences.Reserve(gs_Context->Swapchain.MaxFramesInFlight);

            VkSemaphoreCreateInfo semaphoreInfo{ };
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            VkFenceCreateInfo fenceInfo{ };
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            VkSemaphore tempImageAvailableSemaphores[gs_Context->Swapchain.MaxFramesInFlight];
            VkSemaphore tempRenderFinishedSemaphores[gs_Context->Swapchain.MaxFramesInFlight];
            VkFence     tempInFlightFences[gs_Context->Swapchain.MaxFramesInFlight];

            for (size_t i = 0; i < gs_Context->Swapchain.MaxFramesInFlight; i++)
            {
                OTR_VULKAN_VALIDATE(vkCreateSemaphore(gs_Context->DevicePair.LogicalDevice,
                                                      &semaphoreInfo,
                                                      gs_Context->Allocator,
                                                      &tempImageAvailableSemaphores[i]))
                OTR_VULKAN_VALIDATE(vkCreateSemaphore(gs_Context->DevicePair.LogicalDevice,
                                                      &semaphoreInfo,
                                                      gs_Context->Allocator,
                                                      &tempRenderFinishedSemaphores[i]))
                OTR_VULKAN_VALIDATE(vkCreateFence(gs_Context->DevicePair.LogicalDevice,
                                                  &fenceInfo,
                                                  gs_Context->Allocator,
                                                  &tempInFlightFences[i]))
            }

            Collections::New<VkSemaphore>(tempImageAvailableSemaphores,
                                          gs_Context->Swapchain.MaxFramesInFlight,
                                          gs_Context->DevicePair.ImageAvailableSemaphores);
            Collections::New<VkSemaphore>(tempRenderFinishedSemaphores,
                                          gs_Context->Swapchain.MaxFramesInFlight,
                                          gs_Context->DevicePair.RenderFinishedSemaphores);
            Collections::New<VkFence>(tempInFlightFences,
                                      gs_Context->Swapchain.MaxFramesInFlight,
                                      gs_Context->DevicePair.RenderInFlightFences);
        }

        void DestroySyncObjects()
        {
            for (size_t i = 0; i < gs_Context->Swapchain.MaxFramesInFlight; i++)
            {
                vkDestroySemaphore(gs_Context->DevicePair.LogicalDevice,
                                   gs_Context->DevicePair.ImageAvailableSemaphores[i],
                                   gs_Context->Allocator);
                vkDestroySemaphore(gs_Context->DevicePair.LogicalDevice,
                                   gs_Context->DevicePair.RenderFinishedSemaphores[i],
                                   gs_Context->Allocator);
                vkDestroyFence(gs_Context->DevicePair.LogicalDevice,
                               gs_Context->DevicePair.RenderInFlightFences[i],
                               gs_Context->Allocator);
            }

            gs_Context->DevicePair.ImageAvailableSemaphores.ClearDestructive();
            gs_Context->DevicePair.RenderFinishedSemaphores.ClearDestructive();
            gs_Context->DevicePair.RenderInFlightFences.ClearDestructive();
        }

        void CreatePipelines()
        {
            VkShaderModuleCreateInfo vertexShaderModuleCreateInfo;
            if (!LoadShaderModule("Assets/Shaders/default.vert.spv", &vertexShaderModuleCreateInfo))
            {
                OTR_LOG_WARNING("Error when building the vertex shader module")
            }
            VkShaderModule vertexShaderModule;
            OTR_VULKAN_VALIDATE(vkCreateShaderModule(gs_Context->DevicePair.LogicalDevice,
                                                     &vertexShaderModuleCreateInfo,
                                                     gs_Context->Allocator,
                                                     &vertexShaderModule))

            VkShaderModuleCreateInfo fragmentShaderModuleCreateInfo;
            if (!LoadShaderModule("Assets/Shaders/default.frag.spv", &fragmentShaderModuleCreateInfo))
            {
                OTR_LOG_WARNING("Error when building the fragment shader module")
            }
            VkShaderModule fragmentShaderModule;
            OTR_VULKAN_VALIDATE(vkCreateShaderModule(gs_Context->DevicePair.LogicalDevice,
                                                     &fragmentShaderModuleCreateInfo,
                                                     gs_Context->Allocator,
                                                     &fragmentShaderModule))

            VkPipelineShaderStageCreateInfo vertShaderStageInfo{ };
            vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
            vertShaderStageInfo.module = vertexShaderModule;
            vertShaderStageInfo.pName  = "main";
            vertShaderStageInfo.pNext  = VK_NULL_HANDLE;

            VkPipelineShaderStageCreateInfo fragShaderStageInfo{ };
            fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
            fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
            fragShaderStageInfo.module = fragmentShaderModule;
            fragShaderStageInfo.pName  = "main";
            fragShaderStageInfo.pNext  = VK_NULL_HANDLE;

            VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

            // Vertex Input
            VkPipelineVertexInputStateCreateInfo vertexInputInfo{ };
            vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexBindingDescriptionCount   = 0;
            vertexInputInfo.vertexAttributeDescriptionCount = 0;
            vertexInputInfo.pNext                           = VK_NULL_HANDLE;

            // Input Assembly
            VkPipelineInputAssemblyStateCreateInfo inputAssembly{ };
            inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;
            inputAssembly.pNext                  = VK_NULL_HANDLE;

            // Viewport and Scissor
            VkViewport viewport{ };
            viewport.x        = 0.0f;
            viewport.y        = 0.0f;
            viewport.width    = (float) gs_Context->Swapchain.Extent.width;
            viewport.height   = (float) gs_Context->Swapchain.Extent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            VkRect2D scissor{ };
            scissor.offset = { 0, 0 };
            scissor.extent = gs_Context->Swapchain.Extent;

            VkPipelineViewportStateCreateInfo viewportState{ };
            viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = 1;
            viewportState.pViewports    = &viewport;
            viewportState.scissorCount  = 1;
            viewportState.pScissors     = &scissor;
            viewportState.pNext         = VK_NULL_HANDLE;

            // Rasterizer
            VkPipelineRasterizationStateCreateInfo rasterizer{ };
            rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable        = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth               = 1.0f;
            rasterizer.cullMode                = VK_CULL_MODE_NONE;
            rasterizer.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterizer.depthBiasEnable         = VK_FALSE;
            rasterizer.depthBiasConstantFactor = 0.0f; // Optional
            rasterizer.depthBiasClamp          = 0.0f; // Optional
            rasterizer.depthBiasSlopeFactor    = 0.0f; // Optional
            rasterizer.pNext                   = VK_NULL_HANDLE;

            // Multisampling
            VkPipelineMultisampleStateCreateInfo multisampling{ };
            multisampling.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable   = VK_FALSE;
            multisampling.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
            multisampling.minSampleShading      = 1.0f; // Optional
            multisampling.pSampleMask           = VK_NULL_HANDLE; // Optional
            multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
            multisampling.alphaToOneEnable      = VK_FALSE; // Optional
            multisampling.pNext                 = VK_NULL_HANDLE;

            // Color blending
            VkPipelineColorBlendAttachmentState colorBlendAttachment{ }; // Per attached framebuffer
            colorBlendAttachment.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT
                                                       | VK_COLOR_COMPONENT_G_BIT
                                                       | VK_COLOR_COMPONENT_B_BIT
                                                       | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable         = VK_FALSE;
            colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.colorBlendOp        = VK_BLEND_OP_ADD; // Optional
            colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
            colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
            colorBlendAttachment.alphaBlendOp        = VK_BLEND_OP_ADD; // Optional

            VkPipelineColorBlendStateCreateInfo colorBlending{ }; // Global
            colorBlending.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable   = VK_FALSE;
            colorBlending.logicOp         = VK_LOGIC_OP_COPY; // Optional
            colorBlending.attachmentCount = 1;
            colorBlending.pAttachments    = &colorBlendAttachment;
            colorBlending.blendConstants[0] = 0.0f; // Optional
            colorBlending.blendConstants[1] = 0.0f; // Optional
            colorBlending.blendConstants[2] = 0.0f; // Optional
            colorBlending.blendConstants[3] = 0.0f; // Optional
            colorBlending.pNext = VK_NULL_HANDLE;

            // Pipeline Layout
            VkPipelineLayoutCreateInfo pipelineLayoutInfo{ };
            pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.flags                  = 0;
            pipelineLayoutInfo.setLayoutCount         = 0;
            pipelineLayoutInfo.pSetLayouts            = VK_NULL_HANDLE;
            pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
            pipelineLayoutInfo.pPushConstantRanges    = VK_NULL_HANDLE; // Optional
            pipelineLayoutInfo.pNext                  = VK_NULL_HANDLE;

            OTR_VULKAN_VALIDATE(vkCreatePipelineLayout(gs_Context->DevicePair.LogicalDevice,
                                                       &pipelineLayoutInfo,
                                                       gs_Context->Allocator,
                                                       &g_PipelineLayout))

            // Graphics Pipeline
            VkGraphicsPipelineCreateInfo pipelineInfo{ };
            pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount          = 2;
            pipelineInfo.pStages             = shaderStages;
            pipelineInfo.pVertexInputState   = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState      = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState   = &multisampling;
            pipelineInfo.pDepthStencilState  = VK_NULL_HANDLE; // Optional
            pipelineInfo.pColorBlendState    = &colorBlending;
            pipelineInfo.pDynamicState       = VK_NULL_HANDLE;
            pipelineInfo.layout              = g_PipelineLayout;
            pipelineInfo.renderPass          = gs_Context->RenderPass;
            pipelineInfo.subpass             = 0;
            pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE; // Optional
            pipelineInfo.basePipelineIndex   = -1; // Optional
            pipelineInfo.pNext               = VK_NULL_HANDLE;

            OTR_VULKAN_VALIDATE(vkCreateGraphicsPipelines(gs_Context->DevicePair.LogicalDevice,
                                                          VK_NULL_HANDLE,
                                                          1,
                                                          &pipelineInfo,
                                                          gs_Context->Allocator,
                                                          &g_Pipeline))

            vkDestroyShaderModule(gs_Context->DevicePair.LogicalDevice,
                                  vertexShaderModule,
                                  gs_Context->Allocator);
            vkDestroyShaderModule(gs_Context->DevicePair.LogicalDevice,
                                  fragmentShaderModule,
                                  gs_Context->Allocator);
        }

        void DestroyPipelines()
        {
            vkDestroyPipeline(gs_Context->DevicePair.LogicalDevice,
                              g_Pipeline,
                              gs_Context->Allocator);
            vkDestroyPipelineLayout(gs_Context->DevicePair.LogicalDevice,
                                    g_PipelineLayout,
                                    gs_Context->Allocator);
        }

#if !OTR_RUNTIME
        void CreateVulkanDebugMessenger()
        {
            VkDebugUtilsMessengerCreateInfoEXT createInfo;
            PopulateDebugMessengerCreateInfo(createInfo);

            auto createDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
                gs_Context->Instance,
                "vkCreateDebugUtilsMessengerEXT");

            if (createDebugUtilsMessenger)
            {
                OTR_VULKAN_VALIDATE(createDebugUtilsMessenger(gs_Context->Instance,
                                                              &createInfo, gs_Context->Allocator,
                                                              &gs_Context->DebugMessenger))
            }
        }

        void DestroyVulkanDebugMessenger()
        {
            auto destroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
                gs_Context->Instance,
                "vkDestroyDebugUtilsMessengerEXT");

            if (destroyDebugUtilsMessenger != VK_NULL_HANDLE)
                destroyDebugUtilsMessenger(gs_Context->Instance,
                                           gs_Context->DebugMessenger,
                                           gs_Context->Allocator);
        }

        VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                     VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                     const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
                                                     [[maybe_unused]] void* userData)
        {
            // TODO: Also use the message type to filter out messages
            switch (messageSeverity)
            {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                    OTR_LOG_DEBUG(callbackData->pMessage)
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                    OTR_LOG_INFO(callbackData->pMessage)
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                    OTR_LOG_WARNING(callbackData->pMessage)
                    break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                    OTR_LOG_ERROR(callbackData->pMessage)
                    break;
                default:
                    OTR_LOG_FATAL("Unknown Error: {0}", callbackData->pMessage)
                    break;
            }

            return VK_FALSE;
        }

        void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
        {
            createInfo = { };
            createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            createInfo.messageSeverity =
//            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
//            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
            createInfo.messageType     =
                VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
                VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT;
            createInfo.pfnUserCallback = DebugCallback;
        }

        void GetRequiredInstanceValidationLayers(List<const char*>& layers)
        {
            layers.Add("VK_LAYER_KHRONOS_validation");

            UInt32 availableLayerCount = 0;
            OTR_VULKAN_VALIDATE(vkEnumerateInstanceLayerProperties(&availableLayerCount, VK_NULL_HANDLE))
            auto* availableLayers = Buffer::New<VkLayerProperties>(availableLayerCount);
            OTR_VULKAN_VALIDATE(vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers))

            for (UInt32 i = 0; i < layers.GetCount(); ++i)
            {
                bool found = false;

                for (UInt32 j = 0; j < availableLayerCount; ++j)
                {
                    if (strcmp(layers[i], availableLayers[j].layerName) == 0)
                    {
                        found = true;
                        OTR_LOG_TRACE("Required validation layer found: {0}", layers[i])
                        break;
                    }
                }

                OTR_INTERNAL_ASSERT_MSG(found, "Required layer is missing: {0}", layers[i])
            }

            Buffer::Delete(availableLayers, availableLayerCount);
        }
#endif
    }
}