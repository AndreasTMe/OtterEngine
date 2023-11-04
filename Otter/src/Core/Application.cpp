#include "Otter.PCH.h"

#include "Core/Application.h"
#include "Graphics/GraphicsSystem.h"

namespace Otter
{
    void Application::Run()
    {
        MemorySystem::Initialise();

        auto* platform = Platform::CreatePlatform();
        if (!platform->Startup("Otter Engine", 100, 100, 1280, 720))
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