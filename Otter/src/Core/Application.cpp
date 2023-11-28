#include "Otter.PCH.h"

#include "Core/Application.h"
#include "Graphics/GraphicsSystem.h"

namespace Otter
{
    // TODO: Move this to a configuration file.
    struct ApplicationConfiguration final
    {
        const char* Title;
        UInt16 Width;
        UInt16 Height;
        UInt64 MemoryRequirements;
    };

    const static ApplicationConfiguration gk_Args = {
        "Otter Engine",
        1280,
        720,
        10_KiB
    };

    void Application::Run()
    {
        MemorySystem::Initialise(gk_Args.MemoryRequirements);
        EventSystem::Initialise();

        auto* platform = Platform::CreatePlatform({ gk_Args.Title,
                                                    gk_Args.Width,
                                                    gk_Args.Height,
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