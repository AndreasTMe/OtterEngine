#include <gtest/gtest.h>

#include "Core/Collections/List.h"

template<typename T>
using List = Otter::List<T>;

class List_Fixture : public ::testing::Test
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

TEST_F(List_Fixture, Initialisation_Default)
{
    List<int> list;

    EXPECT_EQ(list.GetData(), nullptr);
    EXPECT_EQ(list.GetCapacity(), 0);
    EXPECT_EQ(list.GetCount(), 0);

    EXPECT_TRUE(list.IsEmpty());
}

TEST_F(List_Fixture, Initialisation_FromInitialisationList)
{
    List<int> list = { 1, 2, 3, 4, 5 };

    EXPECT_NE(list.GetData(), nullptr);
    EXPECT_EQ(list.GetCapacity(), 5);
    EXPECT_EQ(list.GetCount(), 5);

    for (UInt64 i = 0; i < list.GetCount(); ++i)
        EXPECT_EQ(list[i], i + 1);
}

TEST_F(List_Fixture, Initialisation_Copy)
{
    List<int> list = { 1, 2, 3, 4, 5 };
    List<int> copy = list;

    EXPECT_EQ(copy.GetData(), list.GetData());
    EXPECT_EQ(copy.GetCapacity(), list.GetCapacity());
    EXPECT_EQ(copy.GetCount(), list.GetCount());

    for (UInt64 i = 0; i < copy.GetCount(); ++i)
        EXPECT_EQ(copy[i], i + 1);
}

TEST_F(List_Fixture, Initialisation_Move)
{
    List<int> list = { 1, 2, 3, 4, 5 };
    List<int> move = std::move(list);

    EXPECT_NE(move.GetData(), nullptr);
    EXPECT_EQ(move.GetCapacity(), 5);
    EXPECT_EQ(move.GetCount(), 5);

    for (UInt64 i = 0; i < move.GetCount(); ++i)
        EXPECT_EQ(move[i], i + 1);

    EXPECT_EQ(list.GetData(), nullptr);
}

TEST_F(List_Fixture, Assignment_Copy)
{
    List<int> list = { 1, 2, 3, 4, 5 };
    List<int> copy;
    copy = list;

    EXPECT_EQ(copy.GetData(), list.GetData());
    EXPECT_EQ(copy.GetCapacity(), list.GetCapacity());
    EXPECT_EQ(copy.GetCount(), list.GetCount());

    for (UInt64 i = 0; i < copy.GetCount(); ++i)
        EXPECT_EQ(copy[i], i + 1);
}

TEST_F(List_Fixture, Assignment_Move)
{
    List<int> list = { 1, 2, 3, 4, 5 };
    List<int> move;
    move = std::move(list);

    EXPECT_NE(move.GetData(), nullptr);
    EXPECT_EQ(move.GetCapacity(), 5);
    EXPECT_EQ(move.GetCount(), 5);

    for (UInt64 i = 0; i < move.GetCount(); ++i)
        EXPECT_EQ(move[i], i + 1);

    EXPECT_EQ(list.GetData(), nullptr);
}

TEST_F(List_Fixture, Add)
{
    List<int> list;
    list.Add(1);

    EXPECT_NE(list.GetData(), nullptr);
    EXPECT_EQ(list.GetCapacity(), 2);
    EXPECT_EQ(list.GetCount(), 1);
    auto oldCapacity = list.GetCapacity();

    int value = 2;
    list.Add(value);
    list.Add(3);

    EXPECT_EQ(list.GetCapacity(), oldCapacity * 1.5);
    EXPECT_EQ(list.GetCount(), 3);

    for (UInt64 i = 0; i < list.GetCount(); ++i)
        EXPECT_EQ(list[i], i + 1);
}

TEST_F(List_Fixture, TryAddAt)
{
    List<int> list;
    list.Reserve(3);
    EXPECT_TRUE(list.TryAddAt(0, 0));

    EXPECT_NE(list.GetData(), nullptr);
    EXPECT_EQ(list.GetCapacity(), 3);
    EXPECT_EQ(list.GetCount(), 1);

    int value = 1;
    EXPECT_TRUE(list.TryAddAt(1, value));
    EXPECT_TRUE(list.TryAddAt(2, 2));

    EXPECT_EQ(list.GetCount(), 3);

    for (UInt64 i = 0; i < list.GetCount(); ++i)
        EXPECT_EQ(list[i], i);

    EXPECT_FALSE(list.TryAddAt(123, 3));
    EXPECT_FALSE(list.TryAddAt(0, 3));
}

TEST_F(List_Fixture, TryAddRange)
{
    List<int> list;
    EXPECT_TRUE(list.TryAddRange({ 1, 2, 3, 4, 5 }));

    EXPECT_NE(list.GetData(), nullptr);
    EXPECT_EQ(list.GetCapacity(), 5);
    EXPECT_EQ(list.GetCount(), 5);

    for (UInt64 i = 0; i < list.GetCount(); ++i)
        EXPECT_EQ(list[i], i + 1);

    list.Reserve(3);
    EXPECT_FALSE(list.TryAddRange({ 1, 2, 3, 4, 5 }, true));

    EXPECT_NE(list.GetData(), nullptr);
    EXPECT_EQ(list.GetCapacity(), 3);
    EXPECT_EQ(list.GetCount(), 0);
}

TEST_F(List_Fixture, TryRemoveAt)
{
    List<int> list = { 1, 2, 3, 4, 5 };

    EXPECT_TRUE(list.TryRemoveAt(0));
    EXPECT_EQ(list.GetCount(), 4);

    EXPECT_TRUE(list.TryRemoveAt(3));
    EXPECT_EQ(list.GetCount(), 3);

    EXPECT_FALSE(list.TryRemoveAt(123));
}

TEST_F(List_Fixture, TryRemove)
{
    int       value = 2;
    List<int> list  = { 1, value, 3, 4, 5 };

    EXPECT_TRUE(list.TryRemove(3));
    EXPECT_EQ(list.GetCount(), 4);

    EXPECT_TRUE(list.TryRemove(1));
    EXPECT_EQ(list.GetCount(), 3);

    EXPECT_TRUE(list.TryRemove(5));
    EXPECT_EQ(list.GetCount(), 2);

    EXPECT_TRUE(list.TryRemove(value));
    EXPECT_EQ(list.GetCount(), 1);

    EXPECT_TRUE(list.TryRemove(4));
    EXPECT_EQ(list.GetCount(), 0);

    EXPECT_FALSE(list.TryRemove(4));
    EXPECT_EQ(list.GetCount(), 0);
}

TEST_F(List_Fixture, GetMemoryFootprint)
{
    List<int> list = { 1, 2, 3, 4, 5 };

    auto footprint1 = list.GetMemoryFootprint(OTR_NAME_OF(List<int>));
    EXPECT_EQ(footprint1.GetSize(), 1);

    EXPECT_EQ(footprint1[0].GetData().GetName(), OTR_NAME_OF(List<int>));
    EXPECT_EQ(footprint1[0].GetData().GetPointer(), list.GetData());
    EXPECT_EQ(footprint1[0].Size, OTR_ALLOCATED_MEMORY(int, list.GetCapacity()));
    EXPECT_EQ(footprint1[0].Offset, Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(footprint1[0].Padding, 0);
    EXPECT_EQ(footprint1[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

    list.Add(6);

    auto footprint2 = list.GetMemoryFootprint(OTR_NAME_OF(List<int>));
    EXPECT_EQ(footprint2.GetSize(), 1);

    EXPECT_EQ(footprint2[0].GetData().GetName(), OTR_NAME_OF(List<int>));
    EXPECT_EQ(footprint2[0].GetData().GetPointer(), list.GetData());
    EXPECT_NE(footprint2[0].GetData().GetPointer(), footprint1[0].GetData().GetPointer())
                    << "Pointer should have changed because of capacity increase (reallocation)";
    EXPECT_EQ(footprint2[0].Size, OTR_ALLOCATED_MEMORY(int, list.GetCapacity()));
    EXPECT_NE(footprint2[0].Offset, footprint1[0].Offset)
                    << "Offset should have changed because of capacity increase (reallocation)";
    EXPECT_EQ(footprint2[0].Padding, 0);
    EXPECT_EQ(footprint2[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

    list.ClearDestructive();

    auto footprint3 = list.GetMemoryFootprint(OTR_NAME_OF(List<int>));
    EXPECT_EQ(footprint3.GetSize(), 1);

    EXPECT_EQ(footprint3[0].GetData().GetName(), OTR_NAME_OF(List<int>));
    EXPECT_EQ(footprint3[0].GetData().GetPointer(), nullptr);
    EXPECT_EQ(footprint3[0].Size, 0);
    EXPECT_EQ(footprint3[0].Offset, 0);
    EXPECT_EQ(footprint3[0].Padding, 0);
    EXPECT_EQ(footprint3[0].Alignment, 0);
}