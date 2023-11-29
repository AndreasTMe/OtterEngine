#include <gtest/gtest.h>

#include "Core/Collections/Queue.h"

template<typename T>
using Queue = Otter::Queue<T>;

class QueueFixture : public ::testing::Test
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