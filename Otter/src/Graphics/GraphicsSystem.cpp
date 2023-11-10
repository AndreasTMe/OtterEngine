#include "Otter.PCH.h"

#include "Graphics/GraphicsSystem.h"

#include "Graphics/Common/Types.GraphicsAPI.h"
#include "Graphics/Vulkan/VulkanRenderer.h"

namespace Otter::GraphicsSystem
{
    GraphicsAPI g_GraphicsApi = GraphicsAPI::Vulkan; // TODO: Pass from configuration

    bool TryInitialise(const void* platformContext)
    {
        switch (g_GraphicsApi)
        {
            case GraphicsAPI::Vulkan:
            {
                Graphics::Vulkan::Initialise(platformContext);
            }
                break;
            default:
            {
                OTR_LOG_FATAL("Unsupported graphics API: {0}", g_GraphicsApi)
                return false;
            }
        }

        OTR_LOG_DEBUG("Graphics system initialised ({0})", g_GraphicsApi)

        return true;
    }

    void Shutdown()
    {
        OTR_LOG_DEBUG("Shutting down graphics system...")

        switch (g_GraphicsApi)
        {
            case GraphicsAPI::Vulkan:
            {
                Graphics::Vulkan::Shutdown();
            }
                break;
            default:
            {
                OTR_LOG_FATAL("Unsupported graphics API: {0}. Possible memory leak on deletion.", g_GraphicsApi)
            }
        }
    }

    void RenderFrame()
    {
        // TODO: Use function pointers for this, switch is not ideal
    }
}