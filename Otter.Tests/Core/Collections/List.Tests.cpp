#include <gtest/gtest.h>

#include "Core/Collections/List.h"

template<typename T>
using List = Otter::List<T>;

class ListFixture : public ::testing::Test
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