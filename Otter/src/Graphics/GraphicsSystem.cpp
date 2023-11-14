#include "Otter.PCH.h"

#include "Graphics/GraphicsSystem.h"

#include "Graphics/Common/Types.GraphicsAPI.h"
#include "Graphics/Vulkan/VulkanRenderer.h"

namespace Otter::GraphicsSystem
{
    static Graphics::AbstractRenderer* gs_Renderer = nullptr;

    GraphicsAPI g_GraphicsApi = GraphicsAPI::Vulkan; // TODO: Pass from configuration

    bool TryInitialise(const void* platformContext)
    {
        OTR_INTERNAL_ASSERT_MSG(gs_Renderer == nullptr, "Graphics system already initialised")

        switch (g_GraphicsApi)
        {
            case GraphicsAPI::Vulkan:
            {
                gs_Renderer = New<Graphics::Vulkan::Renderer>();
            }
                break;
            default:
            {
                OTR_LOG_FATAL("Unsupported graphics API: {0}", g_GraphicsApi)
                return false;
            }
        }

        gs_Renderer->Initialise(platformContext);
        OTR_LOG_DEBUG("Graphics system initialised ({0})", g_GraphicsApi)

        return true;
    }

    void Shutdown()
    {
        OTR_INTERNAL_ASSERT_MSG(gs_Renderer != nullptr, "Graphics system not initialised")

        OTR_LOG_DEBUG("Shutting down graphics system...")
        gs_Renderer->Shutdown();

        switch (g_GraphicsApi)
        {
            case GraphicsAPI::Vulkan:
            {
                Delete<Graphics::Vulkan::Renderer>((Graphics::Vulkan::Renderer*) gs_Renderer);
            }
                break;
            default:
            {
                OTR_LOG_FATAL("Unsupported graphics API: {0}. Possible memory leak on deletion.", g_GraphicsApi)
            }
        }

        gs_Renderer = nullptr;
    }

    void RenderFrame()
    {
        gs_Renderer->RenderFrame();
    }
}