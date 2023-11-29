#include <gtest/gtest.h>

#include "Core/Collections/Dictionary.h"

template<typename TKey, typename TValue>
using Dictionary = Otter::Dictionary<TKey, TValue>;

class DictionaryFixture : public ::testing::Test
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