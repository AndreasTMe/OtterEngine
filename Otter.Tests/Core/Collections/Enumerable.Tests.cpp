#include <gtest/gtest.h>

#include "Core/Collections/Enumerable.h"
#include "Core/Collections/List.h"

template<typename T>
using Enumerable = Otter::Enumerable<T>;

template<typename T>
using List = Otter::List<T>;

class Enumerable_Fixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Otter::MemorySystem::Initialise(1_KiB);
    }

    void TearDown() override
    {
        EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
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

TEST_F(Enumerable_Fixture, Initialisation_Copy)
{
    List<int>       list       = { 1, 2, 3, 4, 5 };
    Enumerable<int> enumerable = Enumerable<int>::Copy(list.GetData(), 5);

    EXPECT_NE(enumerable.GetData(), nullptr);
    EXPECT_EQ(enumerable.GetCount(), 5);

    EXPECT_FALSE(enumerable.IsEmpty());

    int i = 0;

    for (auto& item: enumerable)
    {
        EXPECT_EQ(item, list[i++]);
        item = 0;
    }

    i = 0;
    for (auto& item: enumerable)
    {
        EXPECT_EQ(item, 0);
        EXPECT_NE(list[i++], 0);
    }
}

TEST_F(Enumerable_Fixture, Initialisation_Wrap)
{
    List<int>       list       = { 1, 2, 3, 4, 5 };
    Enumerable<int> enumerable = Enumerable<int>::Wrap(const_cast<int*>(list.GetData()), 5);

    EXPECT_NE(enumerable.GetData(), nullptr);
    EXPECT_EQ(enumerable.GetCount(), 5);

    EXPECT_FALSE(enumerable.IsEmpty());

    int i = 0;

    for (auto& item: enumerable)
    {
        EXPECT_EQ(item, list[i++]);
        item = 0;
    }

    i = 0;
    for (auto& item: enumerable)
    {
        EXPECT_EQ(item, 0);
        EXPECT_EQ(list[i++], 0);
    }
}

TEST_F(Enumerable_Fixture, Initialisation_Empty)
{
    Enumerable<int> enumerable = Enumerable<int>::Empty();

    EXPECT_EQ(enumerable.GetData(), nullptr);
    EXPECT_EQ(enumerable.GetCount(), 0);

    EXPECT_TRUE(enumerable.IsEmpty());
}

TEST_F(Enumerable_Fixture, Equality)
{
    Enumerable<int> enumerable1 = Enumerable<int>::Of({ 1, 2, 3, 4, 5 });
    Enumerable<int> enumerable2 = Enumerable<int>::Of({ 1, 2, 3, 4, 5 });
    Enumerable<int> enumerable3 = Enumerable<int>::Of({ 1, 2, 3, 4, 5, 6 });

    EXPECT_TRUE(enumerable1 == enumerable2);
    EXPECT_FALSE(enumerable1 == enumerable3);
}

TEST_F(Enumerable_Fixture, ClearDestructive)
{
    Enumerable<int> enumerable1 = Enumerable<int>::Of({ 1, 2, 3, 4, 5 });
    enumerable1.ClearDestructive();

    EXPECT_EQ(enumerable1.GetCount(), 0);
    EXPECT_EQ(enumerable1.GetData(), nullptr);

    Enumerable<List<int>> enumerable2 = Enumerable<List<int>>::Of({{ 1, 2, 3, 4, 5 },
                                                                   { 1, 2, 3, 4, 5 }});

    enumerable2.ClearDestructive();

    EXPECT_EQ(enumerable1.GetCount(), 0);
    EXPECT_EQ(enumerable1.GetData(), nullptr);
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