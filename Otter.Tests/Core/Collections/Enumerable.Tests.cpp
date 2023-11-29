#include <gtest/gtest.h>

#include "Core/Collections/Enumerable.h"

template<typename T>
using Enumerable = Otter::Enumerable<T>;

class EnumerableFixture : public ::testing::Test
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