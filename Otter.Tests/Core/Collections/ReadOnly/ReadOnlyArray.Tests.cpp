#include <gtest/gtest.h>

#include "Core/Collections/ReadOnly/ReadOnlyArray.h"

template<typename T, UInt64 Size>
using ReadOnlyArray = Otter::ReadOnlyArray<T, Size>;

class ReadOnlyArrayFixture : public ::testing::Test
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