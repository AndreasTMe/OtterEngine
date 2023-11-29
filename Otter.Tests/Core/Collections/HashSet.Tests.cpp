#include <gtest/gtest.h>

#include "Core/Collections/HashSet.h"

template<typename T>
using HashSet = Otter::HashSet<T>;

class HashSetFixture : public ::testing::Test
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