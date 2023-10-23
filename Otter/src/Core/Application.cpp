#include "Otter.PCH.h"

#include "Core/Application.h"

namespace Otter
{
    void Application::Run()
    {
        Memory::GetInstance()->Initialise();

        auto* platform = Platform::CreatePlatform();
        if (platform->Startup("Otter Engine", 100, 100, 1280, 720))
        {
            EventSystem::GetInstance()->Initialise();

            while (platform->IsRunning())
            {
                platform->CaptureWindowEvents();
                EventSystem::GetInstance()->Process();
            }

            EventSystem::GetInstance()->Shutdown();
        }

        platform->Shutdown();
        Platform::DestroyPlatform(platform);

        Memory::GetInstance()->Shutdown();
    }
}