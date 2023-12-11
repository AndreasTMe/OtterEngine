// TODO: Remove this.
#include <thread>

#include "Core/Application.h"
#include "Core/Events/EventSystem.h"
#include "Graphics/GraphicsSystem.h"

namespace Otter
{
    Application::Application(const ApplicationConfiguration& config)
        : k_Configuration(config)
    {
        MemorySystem::Initialise(k_Configuration.MemoryRequirements);
        EventSystem::Initialise();
    }

    Application::~Application()
    {
        for (auto* layer: m_Layers)
            Delete<Layer>(layer);

        m_Layers.ClearDestructive();

        EventSystem::Shutdown();
        MemorySystem::Shutdown();

        OTR_LOG_DEBUG("Total allocation after system shutdown: {0} / {1} bytes",
                      MemorySystem::GetUsedMemory(),
                      MemorySystem::GetMemorySize())
    }

    void Application::Run()
    {
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

            m_Time = New<Time>(TimeConfiguration{ 60.0, 75.0, 0.01 },
                               [platform]() { return platform->GetAbsoluteTime(); });
            m_Time->Start();

            while (platform->IsRunning())
            {
                m_Time->Refresh();

                // Logic Update
                platform->CaptureWindowEvents();
                EventSystem::Process();

                for (const auto& layer: m_Layers)
                    layer->OnUpdate(m_Time->GetDeltaTime());

                // Physics Update
                while (m_Time->HasFixedStepsLeft())
                {
                    // TODO: Replace with physics system.
                    //  Physics::Update(m_Time->GetFixedDeltaTime());

                    std::this_thread::sleep_for(std::chrono::duration<double>(m_Time->GetFixedDeltaTime()));
                }

                // Render
                GraphicsSystem::RenderFrame();
            }

            Delete<Time>(m_Time);

            GraphicsSystem::Shutdown();
        }
        else
        {
            OTR_LOG_FATAL("Failed to initialise graphics system")
        }

        platform->Shutdown();
        Platform::DestroyPlatform(platform);
    }
}