#include <gtest/gtest.h>

#include "Core/Collections/Collection.h"

template<typename T>
using Collection = Otter::Collection<T>;

class CollectionFixture : public ::testing::Test
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