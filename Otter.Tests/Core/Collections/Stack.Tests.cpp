#include <gtest/gtest.h>

#include "Core/Collections/Stack.h"

template<typename T>
using Stack = Otter::Stack<T>;

class StackFixture : public ::testing::Test
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