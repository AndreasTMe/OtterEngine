#include <gtest/gtest.h>

#include "Core/Collections/UnsafeList.h"

using UnsafeList = Otter::UnsafeList;

class UnsafeList_Fixture : public ::testing::Test
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