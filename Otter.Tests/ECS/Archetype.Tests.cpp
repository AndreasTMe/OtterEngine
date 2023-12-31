#include <gtest/gtest.h>

#include "ECS/Archetype.h"

using Archetype = Otter::Archetype;

class Archetype_Fixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Otter::MemorySystem::Initialise(1_KiB);
    }

    void TearDown() override
    {
        Otter::MemorySystem::Shutdown();
    }
};

TEST_F(Archetype_Fixture, DefaultConstructor)
{
    {
        Archetype archetype;

        EXPECT_EQ(archetype.GetEntityCount(), 0);
        EXPECT_EQ(archetype.GetComponentCount(), 0);
    }

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
}