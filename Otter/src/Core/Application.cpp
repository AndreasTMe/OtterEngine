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

            const Double64 fixedDeltaTime = 0.01;
            const Double64 frameRate      = 1.0 / 60.0;
            Double64       currentTime    = platform->GetAbsoluteTime();
            Double64       accumulator    = 0.0;

            while (platform->IsRunning())
            {
                Double64 newTime   = platform->GetAbsoluteTime();
                Double64 deltaTime = newTime - currentTime;
                currentTime = newTime;

                // Use accumulator to ensure fixed update is called at a fixed rate
                accumulator += deltaTime;

                // Clamp accumulator to prevent spiral of death
                if (accumulator < frameRate)
                    accumulator = frameRate;

                // Update
                platform->CaptureWindowEvents();
                OTR_EVENT_SYSTEM.Process();

                // Physics Update
                while (accumulator >= fixedDeltaTime)
                {
                    // TODO: Physics Update
                    accumulator -= fixedDeltaTime;
                }

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