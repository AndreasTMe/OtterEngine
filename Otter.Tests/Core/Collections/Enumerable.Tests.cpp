#include <gtest/gtest.h>

#include "Core/Collections/Enumerable.h"

template<typename T>
using Enumerable = Otter::Enumerable<T>;

class Enumerable_Fixture : public ::testing::Test
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

TEST_F(Enumerable_Fixture, Initialisation_Default)
{
    Enumerable<int> enumerable = Enumerable<int>::Of({ 1, 2, 3, 4, 5 });

    EXPECT_NE(enumerable.GetData(), nullptr);
    EXPECT_EQ(enumerable.GetCount(), 5);

    EXPECT_FALSE(enumerable.IsEmpty());
}

TEST_F(Enumerable_Fixture, Initialisation_Empty)
{
    Enumerable<int> enumerable = Enumerable<int>::Empty();

    EXPECT_EQ(enumerable.GetData(), nullptr);
    EXPECT_EQ(enumerable.GetCount(), 0);

    EXPECT_TRUE(enumerable.IsEmpty());
}

TEST_F(Enumerable_Fixture, ClearDestructive)
{
    Enumerable<int> enumerable = Enumerable<int>::Of({ 1, 2, 3, 4, 5 });
    enumerable.ClearDestructive();

    EXPECT_EQ(enumerable.GetCount(), 0);
    EXPECT_EQ(enumerable.GetData(), nullptr);
}

TEST_F(Enumerable_Fixture, Iterator)
{
    Enumerable<int> enumerable = Enumerable<int>::Of({ 1, 2, 3, 4, 5 });

    UInt64 i = 0;
    for (auto& element: enumerable)
        EXPECT_EQ(element, ++i);

    for (auto it = enumerable.rbegin(); it != enumerable.rend(); --it)
        EXPECT_EQ(*it, i--);

    for (auto it = enumerable.cbegin(); it != enumerable.cend(); ++it)
        EXPECT_EQ(*it, ++i);

    for (auto it = enumerable.crbegin(); it != enumerable.crend(); --it)
        EXPECT_EQ(*it, i--);
}