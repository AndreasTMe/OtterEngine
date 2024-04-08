#include <gtest/gtest.h>

#include "Core/GlobalActions.h"
#include "Core/Events/EventSystem.h"

class EventSystem_Fixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Otter::MemorySystem::Initialise(2_KiB);
    }

    void TearDown() override
    {
        EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
        Otter::MemorySystem::Shutdown();
    }
};

TEST_F(EventSystem_Fixture, Schedule_And_Process)
{
    EXPECT_NO_THROW(Otter::EventSystem::Initialise());

    EXPECT_NO_THROW(Otter::EventSystem::Schedule<Otter::WindowCloseEvent>());
    EXPECT_NO_THROW(Otter::EventSystem::Schedule<Otter::WindowResizeEvent>(800, 600, true));
    EXPECT_NO_THROW(Otter::EventSystem::Schedule<Otter::WindowMinimizedEvent>());
    EXPECT_NO_THROW(Otter::EventSystem::Schedule<Otter::WindowMaximizedEvent>());
    EXPECT_NO_THROW(Otter::EventSystem::Schedule<Otter::WindowRestoredEvent>());

    EXPECT_NO_THROW(Otter::EventSystem::Schedule<Otter::KeyPressedEvent>(Otter::KeyCode::KeyA));
    EXPECT_NO_THROW(Otter::EventSystem::Schedule<Otter::KeyReleasedEvent>(Otter::KeyCode::KeyB));
    EXPECT_NO_THROW(Otter::EventSystem::Schedule<Otter::KeyRepeatEvent>(Otter::KeyCode::KeyC, 3));

    EXPECT_NO_THROW(Otter::EventSystem::Schedule<Otter::MouseButtonPressedEvent>(Otter::MouseButton::Left));
    EXPECT_NO_THROW(Otter::EventSystem::Schedule<Otter::MouseButtonReleasedEvent>(Otter::MouseButton::Right));
    EXPECT_NO_THROW(Otter::EventSystem::Schedule<Otter::MouseScrollEvent>(true));
    EXPECT_NO_THROW(Otter::EventSystem::Schedule<Otter::MouseMovedEvent>(100, 200));

    EXPECT_NO_THROW(Otter::EventSystem::Process());

    EXPECT_NO_THROW(Otter::EventSystem::Shutdown());
}