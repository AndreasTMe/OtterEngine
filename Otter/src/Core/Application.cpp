#include "Otter.PCH.h"

#include "Core/Application.h"

namespace Otter
{
    void Application::Run()
    {
        // Initialise memory system;

        auto* platform = Platform::CreatePlatform();
        if (platform->Startup("Otter Engine", 100, 100, 1280, 720))
        {
            EventSystem::GetInstance()->Initialise();
            // Initialise application layers;

            while (platform->IsRunning())
            {
                platform->CaptureWindowEvents();

                EventSystem::GetInstance()->Process();
//                for (auto& layer: g_Layers)
//                    layer->OnUpdate();
            }

            // Shutdown application layers;
            EventSystem::GetInstance()->Shutdown();
        }

        platform->Shutdown();
        Platform::DestroyPlatform(platform);

        // Shutdown memory system;
    }
}