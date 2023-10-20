#include "Otter.PCH.h"

#include "Core/Events/EventSystem.h"

namespace Otter
{
    void EventSystem::Initialise()
    {
        static bool isInitialised = false;
        OTR_INTERNAL_ASSERT_MSG(!isInitialised, "Event system has already been initialised")

        // Window Events
        AddListener(Internal::EventType::WindowClose, GlobalActions::OnWindowClose);
        AddListener(Internal::EventType::WindowResize, GlobalActions::OnWindowResize);
        AddListener(Internal::EventType::WindowMinimized, GlobalActions::OnWindowMinimized);
        AddListener(Internal::EventType::WindowMaximized, GlobalActions::OnWindowMaximized);
        AddListener(Internal::EventType::WindowRestored, GlobalActions::OnWindowRestored);
        AddListener(Internal::EventType::WindowRefresh, GlobalActions::OnWindowRefresh);

        // Keyboard Events
        AddListener(Internal::EventType::KeyPressed, GlobalActions::OnKeyPressed);
        AddListener(Internal::EventType::KeyReleased, GlobalActions::OnKeyReleased);
        AddListener(Internal::EventType::KeyHold, GlobalActions::OnKeyHold);

        // Mouse Events
        AddListener(Internal::EventType::MouseButtonPressed, GlobalActions::OnMouseButtonPressed);
        AddListener(Internal::EventType::MouseButtonReleased, GlobalActions::OnMouseButtonReleased);
        AddListener(Internal::EventType::MouseScroll, GlobalActions::OnMouseScroll);
        AddListener(Internal::EventType::MouseMoved, GlobalActions::OnMouseMoved);
        AddListener(Internal::EventType::MouseDragged, GlobalActions::OnMouseDragged);

        isInitialised = true;
    }

    void EventSystem::Shutdown()
    {
        RemoveAllListeners();
        ClearEvents();
    }

    void EventSystem::Process()
    {
        while (!m_Events.empty())
        {
            Event& event = m_Events.front();
            auto type = event.GetEventType();

            if (m_Listeners.find(type) != m_Listeners.end())
                m_Listeners[type]->Invoke(event);

            m_Events.pop();
        }
    }

    template<typename TEvent, typename TActionArg>
    void EventSystem::AddListener(EventType type, const Action<TActionArg>& action)
    {
        OTR_INTERNAL_ASSERT_MSG(type != Internal::EventType::None, "Event type cannot be None")
        OTR_INTERNAL_ASSERT_MSG(type != Internal::EventType::Max, "Event type cannot be Max")

        if (m_Listeners.find(type) != m_Listeners.end())
            return;

        m_Listeners[type] = CreateSharedPointer<EventAction>((EventAction * ) & action);
    }

    void EventSystem::RemoveAllListeners()
    {
        for (auto& listener: m_Listeners)
            listener.second->Clear();

        m_Listeners.clear();
    }

    void EventSystem::ClearEvents()
    {
        while (!m_Events.empty())
            m_Events.pop();
    }
}