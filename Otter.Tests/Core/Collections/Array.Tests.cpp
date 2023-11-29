#include <gtest/gtest.h>

#include "Core/Collections/Array.h"

template<typename T, UInt64 Size>
using Array = Otter::Array<T, Size>;

class ArrayFixture : public ::testing::Test
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