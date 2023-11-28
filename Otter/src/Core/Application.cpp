#include "Otter.PCH.h"

#include "Core/Application.h"
#include "Graphics/GraphicsSystem.h"

namespace Otter
{
    void Application::Run()
    {
        MemorySystem::Initialise(k_Configuration.MemoryRequirements);
        EventSystem::Initialise();

        auto* platform = Platform::CreatePlatform({ k_Configuration.Title,
                                                    k_Configuration.Width,
                                                    k_Configuration.Height,
                                                    WindowState::Default });
        if (!platform->TryInitialise())
        {
            OTR_LOG_FATAL("Failed to initialise platform. Shutting down...")

            Platform::DestroyPlatform(platform);

            EventSystem::Shutdown();
            MemorySystem::Shutdown();

            return;
        }

        if (GraphicsSystem::TryInitialise(platform->GetUnsafeContext()))
        {
            OTR_LOG_DEBUG("Total allocation after system initialisation: {0} / {1} bytes",
                          MemorySystem::GetUsedMemory(),
                          MemorySystem::GetMemorySize())

            while (platform->IsRunning())
            {
                platform->CaptureWindowEvents();
                EventSystem::Process();

                GraphicsSystem::RenderFrame();
            }

            GraphicsSystem::Shutdown();
        }
        else
        {
            OTR_LOG_FATAL("Failed to initialise graphics system")
        }

        platform->Shutdown();
        Platform::DestroyPlatform(platform);

        EventSystem::Shutdown();
        MemorySystem::Shutdown();

        OTR_LOG_DEBUG("Total allocation after system shutdown: {0} / {1} bytes",
                      MemorySystem::GetUsedMemory(),
                      MemorySystem::GetMemorySize())
    }
}