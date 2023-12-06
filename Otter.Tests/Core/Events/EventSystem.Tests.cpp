#include <gtest/gtest.h>

#include "Core/GlobalActions.h"
#include "Core/Events/EventSystem.h"

class EventSystem_Fixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        OTR_MEMORY_SYSTEM.Initialise(2_KiB);
    }

    void TearDown() override
    {
        OTR_MEMORY_SYSTEM.Shutdown();
    }
};

TEST_F(EventSystem_Fixture, Schedule_And_Process)
{
    EXPECT_NO_THROW(OTR_EVENT_SYSTEM.Initialise());

    EXPECT_NO_THROW(OTR_EVENT_SYSTEM.Schedule<Otter::WindowCloseEvent>());
    EXPECT_NO_THROW(OTR_EVENT_SYSTEM.Schedule<Otter::WindowResizeEvent>(800, 600, true));
    EXPECT_NO_THROW(OTR_EVENT_SYSTEM.Schedule<Otter::WindowMinimizedEvent>());
    EXPECT_NO_THROW(OTR_EVENT_SYSTEM.Schedule<Otter::WindowMaximizedEvent>());
    EXPECT_NO_THROW(OTR_EVENT_SYSTEM.Schedule<Otter::WindowRestoredEvent>());

    EXPECT_NO_THROW(OTR_EVENT_SYSTEM.Schedule<Otter::KeyPressedEvent>(Otter::KeyCode::KeyA));
    EXPECT_NO_THROW(OTR_EVENT_SYSTEM.Schedule<Otter::KeyReleasedEvent>(Otter::KeyCode::KeyB));
    EXPECT_NO_THROW(OTR_EVENT_SYSTEM.Schedule<Otter::KeyRepeatEvent>(Otter::KeyCode::KeyC, 3));

    EXPECT_NO_THROW(OTR_EVENT_SYSTEM.Schedule<Otter::MouseButtonPressedEvent>(Otter::MouseButton::Left));
    EXPECT_NO_THROW(OTR_EVENT_SYSTEM.Schedule<Otter::MouseButtonReleasedEvent>(Otter::MouseButton::Right));
    EXPECT_NO_THROW(OTR_EVENT_SYSTEM.Schedule<Otter::MouseScrollEvent>(true));
    EXPECT_NO_THROW(OTR_EVENT_SYSTEM.Schedule<Otter::MouseMovedEvent>(100, 200));

    EXPECT_NO_THROW(OTR_EVENT_SYSTEM.Process());

    EXPECT_NO_THROW(OTR_EVENT_SYSTEM.Shutdown());
}