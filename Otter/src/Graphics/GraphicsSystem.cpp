#include "Otter.PCH.h"

#include "Graphics/GraphicsSystem.h"

#include "Graphics/Common/Types.GraphicsAPI.h"
#include "Graphics/AbstractRenderer.h"
#include "Graphics/Vulkan/VulkanRenderer.h"

namespace Otter::GraphicsSystem
{
    Graphics::AbstractRenderer* g_Renderer;
    GraphicsAPI g_GraphicsApi = GraphicsAPI::Vulkan; // TODO: Pass from configuration

    bool TryInitialise(const void* platformContext)
    {
        switch (g_GraphicsApi)
        {
            case GraphicsAPI::Vulkan:
                g_Renderer = New<Graphics::Vulkan::VulkanRenderer>();
                break;
            default:
            {
                OTR_LOG_FATAL("Unsupported graphics API: {0}", g_GraphicsApi)
                return false;
            }
        }

        g_Renderer->Initialise(platformContext);

        OTR_LOG_DEBUG("Graphics system initialised ({0})", g_GraphicsApi)

        return true;
    }

    void Shutdown()
    {
        OTR_LOG_DEBUG("Shutting down graphics system...")

        g_Renderer->Shutdown();

        switch (g_GraphicsApi)
        {
            case GraphicsAPI::Vulkan:
                Delete<Graphics::Vulkan::VulkanRenderer>((Graphics::Vulkan::VulkanRenderer*) g_Renderer);
                break;
            default:
            {
                OTR_LOG_FATAL("Unsupported graphics API: {0}. Possible memory leak on deletion.", g_GraphicsApi)
                Delete(g_Renderer);
            }
        }
    }

    void RenderFrame()
    {
        g_Renderer->RenderFrame();
    }
}