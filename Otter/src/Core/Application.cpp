#include "Otter.PCH.h"

#include "Core/Application.h"
#include "Graphics/GraphicsSystem.h"

namespace Otter
{
    void Application::Run()
    {
        OTR_MEMORY_SYSTEM.Initialise(k_Configuration.MemoryRequirements);
        OTR_EVENT_SYSTEM.Initialise();

        auto* platform = Platform::CreatePlatform({ k_Configuration.Title,
                                                    k_Configuration.Width,
                                                    k_Configuration.Height,
                                                    WindowState::Default });
        if (!platform->TryInitialise())
        {
            OTR_LOG_FATAL("Failed to initialise platform. Shutting down...")

            Platform::DestroyPlatform(platform);

            OTR_EVENT_SYSTEM.Shutdown();
            OTR_MEMORY_SYSTEM.Shutdown();

            return;
        }

        if (OTR_GRAPHICS_SYSTEM.TryInitialise(platform->GetUnsafeContext()))
        {
            OTR_LOG_DEBUG("Total allocation after system initialisation: {0} / {1} bytes",
                          OTR_MEMORY_SYSTEM.GetUsedMemory(),
                          OTR_MEMORY_SYSTEM.GetMemorySize())

            while (platform->IsRunning())
            {
                platform->CaptureWindowEvents();
                OTR_EVENT_SYSTEM.Process();

                OTR_GRAPHICS_SYSTEM.RenderFrame();
            }

            OTR_GRAPHICS_SYSTEM.Shutdown();
        }
        else
        {
            OTR_LOG_FATAL("Failed to initialise graphics system")
        }

        platform->Shutdown();
        Platform::DestroyPlatform(platform);

        OTR_EVENT_SYSTEM.Shutdown();
        OTR_MEMORY_SYSTEM.Shutdown();

        OTR_LOG_DEBUG("Total allocation after system shutdown: {0} / {1} bytes",
                      OTR_MEMORY_SYSTEM.GetUsedMemory(),
                      OTR_MEMORY_SYSTEM.GetMemorySize())
    }
}