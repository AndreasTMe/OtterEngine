#include "Otter.PCH.h"

#include "Core/Application.h"
#include "Graphics/GraphicsSystem.h"

namespace Otter
{
    // TODO: Move this to a configuration file.
    struct ApplicationArgs
    {
        const char* m_Title;
        UInt32 m_Left;
        UInt32 m_Top;
        UInt32 m_Width;
        UInt32 m_Height;
        UInt64 m_MemoryRequirements;
    };

    const static ApplicationArgs gk_Args = {
        "Otter Engine",
        100,
        100,
        1280,
        720,
        10_KiB
    };

    void Application::Run()
    {
        MemorySystem::Initialise(gk_Args.m_MemoryRequirements);

        auto* platform = Platform::CreatePlatform();
        if (!platform->Startup(gk_Args.m_Title, gk_Args.m_Left, gk_Args.m_Top, gk_Args.m_Width, gk_Args.m_Height))
        {
            OTR_LOG_FATAL("Failed to initialise platform. Shutting down...")

            Platform::DestroyPlatform(platform);
            MemorySystem::Shutdown();

            return;
        }

        EventSystem::Initialise();

        if (GraphicsSystem::TryInitialise(platform->GetUnsafeContext()))
        {
            OTR_LOG_DEBUG("Total allocation after system initialisation: {0}", MemorySystem::GetTotalAllocation())

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

        EventSystem::Shutdown();

        platform->Shutdown();
        Platform::DestroyPlatform(platform);

        MemorySystem::Shutdown();

        OTR_LOG_DEBUG("Total allocation after system shutdown: {0}", MemorySystem::GetTotalAllocation())
    }
}