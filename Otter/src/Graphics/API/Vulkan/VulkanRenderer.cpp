#include "Otter.PCH.h"

#include "Graphics/API/Vulkan/VulkanRenderer.h"
#include "Graphics/API/Vulkan/Types/VulkanTypes.Shader.h"
#include "Graphics/API/Vulkan/Types/VulkanTypes.Point.h"
#include "Graphics/API/Vulkan/VulkanExtensions.h"
#include "Graphics/API/Vulkan/VulkanSwapchains.h"
#include "Graphics/API/Vulkan/VulkanShaders.h"
#include "Graphics/API/Vulkan/VulkanPipelines.h"
#include "Graphics/API/Vulkan/VulkanBuffers.h"
#include "Graphics/API/Vulkan/VulkanQueues.h"
#include "Graphics/API/Vulkan/Types/VulkanTypes.UniformBuffers.h"
#include "Math/Matrix.h"

// TODO: Remove later
#include "Graphics/2D/Sprite.h"

namespace Otter::Graphics::Vulkan
{
#define OTR_VULKAN_SYNC_TIMEOUT 1000000000

    Sprite g_Sprite = {
        { 0.0f, 0.0f },
        { 1.0f, 1.0f },
        { 0.5f, 0.2f, 0.2f, 1.0f }
    };

    Span<Point2D, 4>      g_Vertices = { };
    const Span<UInt16, 6> gk_Indices = { 0, 1, 2, 2, 3, 0 };

    GlobalUniformBufferObject g_GlobalUbo = {
        Math::Perspective<Float32>(Math::DegToRad(45.0f), 1280.0f / 720.0f, 0.1f, 1000.0f),
        { 1.0f, 0.0f, 0.0f, 0.0f,
          0.0f, 1.0f, 0.0f, 0.0f,
          0.0f, 0.0f, 1.0f, 0.0f,
          0.0f, 0.0f, -5.0f, 1.0f },
    };

    Matrix<4, 4, Float32> g_Model = Matrix<4, 4, Float32>::Identity();

    enum class WindowState : UInt8
    {
        Normal    = 0,
        Minimized = 1,
        Maximized = 2
    };

    static WindowState gs_WindowState = WindowState::Normal;

    void Renderer::Initialise(const void* const platformContext)
    {
        CreateVulkanInstance(m_Allocator, &m_Instance);
#if !OTR_RUNTIME
        CreateVulkanDebugMessenger(m_Instance, m_Allocator, &m_DebugMessenger);
#endif
        CreateSurface(platformContext, m_Instance, m_Allocator, &m_Surface);
        CreateDevicePairs(m_Instance, m_Surface, m_Allocator, &m_DevicePair);
        CreateSwapchains();
        CreateRenderPass(m_DevicePair.LogicalDevice, m_Allocator, m_Swapchain, &m_RenderPass);
        CreateSwapchainFrameBuffers(m_DevicePair.LogicalDevice,
                                    m_Allocator,
                                    m_Swapchain.Extent,
                                    m_SwapchainImageViews,
                                    m_RenderPass,
                                    m_SwapchainFrameBuffers);
        CreateCommandPool(m_DevicePair, m_Allocator, &m_DevicePair.GraphicsCommandPool);
        CreateCommandBuffers(m_DevicePair.LogicalDevice,
                             m_DevicePair.GraphicsCommandPool,
                             m_Swapchain.MaxFramesInFlight,
                             m_DevicePair.CommandBuffers);
        CreateSyncObjects();

        CreatePipelines();
        CreateVertexBuffer();
        CreateIndexBuffer();
        CreateUniformBuffer();
        CreateDescriptorPool();
        CreateDescriptorSets();

        GlobalActions::OnWindowMinimized += [&](const Internal::WindowMinimizedEvent& event)
        {
            gs_WindowState = WindowState::Minimized;

            return true;
        };
        GlobalActions::OnWindowMaximized += [&](const Internal::WindowMaximizedEvent& event)
        {
            gs_WindowState = WindowState::Maximized;

            return true;
        };
        GlobalActions::OnWindowRestored += [&](const Internal::WindowRestoredEvent& event)
        {
            gs_WindowState = WindowState::Normal;

            return true;
        };
        GlobalActions::OnWindowResize += [&](const Internal::WindowResizeEvent& event)
        {
            if (event.GetWidth() == 0 || event.GetHeight() == 0)
                gs_WindowState = WindowState::Minimized;
            else
                gs_WindowState = WindowState::Normal;

            return true;
        };
    }

    void Renderer::Shutdown()
    {
        vkDeviceWaitIdle(m_DevicePair.LogicalDevice);

        DestroyCommandBuffers(m_DevicePair.CommandBuffers);
        DestroyCommandPool(m_DevicePair, m_Allocator, &m_DevicePair.GraphicsCommandPool);
        DestroySyncObjects();
        // TODO: DestroyUniformBuffers();
        DestroyVulkanDescriptors();
        DestroyBuffer(m_DevicePair.LogicalDevice, m_Allocator, &m_VertexBuffer);
        DestroyBuffer(m_DevicePair.LogicalDevice, m_Allocator, &m_IndexBuffer);
        DestroyPipeline(m_DevicePair.LogicalDevice, m_Allocator, &m_PipelineLayout, &m_Pipeline);
        DestroyRenderPass(m_DevicePair.LogicalDevice, m_Allocator, &m_RenderPass);
        DestroySwapchains();
        DestroyDevicePairs(m_Allocator, &m_DevicePair);
        DestroySurface(m_Instance, m_Allocator, &m_Surface);
#if !OTR_RUNTIME
        DestroyVulkanDebugMessenger(m_Instance, m_Allocator, &m_DebugMessenger);
#endif
        DestroyVulkanInstance(m_Allocator, &m_Instance);
    }

    bool Renderer::TryBeginFrame()
    {
        if (gs_WindowState == WindowState::Minimized)
            return false;

        const auto currentFrame = m_Swapchain.CurrentFrame;

        OTR_VULKAN_VALIDATE(vkWaitForFences(m_DevicePair.LogicalDevice,
                                            1,
                                            &m_DevicePair.RenderInFlightFences[currentFrame],
                                            VK_TRUE,
                                            OTR_VULKAN_SYNC_TIMEOUT))
        OTR_VULKAN_VALIDATE(vkResetFences(m_DevicePair.LogicalDevice,
                                          1,
                                          &m_DevicePair.RenderInFlightFences[currentFrame]))

        VkResult acquireNextImageResult = vkAcquireNextImageKHR(m_DevicePair.LogicalDevice,
                                                                m_Swapchain.Handle,
                                                                OTR_VULKAN_SYNC_TIMEOUT,
                                                                m_DevicePair.ImageAvailableSemaphores[currentFrame],
                                                                VK_NULL_HANDLE,
                                                                &m_Swapchain.CurrentImageIndex);
        if (acquireNextImageResult == VK_ERROR_OUT_OF_DATE_KHR)
        {
            RecreateSwapchains();
            return false;
        }
        else if (acquireNextImageResult != VK_SUCCESS && acquireNextImageResult != VK_SUBOPTIMAL_KHR)
        {
            OTR_INTERNAL_ASSERT_MSG(false, "Failed to acquire swapchain image")
        }

        const auto commandBuffer = m_DevicePair.CommandBuffers[currentFrame];
        OTR_VULKAN_VALIDATE(vkResetCommandBuffer(commandBuffer, 0))

        VkCommandBufferBeginInfo beginInfo{ };
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        OTR_VULKAN_VALIDATE(vkBeginCommandBuffer(commandBuffer, &beginInfo))

        VkViewport viewport{ };
        viewport.x        = 0.0f;
        viewport.y        = 0.0f;
        viewport.width    = static_cast<Float32>(m_Swapchain.Extent.width);
        viewport.height   = static_cast<Float32>(m_Swapchain.Extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor{ };
        scissor.offset = { 0, 0 };
        scissor.extent = m_Swapchain.Extent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        VkClearValue clearColor = {{{ 0.0f, 0.0f, 0.0f, 1.0f }}};

        VkRenderPassBeginInfo renderPassInfo{ };
        renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass        = m_RenderPass;
        renderPassInfo.framebuffer       = m_SwapchainFrameBuffers[m_Swapchain.CurrentImageIndex];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = m_Swapchain.Extent;
        renderPassInfo.clearValueCount   = 1;
        renderPassInfo.pClearValues      = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        return true;
    }

    void Renderer::DrawFrame()
    {
        const auto currentFrame = m_Swapchain.CurrentFrame;
        vkCmdBindPipeline(m_DevicePair.CommandBuffers[currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

        // TODO: Update uniform buffer (temp)
        void* data;
        vkMapMemory(m_DevicePair.LogicalDevice, m_UniformBuffer.DeviceMemory, 0, m_UniformBuffer.Size, 0, &data);
        memcpy(data, &g_GlobalUbo, (Size) m_UniformBuffer.Size);
        vkUnmapMemory(m_DevicePair.LogicalDevice, m_UniformBuffer.DeviceMemory);

        VkDescriptorBufferInfo bufferInfo{ };
        bufferInfo.buffer = m_UniformBuffer.Handle;
        bufferInfo.offset = 0;
        bufferInfo.range  = sizeof(GlobalUniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{ };
        descriptorWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet           = m_Descriptor.Set[currentFrame];
        descriptorWrite.dstBinding       = 0;
        descriptorWrite.dstArrayElement  = 0;
        descriptorWrite.descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount  = 1;
        descriptorWrite.pBufferInfo      = &bufferInfo;
        descriptorWrite.pImageInfo       = VK_NULL_HANDLE;
        descriptorWrite.pTexelBufferView = VK_NULL_HANDLE;

        vkUpdateDescriptorSets(m_DevicePair.LogicalDevice, 1, &descriptorWrite, 0, VK_NULL_HANDLE);
        // TODO: End temp code

        // TODO: Update push constants (temp)
        vkCmdPushConstants(m_DevicePair.CommandBuffers[currentFrame],
                           m_PipelineLayout,
                           VK_SHADER_STAGE_VERTEX_BIT,
                           0,
                           sizeof(Matrix<4, 4, Float32>) * 2,
                           &g_Model);
        // TODO: End temp code


        VkBuffer     vertexBuffers[] = { m_VertexBuffer.Handle };
        VkDeviceSize offsets[]       = { 0 };

        vkCmdBindVertexBuffers(m_DevicePair.CommandBuffers[currentFrame], 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(m_DevicePair.CommandBuffers[currentFrame],
                             m_IndexBuffer.Handle,
                             0,
                             VK_INDEX_TYPE_UINT16);
        vkCmdBindDescriptorSets(m_DevicePair.CommandBuffers[currentFrame],
                                VK_PIPELINE_BIND_POINT_GRAPHICS,
                                m_PipelineLayout,
                                0,
                                1,
                                &m_Descriptor.Set[currentFrame],
                                0,
                                nullptr);

        vkCmdDrawIndexed(m_DevicePair.CommandBuffers[currentFrame],
                         static_cast<UInt32>(gk_Indices.Length()),
                         1, 0, 0, 0);
    }

    void Renderer::EndFrame()
    {
        const auto currentFrame  = m_Swapchain.CurrentFrame;
        const auto commandBuffer = m_DevicePair.CommandBuffers[currentFrame];

        vkCmdEndRenderPass(commandBuffer);

        OTR_VULKAN_VALIDATE(vkEndCommandBuffer(commandBuffer))

        VkPipelineStageFlags waitStages[]       = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        VkSemaphore          waitSemaphores[]   = { m_DevicePair.ImageAvailableSemaphores[currentFrame] };
        VkSemaphore          signalSemaphores[] = { m_DevicePair.RenderFinishedSemaphores[currentFrame] };

        VkSubmitInfo submitInfo{ };
        submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount   = 1;
        submitInfo.pCommandBuffers      = &commandBuffer;
        submitInfo.pWaitDstStageMask    = waitStages;
        submitInfo.waitSemaphoreCount   = 1;
        submitInfo.pWaitSemaphores      = waitSemaphores;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = signalSemaphores;

        OTR_VULKAN_VALIDATE(vkQueueSubmit(m_DevicePair.GraphicsQueueFamily.Handle,
                                          1,
                                          &submitInfo,
                                          m_DevicePair.RenderInFlightFences[currentFrame]))

        VkSwapchainKHR swapChains[] = { m_Swapchain.Handle };

        VkPresentInfoKHR presentInfo{ };
        presentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores    = signalSemaphores;
        presentInfo.swapchainCount     = 1;
        presentInfo.pSwapchains        = swapChains;
        presentInfo.pImageIndices      = &m_Swapchain.CurrentImageIndex;

        VkResult queuePresentResult = vkQueuePresentKHR(m_DevicePair.PresentationQueueFamily.Handle, &presentInfo);
        if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR)
        {
            RecreateSwapchains();
        }
        else if (queuePresentResult != VK_SUCCESS)
        {
            OTR_INTERNAL_ASSERT_MSG(false, "Failed to present swapchain image")
        }

        m_Swapchain.CurrentFrame = (currentFrame + 1) % m_Swapchain.MaxFramesInFlight;
    }

    void Renderer::CreateVulkanInstance(const VkAllocationCallbacks* const allocator, VkInstance* outInstance)
    {
        OTR_LOG_TRACE("Creating Vulkan instance...")

        VkApplicationInfo appInfo{ };
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName   = "Otter Engine";
        appInfo.applicationVersion = VK_MAKE_API_VERSION(1, 0, 0, 0);
        appInfo.pEngineName        = "Otter Engine";
        appInfo.engineVersion      = VK_MAKE_API_VERSION(1, 0, 0, 0);
        appInfo.apiVersion         = VK_API_VERSION_1_3;

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

        OTR_VULKAN_VALIDATE(vkCreateInstance(&createInfo, allocator, outInstance))
    }

    void Renderer::DestroyVulkanInstance(const VkAllocationCallbacks* const allocator, VkInstance* outInstance)
    {
        OTR_INTERNAL_ASSERT_MSG(*outInstance != VK_NULL_HANDLE,
                                "Instance must be initialized before being destroyed")

        vkDestroyInstance(*outInstance, allocator);
        *outInstance = VK_NULL_HANDLE;
    }

    void Renderer::CreateSurface(const void* const platformContext,
                                 const VkInstance& instance,
                                 const VkAllocationCallbacks* const allocator,
                                 VkSurfaceKHR* outSurface)
    {
#if OTR_PLATFORM_WINDOWS
        const auto* const platformContextData       = ((PlatformContext*) platformContext)->Data;
        const auto* const windowsPlatformWindowData =
                      (Otter::Internal::WindowsPlatformWindowData*) platformContextData;

        VkWin32SurfaceCreateInfoKHR createInfo{ };
        createInfo.sType     = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd      = windowsPlatformWindowData->WindowHandle;
        createInfo.hinstance = windowsPlatformWindowData->InstanceHandle;

        OTR_VULKAN_VALIDATE(vkCreateWin32SurfaceKHR(instance, &createInfo, allocator, outSurface))
#else
        OTR_LOG_FATAL("'CreateSurface' not supported for this platform")
#endif
    }

    void Renderer::DestroySurface(const VkInstance& instance,
                                  const VkAllocationCallbacks* const allocator,
                                  VkSurfaceKHR* outSurface)
    {
        OTR_INTERNAL_ASSERT_MSG(instance != VK_NULL_HANDLE,
                                "Instance must be initialized before destroying surface")
        OTR_INTERNAL_ASSERT_MSG(*outSurface != VK_NULL_HANDLE,
                                "Surface must be initialized before being destroyed")

        vkDestroySurfaceKHR(instance, *outSurface, allocator);
        *outSurface = VK_NULL_HANDLE;
    }

    void Renderer::SelectPhysicalDevice(const VkInstance& instance,
                                        const VkSurfaceKHR& surface,
                                        VulkanDevicePair* outDevicePair)
    {
        UInt32 deviceCount = 0;
        OTR_VULKAN_VALIDATE(vkEnumeratePhysicalDevices(instance, &deviceCount, VK_NULL_HANDLE))

        OTR_INTERNAL_ASSERT_MSG(deviceCount > 0, "Failed to find GPUs with Vulkan support")

        VkPhysicalDevice queriedDevices[deviceCount];
        OTR_VULKAN_VALIDATE(vkEnumeratePhysicalDevices(instance, &deviceCount, queriedDevices))

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
            QueryQueueFamilies(surface, queriedDevice, &graphicsFamily, &presentFamily);

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
            QuerySwapchainSupport(surface, queriedDevice, &swapchainSupportInfo);

            bool isSwapchainSupported = swapchainSupportInfo.SurfaceFormats.GetCount() > 0
                                        && swapchainSupportInfo.PresentModes.GetCount() > 0;

            if (!isSwapchainSupported)
            {
                OTR_LOG_TRACE("{0} is not suitable due to missing swapchain support. "
                              "Moving to the next one...", deviceProperties.deviceName)
                continue;
            }

            outDevicePair->PhysicalDevice                = queriedDevice;
            outDevicePair->GraphicsQueueFamily.Index     = graphicsFamily;
            outDevicePair->PresentationQueueFamily.Index = presentFamily;

            OTR_LOG_TRACE("Found a suitable GPU: {0}", deviceProperties.deviceName)

            break;
        }

        OTR_INTERNAL_ASSERT_MSG(outDevicePair->PhysicalDevice != VK_NULL_HANDLE, "Failed to find a suitable GPU")
    }

    void Renderer::CreateLogicalDevice(const VkAllocationCallbacks* const allocator, VulkanDevicePair* outDevicePair)
    {
        List <VkDeviceQueueCreateInfo> queueCreateInfos;

        HashSet <UInt32> uniqueQueueFamilies(2);
        uniqueQueueFamilies.TryAdd(outDevicePair->GraphicsQueueFamily.Index);
        uniqueQueueFamilies.TryAdd(outDevicePair->PresentationQueueFamily.Index);

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
#if !OTR_RUNTIME
        List<const char*> layers;
        GetRequiredInstanceValidationLayers(layers);

        createInfo.enabledLayerCount   = layers.GetCount();
        createInfo.ppEnabledLayerNames = layers.GetData();
#else
        createInfo.enabledLayerCount   = 0;
        createInfo.ppEnabledLayerNames = VK_NULL_HANDLE;
#endif

        OTR_VULKAN_VALIDATE(vkCreateDevice(outDevicePair->PhysicalDevice,
                                           &createInfo,
                                           allocator,
                                           &outDevicePair->LogicalDevice))

        OTR_INTERNAL_ASSERT_MSG(outDevicePair->LogicalDevice != VK_NULL_HANDLE,
                                "Failed to create logical device")

        vkGetDeviceQueue(outDevicePair->LogicalDevice,
                         outDevicePair->GraphicsQueueFamily.Index,
                         0,
                         &outDevicePair->GraphicsQueueFamily.Handle);
        vkGetDeviceQueue(outDevicePair->LogicalDevice,
                         outDevicePair->PresentationQueueFamily.Index,
                         0,
                         &outDevicePair->PresentationQueueFamily.Handle);

        OTR_INTERNAL_ASSERT_MSG(outDevicePair->GraphicsQueueFamily.Handle != VK_NULL_HANDLE,
                                "Failed to get graphics queue")
        OTR_INTERNAL_ASSERT_MSG(outDevicePair->PresentationQueueFamily.Handle != VK_NULL_HANDLE,
                                "Failed to get presentation queue")
    }

    void Renderer::CreateDevicePairs(const VkInstance& instance,
                                     const VkSurfaceKHR& surface,
                                     const VkAllocationCallbacks* const allocator,
                                     VulkanDevicePair* outDevicePair)
    {
        OTR_INTERNAL_ASSERT_MSG(instance != VK_NULL_HANDLE,
                                "Instance must be initialized before creating device pairs")
        OTR_INTERNAL_ASSERT_MSG(surface != VK_NULL_HANDLE,
                                "Surface must be initialized before creating device pairs")

        OTR_LOG_TRACE("Creating device pairs...")

        SelectPhysicalDevice(instance, surface, outDevicePair);
        CreateLogicalDevice(allocator, outDevicePair);
    }

    void Renderer::DestroyDevicePairs(const VkAllocationCallbacks* const allocator, VulkanDevicePair* outDevicePair)
    {
        OTR_INTERNAL_ASSERT_MSG(outDevicePair->LogicalDevice != VK_NULL_HANDLE,
                                "Logical device must be initialized before destroying device pairs")

        vkDestroyDevice(outDevicePair->LogicalDevice, allocator);

        outDevicePair->PhysicalDevice                 = VK_NULL_HANDLE;
        outDevicePair->LogicalDevice                  = VK_NULL_HANDLE;
        outDevicePair->GraphicsQueueFamily.Handle     = VK_NULL_HANDLE;
        outDevicePair->PresentationQueueFamily.Handle = VK_NULL_HANDLE;
    }

    void Renderer::CreateSwapchains()
    {
        OTR_LOG_TRACE("Creating Vulkan swapchains...")

        CreateSingleSwapchain(m_Surface, m_DevicePair, m_Allocator, &m_Swapchain);
        CreateSwapchainImages(m_DevicePair.LogicalDevice, m_Swapchain.Handle, m_SwapchainImages);
        CreateSwapchainImageViews(m_DevicePair.LogicalDevice,
                                  m_Allocator,
                                  m_SwapchainImages,
                                  m_Swapchain.SurfaceFormat.format,
                                  m_SwapchainImageViews);
    }

    void Renderer::RecreateSwapchains()
    {
        OTR_LOG_TRACE("Recreating Vulkan swapchains...")

        vkDeviceWaitIdle(m_DevicePair.LogicalDevice);

        DestroySwapchains();

        CreateSingleSwapchain(m_Surface, m_DevicePair, m_Allocator, &m_Swapchain);
        CreateSwapchainImages(m_DevicePair.LogicalDevice, m_Swapchain.Handle, m_SwapchainImages);
        CreateSwapchainImageViews(m_DevicePair.LogicalDevice,
                                  m_Allocator,
                                  m_SwapchainImages,
                                  m_Swapchain.SurfaceFormat.format,
                                  m_SwapchainImageViews);
        CreateSwapchainFrameBuffers(m_DevicePair.LogicalDevice,
                                    m_Allocator,
                                    m_Swapchain.Extent,
                                    m_SwapchainImageViews,
                                    m_RenderPass,
                                    m_SwapchainFrameBuffers);
    }

    void Renderer::DestroySwapchains()
    {
        OTR_INTERNAL_ASSERT_MSG(m_DevicePair.LogicalDevice != VK_NULL_HANDLE,
                                "Logical device must be initialized before destroying its swapchain")

        for (auto& swapchainFrameBuffer: m_SwapchainFrameBuffers)
            vkDestroyFramebuffer(m_DevicePair.LogicalDevice, swapchainFrameBuffer, m_Allocator);

        m_SwapchainFrameBuffers.ClearDestructive();

        for (auto& swapchainImageView: m_SwapchainImageViews)
            vkDestroyImageView(m_DevicePair.LogicalDevice, swapchainImageView, m_Allocator);

        m_SwapchainImageViews.ClearDestructive();
        m_SwapchainImages.ClearDestructive();

        vkDestroySwapchainKHR(m_DevicePair.LogicalDevice, m_Swapchain.Handle, m_Allocator);
    }

    void Renderer::CreateRenderPass(const VkDevice& logicalDevice,
                                    const VkAllocationCallbacks* const allocator,
                                    const VulkanSwapchain& swapchain,
                                    VkRenderPass* outRenderPass)
    {
        OTR_INTERNAL_ASSERT_MSG(logicalDevice != VK_NULL_HANDLE,
                                "Logical device must be initialized before creating render pass")
        OTR_INTERNAL_ASSERT_MSG(swapchain.Handle != VK_NULL_HANDLE,
                                "Swapchain must be initialized before creating render pass")

        VkAttachmentDescription colorAttachment{ };
        colorAttachment.format         = swapchain.SurfaceFormat.format;
        colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        colorAttachment.flags          = 0;

        VkAttachmentReference colorAttachmentRef{ };
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subPass{ };
        subPass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subPass.inputAttachmentCount    = 0;
        subPass.pInputAttachments       = VK_NULL_HANDLE;
        subPass.colorAttachmentCount    = 1;
        subPass.pColorAttachments       = &colorAttachmentRef;
        subPass.pResolveAttachments     = VK_NULL_HANDLE;
        subPass.pDepthStencilAttachment = VK_NULL_HANDLE;
        subPass.preserveAttachmentCount = 0;
        subPass.pPreserveAttachments    = VK_NULL_HANDLE;
        subPass.flags                   = 0;

        VkSubpassDependency dependency{ };
        dependency.srcSubpass      = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass      = 0;
        dependency.srcStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask    = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask   = 0;
        dependency.dstAccessMask   = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependency.dependencyFlags = 0;

        VkRenderPassCreateInfo renderPassInfo{ };
        renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments    = &colorAttachment;
        renderPassInfo.subpassCount    = 1;
        renderPassInfo.pSubpasses      = &subPass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies   = &dependency;
        renderPassInfo.pNext           = VK_NULL_HANDLE;

        OTR_VULKAN_VALIDATE(vkCreateRenderPass(logicalDevice, &renderPassInfo, allocator, outRenderPass))
    }

    void Renderer::DestroyRenderPass(const VkDevice& logicalDevice,
                                     const VkAllocationCallbacks* const allocator,
                                     VkRenderPass* outRenderPass)
    {
        OTR_INTERNAL_ASSERT_MSG(logicalDevice != VK_NULL_HANDLE,
                                "Logical device must be initialized before destroying render pass")
        OTR_INTERNAL_ASSERT_MSG(*outRenderPass != VK_NULL_HANDLE,
                                "Render pass must be initialized before being destroyed")

        vkDestroyRenderPass(logicalDevice, *outRenderPass, allocator);
        *outRenderPass = VK_NULL_HANDLE;
    }

    void Renderer::CreateCommandPool(const VulkanDevicePair& devicePair,
                                     const VkAllocationCallbacks* const allocator,
                                     VkCommandPool* outCommandPool)
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo{ };
        commandPoolCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = devicePair.GraphicsQueueFamily.Index;
        commandPoolCreateInfo.pNext            = VK_NULL_HANDLE;

        OTR_VULKAN_VALIDATE(vkCreateCommandPool(devicePair.LogicalDevice,
                                                &commandPoolCreateInfo,
                                                allocator,
                                                outCommandPool))
    }

    void Renderer::DestroyCommandPool(const VulkanDevicePair& devicePair,
                                      const VkAllocationCallbacks* const allocator,
                                      VkCommandPool* outCommandPool)
    {
        OTR_INTERNAL_ASSERT_MSG(devicePair.LogicalDevice != VK_NULL_HANDLE, "Vulkan logical device is null!")
        OTR_INTERNAL_ASSERT_MSG(*outCommandPool != VK_NULL_HANDLE, "Vulkan command pool is null!")

        vkDestroyCommandPool(devicePair.LogicalDevice, *outCommandPool, allocator);
    }

    void Renderer::CreateCommandBuffers(const VkDevice& logicalDevice,
                                        const VkCommandPool& commandPool,
                                        const UInt32& commandBufferCount,
                                        List <VkCommandBuffer>& outCommandBuffers)
    {
        VkCommandBufferAllocateInfo commandBufferAllocateInfo{ };
        commandBufferAllocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.commandPool        = commandPool;
        commandBufferAllocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = commandBufferCount;
        commandBufferAllocateInfo.pNext              = VK_NULL_HANDLE;

        VkCommandBuffer tempCommandBuffers[commandBufferCount];
        OTR_VULKAN_VALIDATE(vkAllocateCommandBuffers(logicalDevice, &commandBufferAllocateInfo, tempCommandBuffers))

        Collections::New<VkCommandBuffer>(tempCommandBuffers, commandBufferCount, outCommandBuffers);
    }

    void Renderer::DestroyCommandBuffers(List <VkCommandBuffer>& outCommandBuffers)
    {
        outCommandBuffers.ClearDestructive();
    }

    void Renderer::CreateSyncObjects()
    {
        m_DevicePair.ImageAvailableSemaphores.Reserve(m_Swapchain.MaxFramesInFlight);
        m_DevicePair.RenderFinishedSemaphores.Reserve(m_Swapchain.MaxFramesInFlight);
        m_DevicePair.RenderInFlightFences.Reserve(m_Swapchain.MaxFramesInFlight);

        VkSemaphoreCreateInfo semaphoreInfo{ };
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreInfo.pNext = VK_NULL_HANDLE;

        VkFenceCreateInfo fenceInfo{ };
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
        fenceInfo.pNext = VK_NULL_HANDLE;

        VkSemaphore tempImageAvailableSemaphores[m_Swapchain.MaxFramesInFlight];
        VkSemaphore tempRenderFinishedSemaphores[m_Swapchain.MaxFramesInFlight];
        VkFence     tempInFlightFences[m_Swapchain.MaxFramesInFlight];

        for (size_t i = 0; i < m_Swapchain.MaxFramesInFlight; i++)
        {
            OTR_VULKAN_VALIDATE(vkCreateSemaphore(m_DevicePair.LogicalDevice,
                                                  &semaphoreInfo,
                                                  m_Allocator,
                                                  &tempImageAvailableSemaphores[i]))
            OTR_VULKAN_VALIDATE(vkCreateSemaphore(m_DevicePair.LogicalDevice,
                                                  &semaphoreInfo,
                                                  m_Allocator,
                                                  &tempRenderFinishedSemaphores[i]))
            OTR_VULKAN_VALIDATE(vkCreateFence(m_DevicePair.LogicalDevice,
                                              &fenceInfo,
                                              m_Allocator,
                                              &tempInFlightFences[i]))
        }

        Collections::New<VkSemaphore>(tempImageAvailableSemaphores,
                                      m_Swapchain.MaxFramesInFlight,
                                      m_DevicePair.ImageAvailableSemaphores);
        Collections::New<VkSemaphore>(tempRenderFinishedSemaphores,
                                      m_Swapchain.MaxFramesInFlight,
                                      m_DevicePair.RenderFinishedSemaphores);
        Collections::New<VkFence>(tempInFlightFences,
                                  m_Swapchain.MaxFramesInFlight,
                                  m_DevicePair.RenderInFlightFences);
    }

    void Renderer::DestroySyncObjects()
    {
        for (size_t i = 0; i < m_Swapchain.MaxFramesInFlight; i++)
        {
            vkDestroySemaphore(m_DevicePair.LogicalDevice, m_DevicePair.ImageAvailableSemaphores[i], m_Allocator);
            vkDestroySemaphore(m_DevicePair.LogicalDevice, m_DevicePair.RenderFinishedSemaphores[i], m_Allocator);
            vkDestroyFence(m_DevicePair.LogicalDevice, m_DevicePair.RenderInFlightFences[i], m_Allocator);
        }

        m_DevicePair.ImageAvailableSemaphores.ClearDestructive();
        m_DevicePair.RenderFinishedSemaphores.ClearDestructive();
        m_DevicePair.RenderInFlightFences.ClearDestructive();
    }

    void Renderer::CreateVertexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(g_Vertices[0]) * g_Vertices.Length();

        VulkanBuffer stagingBuffer;
        if (!TryCreateBuffer(m_DevicePair,
                             m_Allocator,
                             bufferSize,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             &stagingBuffer))
        {
            OTR_LOG_FATAL("Failed to create staging buffer")
            return;
        }

        BindBuffer(m_DevicePair.LogicalDevice, stagingBuffer, 0);

        const auto  spriteVertices = g_Sprite.GetVertices();
        for (UInt64 i              = 0; i < spriteVertices.Length(); i++)
        {
            g_Vertices[i].Position = { spriteVertices[i][0], spriteVertices[i][1], 0.0f };
            g_Vertices[i].Color    = g_Sprite.GetColor();
        }

        void* data;
        vkMapMemory(m_DevicePair.LogicalDevice, stagingBuffer.DeviceMemory, 0, bufferSize, 0, &data);
        memcpy(data, (void*) g_Vertices.GetData(), (Size) bufferSize);
        vkUnmapMemory(m_DevicePair.LogicalDevice, stagingBuffer.DeviceMemory);

        if (!TryCreateBuffer(m_DevicePair,
                             m_Allocator,
                             bufferSize,
                             VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                             VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             &m_VertexBuffer))
        {
            OTR_LOG_FATAL("Failed to create vertex buffer")
            DestroyBuffer(m_DevicePair.LogicalDevice, m_Allocator, &stagingBuffer);
            return;
        }

        BindBuffer(m_DevicePair.LogicalDevice, m_VertexBuffer, 0);
        CopyBuffer(m_DevicePair.LogicalDevice,
                   bufferSize,
                   m_DevicePair.GraphicsQueueFamily.Handle,
                   m_DevicePair.GraphicsCommandPool,
                   stagingBuffer.Handle,
                   &m_VertexBuffer.Handle);

        DestroyBuffer(m_DevicePair.LogicalDevice, m_Allocator, &stagingBuffer);
    }

    void Renderer::CreateIndexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(gk_Indices[0]) * gk_Indices.Length();

        VulkanBuffer stagingBuffer;
        if (!TryCreateBuffer(m_DevicePair,
                             m_Allocator,
                             bufferSize,
                             VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             &stagingBuffer))
        {
            OTR_LOG_FATAL("Failed to create staging buffer")
            return;
        }

        BindBuffer(m_DevicePair.LogicalDevice, stagingBuffer, 0);

        void* data;
        vkMapMemory(m_DevicePair.LogicalDevice, stagingBuffer.DeviceMemory, 0, bufferSize, 0, &data);
        memcpy(data, (void*) gk_Indices.GetData(), (Size) bufferSize);
        vkUnmapMemory(m_DevicePair.LogicalDevice, stagingBuffer.DeviceMemory);

        if (!TryCreateBuffer(m_DevicePair,
                             m_Allocator,
                             bufferSize,
                             VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                             VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                             &m_IndexBuffer))
        {
            OTR_LOG_FATAL("Failed to create index buffer")
            DestroyBuffer(m_DevicePair.LogicalDevice, m_Allocator, &stagingBuffer);
            return;
        }

        BindBuffer(m_DevicePair.LogicalDevice, m_IndexBuffer, 0);
        CopyBuffer(m_DevicePair.LogicalDevice,
                   bufferSize,
                   m_DevicePair.GraphicsQueueFamily.Handle,
                   m_DevicePair.GraphicsCommandPool,
                   stagingBuffer.Handle,
                   &m_IndexBuffer.Handle);

        DestroyBuffer(m_DevicePair.LogicalDevice, m_Allocator, &stagingBuffer);
    }

    void Renderer::CreateUniformBuffer()
    {
        VkDeviceSize bufferSize = sizeof(GlobalUniformBufferObject);

        if (!TryCreateBuffer(m_DevicePair,
                             m_Allocator,
                             bufferSize,
                             VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                             &m_UniformBuffer))
        {
            OTR_LOG_FATAL("Failed to create global uniform buffer")
            return;
        }

        BindBuffer(m_DevicePair.LogicalDevice, m_UniformBuffer, 0);
    }

    void Renderer::CreateDescriptorSetLayout()
    {
        VkDescriptorSetLayoutBinding uboLayoutBinding{ };
        uboLayoutBinding.binding            = 0;
        uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        uboLayoutBinding.descriptorCount    = 1;
        uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_VERTEX_BIT;
        uboLayoutBinding.pImmutableSamplers = VK_NULL_HANDLE;

        VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{ };
        descriptorSetLayoutCreateInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorSetLayoutCreateInfo.bindingCount = 1;
        descriptorSetLayoutCreateInfo.pBindings    = &uboLayoutBinding;
        descriptorSetLayoutCreateInfo.pNext        = VK_NULL_HANDLE;

        OTR_VULKAN_VALIDATE(vkCreateDescriptorSetLayout(m_DevicePair.LogicalDevice,
                                                        &descriptorSetLayoutCreateInfo,
                                                        m_Allocator,
                                                        &m_Descriptor.SetLayout))
    }

    void Renderer::CreateDescriptorPool()
    {
        VkDescriptorPoolSize globalDescriptorPoolSize{ };
        globalDescriptorPoolSize.type            = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        globalDescriptorPoolSize.descriptorCount = m_Swapchain.MaxFramesInFlight;

        VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{ };
        descriptorPoolCreateInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptorPoolCreateInfo.poolSizeCount = 1;
        descriptorPoolCreateInfo.pPoolSizes    = &globalDescriptorPoolSize;
        descriptorPoolCreateInfo.maxSets       = m_Swapchain.MaxFramesInFlight;
        descriptorPoolCreateInfo.pNext         = VK_NULL_HANDLE;

        OTR_VULKAN_VALIDATE(vkCreateDescriptorPool(m_DevicePair.LogicalDevice,
                                                   &descriptorPoolCreateInfo,
                                                   m_Allocator,
                                                   &m_Descriptor.Pool))
    }

    void Renderer::CreateDescriptorSets()
    {
        Enumerable <VkDescriptorSetLayout> layouts = Enumerable<VkDescriptorSetLayout>::Of({ m_Descriptor.SetLayout,
                                                                                             m_Descriptor.SetLayout,
                                                                                             m_Descriptor.SetLayout });
        VkDescriptorSetAllocateInfo        descriptorSetAllocateInfo{ };
        descriptorSetAllocateInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocateInfo.descriptorPool     = m_Descriptor.Pool;
        descriptorSetAllocateInfo.descriptorSetCount = m_Swapchain.MaxFramesInFlight;
        descriptorSetAllocateInfo.pSetLayouts        = layouts.GetData();
        descriptorSetAllocateInfo.pNext              = VK_NULL_HANDLE;

        // TODO: "const_cast" usage reminder
        OTR_VULKAN_VALIDATE(vkAllocateDescriptorSets(m_DevicePair.LogicalDevice,
                                                     &descriptorSetAllocateInfo,
                                                     const_cast<VkDescriptorSet*>(m_Descriptor.Set.GetData())))
    }

    void Renderer::DestroyVulkanDescriptors()
    {
        DestroyBuffer(m_DevicePair.LogicalDevice, m_Allocator, &m_UniformBuffer);

        vkDestroyDescriptorPool(m_DevicePair.LogicalDevice, m_Descriptor.Pool, m_Allocator);
        vkDestroyDescriptorSetLayout(m_DevicePair.LogicalDevice, m_Descriptor.SetLayout, m_Allocator);

        m_Descriptor.Pool      = VK_NULL_HANDLE;
        m_Descriptor.SetLayout = VK_NULL_HANDLE;
    }

    void Renderer::CreatePipelines()
    {
        // HELP: Shader related
        VulkanShader shader{ };
        if (!TryCreateShaderModule(m_DevicePair.LogicalDevice,
                                   m_Allocator,
                                   "Assets/Shaders/default.vert.spv", // TODO: Remove hardcoded path
                                   &shader.VertexModule))
        {
            OTR_LOG_FATAL("Error when building the vertex shader module")
            return;
        }

        if (!TryCreateShaderModule(m_DevicePair.LogicalDevice,
                                   m_Allocator,
                                   "Assets/Shaders/default.frag.spv", // TODO: Remove hardcoded path
                                   &shader.FragmentModule))
        {
            DestroyShaderModule(m_DevicePair.LogicalDevice, m_Allocator, &shader.VertexModule);

            OTR_LOG_FATAL("Error when building the fragment shader module")
            return;
        }

        VkPipelineShaderStageCreateInfo vertShaderStageCreateInfo{ };
        vertShaderStageCreateInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageCreateInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageCreateInfo.module = shader.VertexModule;
        vertShaderStageCreateInfo.pName  = "main";
        vertShaderStageCreateInfo.pNext  = VK_NULL_HANDLE;

        VkPipelineShaderStageCreateInfo fragShaderStageCreateInfo{ };
        fragShaderStageCreateInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageCreateInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageCreateInfo.module = shader.FragmentModule;
        fragShaderStageCreateInfo.pName  = "main";
        fragShaderStageCreateInfo.pNext  = VK_NULL_HANDLE;

        CreateDescriptorSetLayout();

        VkPushConstantRange pushConstantRange{ };
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        pushConstantRange.offset     = 0;
        pushConstantRange.size       = sizeof(Matrix<4, 4, Float32>) * 2;

        CreatePipeline(m_DevicePair.LogicalDevice,
                       m_RenderPass,
                       m_Allocator,
                       Enumerable<VkPipelineShaderStageCreateInfo>::Of({ vertShaderStageCreateInfo,
                                                                         fragShaderStageCreateInfo }),
                       Enumerable<VkDescriptorSetLayout>::Of({ m_Descriptor.SetLayout }),
                       Enumerable<VkPushConstantRange>::Of({ pushConstantRange }),
                       m_Swapchain.Extent,
                       &m_PipelineLayout,
                       &m_Pipeline);

        DestroyShaderModule(m_DevicePair.LogicalDevice, m_Allocator, &shader.VertexModule);
        DestroyShaderModule(m_DevicePair.LogicalDevice, m_Allocator, &shader.FragmentModule);
    }

#if !OTR_RUNTIME
    void Renderer::CreateVulkanDebugMessenger(const VkInstance& instance,
                                              const VkAllocationCallbacks* const allocator,
                                              VkDebugUtilsMessengerEXT* outDebugMessenger)
    {
        OTR_INTERNAL_ASSERT_MSG(instance != VK_NULL_HANDLE,
                                "Instance must be initialized before creating debug messenger")

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        PopulateDebugMessengerCreateInfo(createInfo);

        auto createDebugUtilsMessenger = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            instance,
            "vkCreateDebugUtilsMessengerEXT");

        if (createDebugUtilsMessenger)
        {
            OTR_VULKAN_VALIDATE(createDebugUtilsMessenger(instance, &createInfo, allocator, outDebugMessenger))
        }
    }

    void Renderer::DestroyVulkanDebugMessenger(const VkInstance& instance,
                                               const VkAllocationCallbacks* const allocator,
                                               VkDebugUtilsMessengerEXT* outDebugMessenger)
    {
        OTR_INTERNAL_ASSERT_MSG(instance != VK_NULL_HANDLE,
                                "Instance must be initialized before destroying debug messenger")
        OTR_INTERNAL_ASSERT_MSG(*outDebugMessenger != VK_NULL_HANDLE,
                                "Debug messenger must be initialized before being destroyed")

        auto destroyDebugUtilsMessenger = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            instance,
            "vkDestroyDebugUtilsMessengerEXT");

        if (destroyDebugUtilsMessenger != VK_NULL_HANDLE)
            destroyDebugUtilsMessenger(instance, *outDebugMessenger, allocator);

        *outDebugMessenger = VK_NULL_HANDLE;
    }

    VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
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

    void Renderer::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
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

    void Renderer::GetRequiredInstanceValidationLayers(List<const char*>& layers)
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
                    break;
                }
            }

            OTR_INTERNAL_ASSERT_MSG(found, "Required layer is missing: {0}", layers[i])
        }

        Buffer::Delete(availableLayers, availableLayerCount);
    }
#endif

#undef OTR_VULKAN_SYNC_TIMEOUT
}