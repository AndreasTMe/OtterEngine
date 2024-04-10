// TODO: Remove this.
#include <thread>

#include "Core/Application.h"
#include "Core/Events/EventSystem.h"
#include "ECS/World.h"
#include "Graphics/GraphicsSystem.h"

namespace Otter
{
    Application::Application(const ApplicationConfiguration& config)
        : k_Configuration(config)
    {
        MemorySystem::Initialise(k_Configuration.MemoryRequirements);
        EventSystem::Initialise();
    }

    void Application::Run()
    {
        if (!OnBeforeMainLoop())
            return;

        OnMainLoop();
        OnAfterMainLoop();
    }

    bool Application::OnBeforeMainLoop()
    {
        m_Platform = Platform::CreatePlatform({ k_Configuration.Title,
                                                k_Configuration.Width,
                                                k_Configuration.Height,
                                                WindowState::Default });
        if (!m_Platform->TryInitialise())
        {
            OTR_LOG_FATAL("Failed to initialise platform. Shutting down...")

            Platform::DestroyPlatform(m_Platform);

            return false;
        }

        if (!World::TryInitialise())
        {
            OTR_LOG_FATAL("Failed to initialise Entity-Component System. Shutting down...")

            m_Platform->Shutdown();
            Platform::DestroyPlatform(m_Platform);

            return false;
        }

        if (!GraphicsSystem::TryInitialise(m_Platform->GetUnsafeContext()))
        {
            OTR_LOG_FATAL("Failed to initialise graphics system. Shutting down...")

            World::Destroy();

            m_Platform->Shutdown();
            Platform::DestroyPlatform(m_Platform);

            return false;
        }

        OTR_LOG_DEBUG("Total allocation after system initialisation: {0} / {1} bytes",
                      MemorySystem::GetUsedMemory(),
                      MemorySystem::GetMemorySize())

        return true;
    }

    void Application::OnMainLoop()
    {
        m_Time = New<Time>(TimeConfiguration{ 60.0, 75.0, 0.01 }, [&]() { return m_Platform->GetAbsoluteTime(); });
        m_Time->Start();

        while (m_Platform->IsRunning())
        {
            m_Time->Refresh();

            // Logic Update
            m_Platform->CaptureWindowEvents();
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
    }

    void Application::OnAfterMainLoop()
    {
        Delete<Time>(m_Time);

        GraphicsSystem::Shutdown();

        World::Destroy();

        m_Platform->Shutdown();
        Platform::DestroyPlatform(m_Platform);

        for (auto* layer: m_Layers)
            Delete<Layer>(layer);

        m_Layers.ClearDestructive();

        EventSystem::Shutdown();
        MemorySystem::Shutdown();

        OTR_LOG_DEBUG("Total allocation after system shutdown: {0} / {1} bytes",
                      MemorySystem::GetUsedMemory(),
                      MemorySystem::GetMemorySize())
    }
}