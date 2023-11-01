#include "Otter.PCH.h"

#include "Core/Application.h"

namespace Otter
{
    void Application::Run()
    {
        MemorySystem::Initialise();

        auto* platform = Platform::CreatePlatform();
        if (platform->Startup("Otter Engine", 100, 100, 1280, 720))
        {
            EventSystem::Initialise();

            OTR_LOG_DEBUG("Total allocation after system initialisation: {0}", MemorySystem::GetTotalAllocation())

            while (platform->IsRunning())
            {
                platform->CaptureWindowEvents();
                EventSystem::Process();
            }

            EventSystem::Shutdown();
        }

        platform->Shutdown();
        Platform::DestroyPlatform(platform);

        MemorySystem::Shutdown();

        OTR_LOG_DEBUG("Total allocation after system shutdown: {0}", MemorySystem::GetTotalAllocation())
    }
}