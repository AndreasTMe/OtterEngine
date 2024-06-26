#include <gtest/gtest.h>

#include "Core/Collections/UnsafeList.h"
#include "Core/Collections/List.h"

using UnsafeList = Otter::UnsafeList;

template<typename T>
using List = Otter::List<T>;

class UnsafeList_Fixture : public ::testing::Test
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

TEST_F(UnsafeList_Fixture, Initialisation_Default)
{
    UnsafeList list = UnsafeList::Empty<int>();

    EXPECT_EQ(list.GetData(), nullptr);
    EXPECT_EQ(list.GetCapacity(), 0);
    EXPECT_EQ(list.GetCount(), 0);
    EXPECT_EQ(list.GetOffset(), sizeof(int));

    EXPECT_TRUE(list.IsEmpty());
    EXPECT_FALSE(list.IsCreated());
}

TEST_F(UnsafeList_Fixture, Initialisation_FromInitialisationList)
{
    UnsafeList list = UnsafeList::Of<int>({ 1, 2, 3, 4, 5 });

    EXPECT_NE(list.GetData(), nullptr);
    EXPECT_EQ(*list.GetData<int>(), 1);
    EXPECT_EQ(list.GetCapacity(), 5);
    EXPECT_EQ(list.GetCount(), 5);

    EXPECT_FALSE(list.IsEmpty());
    EXPECT_TRUE(list.IsCreated());
}

TEST_F(UnsafeList_Fixture, Initialisation_Copy)
{
    UnsafeList list = UnsafeList::Of<int>({ 1, 2, 3, 4, 5 });
    UnsafeList copy = list;

    EXPECT_NE(copy.GetData(), list.GetData());
    EXPECT_EQ(*copy.GetData<int>(), 1);
    EXPECT_EQ(copy.GetCapacity(), list.GetCapacity());
    EXPECT_EQ(copy.GetCount(), list.GetCount());

    EXPECT_FALSE(copy.IsEmpty());
    EXPECT_TRUE(copy.IsCreated());
}

TEST_F(UnsafeList_Fixture, Initialisation_Move)
{
    UnsafeList list = UnsafeList::Of<int>({ 1, 2, 3, 4, 5 });
    UnsafeList move = std::move(list);

    EXPECT_NE(move.GetData(), list.GetData());
    EXPECT_EQ(*move.GetData<int>(), 1);
    EXPECT_NE(move.GetCapacity(), list.GetCapacity());
    EXPECT_NE(move.GetCount(), list.GetCount());

    EXPECT_FALSE(move.IsEmpty());
    EXPECT_TRUE(move.IsCreated());
}

TEST_F(UnsafeList_Fixture, Initialisation_CopyAssignment)
{
    UnsafeList list = UnsafeList::Of<int>({ 1, 2, 3, 4, 5 });
    UnsafeList copy = UnsafeList::Empty<int>();

    copy = list;

    EXPECT_NE(copy.GetData(), list.GetData());
    EXPECT_EQ(*copy.GetData<int>(), 1);
    EXPECT_EQ(copy.GetCapacity(), list.GetCapacity());
    EXPECT_EQ(copy.GetCount(), list.GetCount());

    EXPECT_FALSE(copy.IsEmpty());
    EXPECT_TRUE(copy.IsCreated());

    EXPECT_TRUE(list.IsCreated());
}

TEST_F(UnsafeList_Fixture, Initialisation_MoveAssignment)
{
    UnsafeList list = UnsafeList::Of<int>({ 1, 2, 3, 4, 5 });
    UnsafeList move = UnsafeList::Empty<int>();

    move = std::move(list);

    EXPECT_NE(move.GetData(), list.GetData());
    EXPECT_EQ(*move.GetData<int>(), 1);
    EXPECT_NE(move.GetCapacity(), list.GetCapacity());
    EXPECT_NE(move.GetCount(), list.GetCount());

    EXPECT_FALSE(move.IsEmpty());
    EXPECT_TRUE(move.IsCreated());

    EXPECT_FALSE(list.IsCreated());
}

TEST_F(UnsafeList_Fixture, TryGet)
{
    UnsafeList list = UnsafeList::Of<int>({ 1, 2, 3, 4, 5 });

    int value = 0;
    EXPECT_TRUE(list.TryGet(0, &value));
    EXPECT_EQ(value, 1);

    EXPECT_TRUE(list.TryGet(4, &value));
    EXPECT_EQ(value, 5);

    EXPECT_FALSE(list.TryGet(5, &value));
    EXPECT_FALSE(list.TryGet(-1, &value));

    Byte data[list.GetOffset()];
    EXPECT_TRUE(list.TryGetUnsafe(0, data));
    EXPECT_EQ(*(int*) data, 1);
}

TEST_F(UnsafeList_Fixture, Add)
{
    UnsafeList list = UnsafeList::Empty<int>();

    EXPECT_EQ(list.GetCount(), 0);
    EXPECT_EQ(list.GetCapacity(), 0);

    list.Add(1);
    EXPECT_EQ(list.GetCount(), 1);

    int item = 123;
    list.Add(item);
    EXPECT_EQ(list.GetCount(), 2);

    list.Add(3);
    EXPECT_EQ(list.GetCount(), 3);

    list.Add(4);
    EXPECT_EQ(list.GetCount(), 4);

    list.Add(5);
    EXPECT_EQ(list.GetCount(), 5);

    int value = 0;
    EXPECT_TRUE(list.TryGet(2, &value));
    EXPECT_EQ(value, 3);

    *list.operator[]<int>(0) = 321;
    EXPECT_TRUE(list.TryGet(0, &value));
    EXPECT_EQ(value, 321);
}

struct TestStruct
{
    int A;
    int B;
    int C;
};

TEST_F(UnsafeList_Fixture, Add_Item)
{
    UnsafeList list = UnsafeList::Empty<TestStruct>();

    EXPECT_EQ(list.GetCount(), 0);
    EXPECT_EQ(list.GetCapacity(), 0);

    TestStruct item1 = { 1, 2, 3 };
    list.Add(item1);
    EXPECT_EQ(list.GetCount(), 1);

    TestStruct item2 = { 4, 5, 6 };
    Byte* data = reinterpret_cast<Byte*>(&item2);

    list.Add(data, sizeof(TestStruct));
    EXPECT_EQ(list.GetCount(), 2);

    TestStruct  value{ };
    for (UInt64 i = 0; i < list.GetCount(); ++i)
    {
        EXPECT_TRUE(list.TryGet(i, &value));

        EXPECT_EQ(value.A, i * 3 + 1);
        EXPECT_EQ(value.B, i * 3 + 2);
        EXPECT_EQ(value.C, i * 3 + 3);
    }
}

TEST_F(UnsafeList_Fixture, TryAddAt)
{
    UnsafeList list = UnsafeList::Empty<int>();
    list.Add(1);
    list.Add(2);
    list.Add(3);
    list.Add(4);
    list.Add(5);

    EXPECT_EQ(list.GetCount(), 5);
    EXPECT_GT(list.GetCapacity(), list.GetCount());

    list.TryAddAt(2, 123);
    EXPECT_EQ(list.GetCount(), 6);

    int value = 0;
    EXPECT_TRUE(list.TryGet(0, &value));
    EXPECT_EQ(value, 1);

    EXPECT_TRUE(list.TryGet(1, &value));
    EXPECT_EQ(value, 2);

    EXPECT_TRUE(list.TryGet(2, &value));
    EXPECT_EQ(value, 123);

    EXPECT_TRUE(list.TryGet(3, &value));
    EXPECT_EQ(value, 3);

    EXPECT_TRUE(list.TryGet(4, &value));
    EXPECT_EQ(value, 4);

    EXPECT_TRUE(list.TryGet(5, &value));
    EXPECT_EQ(value, 5);
}

TEST_F(UnsafeList_Fixture, TryAddRange)
{
    UnsafeList list = UnsafeList::Empty<int>();

    EXPECT_EQ(list.GetCount(), 0);
    EXPECT_EQ(list.GetCapacity(), 0);

    list.TryAddRange<int>({ 1, 2, 3, 4, 5 }, true);
    EXPECT_EQ(list.GetCount(), 0);

    list.TryAddRange<int>({ 1, 2, 3, 4, 5 });
    EXPECT_EQ(list.GetCount(), 5);

    list.TryAddRange(List<int>{ 1, 2, 3, 4, 5 });
    EXPECT_EQ(list.GetCount(), 10);

    list.TryAddRange(UnsafeList::Of<int>({ 1, 2, 3, 4, 5 }));
    EXPECT_EQ(list.GetCount(), 15);
}

TEST_F(UnsafeList_Fixture, TryRemove)
{
    UnsafeList list = UnsafeList::Empty<int>();
    list.Add(1);
    list.Add(2);
    list.Add(3);
    list.Add(4);
    list.Add(5);

    EXPECT_EQ(list.GetCount(), 5);

    EXPECT_TRUE(list.TryRemove(3));
    EXPECT_EQ(list.GetCount(), 4);

    for (UInt64 i = 0; i < list.GetCount(); ++i)
    {
        int value = 0;
        EXPECT_TRUE(list.TryGet(i, &value));
        EXPECT_NE(value, 3);
    }

    EXPECT_TRUE(list.TryRemove(1));
    EXPECT_EQ(list.GetCount(), 3);

    for (UInt64 i = 0; i < list.GetCount(); ++i)
    {
        int value = 0;
        EXPECT_TRUE(list.TryGet(i, &value));
        EXPECT_NE(value, 1);
    }

    EXPECT_FALSE(list.TryRemove(123));
}

TEST_F(UnsafeList_Fixture, Contains)
{
    UnsafeList list = UnsafeList::Empty<int>();
    list.Add(1);
    list.Add(2);
    list.Add(3);
    list.Add(4);
    list.Add(5);

    EXPECT_TRUE(list.Contains(1));
    EXPECT_TRUE(list.Contains(2));
    EXPECT_TRUE(list.Contains(3));
    EXPECT_TRUE(list.Contains(4));
    EXPECT_TRUE(list.Contains(5));

    EXPECT_FALSE(list.Contains(123));
}

TEST_F(UnsafeList_Fixture, TryGetIndexOf)
{
    UnsafeList list = UnsafeList::Of<int>({ 1, 2, 3, 4, 5 });

    UInt64 index;
    EXPECT_TRUE(list.TryGetIndexOf(1, &index));
    EXPECT_EQ(index, 0);

    EXPECT_TRUE(list.TryGetIndexOf(2, &index));
    EXPECT_EQ(index, 1);

    EXPECT_TRUE(list.TryGetIndexOf(3, &index));
    EXPECT_EQ(index, 2);

    EXPECT_TRUE(list.TryGetIndexOf(4, &index));
    EXPECT_EQ(index, 3);

    EXPECT_TRUE(list.TryGetIndexOf(5, &index));
    EXPECT_EQ(index, 4);

    EXPECT_FALSE(list.TryGetIndexOf(123, &index));
}

TEST_F(UnsafeList_Fixture, Reserve)
{
    UnsafeList list = UnsafeList::Of<int>({ 1, 2, 3, 4, 5 });
    list.Reserve(10);

    EXPECT_NE(list.GetData(), nullptr);
    EXPECT_EQ(list.GetCapacity(), 10);
    EXPECT_EQ(list.GetCount(), 0);
}

TEST_F(UnsafeList_Fixture, Expand)
{
    UnsafeList list = UnsafeList::Of<int>({ 1, 2, 3, 4, 5 });

    const auto oldCapacity  = list.GetCapacity();
    const auto oldCount     = list.GetCount();
    const auto expandAmount = 10;
    list.Expand(expandAmount);

    EXPECT_NE(list.GetData(), nullptr);
    EXPECT_EQ(list.GetCapacity(), oldCapacity + expandAmount);
    EXPECT_EQ(list.GetCount(), oldCount);
}

TEST_F(UnsafeList_Fixture, Shrink)
{
    UnsafeList emptyList = UnsafeList::Empty<int>();
    EXPECT_EQ(emptyList.GetCapacity(), 0);
    EXPECT_EQ(emptyList.GetCount(), 0);

    emptyList.Shrink(10);

    EXPECT_EQ(emptyList.GetCapacity(), 0);
    EXPECT_EQ(emptyList.GetCount(), 0);

    UnsafeList list = UnsafeList::Of<int>({ 1, 2, 3, 4, 5 });

    const auto oldCapacity  = list.GetCapacity();
    const auto oldCount     = list.GetCount();
    const auto shrinkAmount = 3;

    list.Shrink(shrinkAmount, true);

    EXPECT_NE(list.GetData(), nullptr);
    EXPECT_EQ(list.GetCapacity(), oldCapacity - shrinkAmount);
    EXPECT_EQ(list.GetCount(), oldCount - shrinkAmount);
}

TEST_F(UnsafeList_Fixture, Clear)
{
    UnsafeList list = UnsafeList::Of<int>({ 1, 2, 3, 4, 5 });

    EXPECT_EQ(list.GetCount(), 5);
    EXPECT_EQ(list.GetCapacity(), 5);

    list.Clear();
    EXPECT_EQ(list.GetCount(), 0);
    EXPECT_EQ(list.GetCapacity(), 5);
}

TEST_F(UnsafeList_Fixture, ClearDestructive)
{
    UnsafeList list = UnsafeList::Of<int>({ 1, 2, 3, 4, 5 });

    EXPECT_EQ(list.GetCount(), 5);
    EXPECT_EQ(list.GetCapacity(), 5);

    list.ClearDestructive();
    EXPECT_EQ(list.GetCount(), 0);
    EXPECT_EQ(list.GetCapacity(), 0);
}

TEST_F(UnsafeList_Fixture, GetMemoryFootprint)
{
    UnsafeList list = UnsafeList::Of<int>({ 1, 2, 3, 4, 5 });

    auto footprint1 = list.GetMemoryFootprint(OTR_NAME_OF(UnsafeList));
    EXPECT_EQ(footprint1.GetSize(), 1);

    EXPECT_EQ(footprint1[0].GetData().GetName(), OTR_NAME_OF(UnsafeList));
    EXPECT_EQ(footprint1[0].GetData().GetPointer(), list.GetData());
    EXPECT_EQ(footprint1[0].Size,
              OTR_ALIGNED_OFFSET(sizeof(int) * list.GetCapacity(), OTR_PLATFORM_MEMORY_ALIGNMENT) +
              Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(footprint1[0].Offset, Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(footprint1[0].Padding, 0);
    EXPECT_EQ(footprint1[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

    list.Add(6);

    auto footprint2 = list.GetMemoryFootprint(OTR_NAME_OF(UnsafeList));
    EXPECT_EQ(footprint2.GetSize(), 1);

    EXPECT_EQ(footprint2[0].GetData().GetName(), OTR_NAME_OF(UnsafeList));
    EXPECT_EQ(footprint2[0].GetData().GetPointer(), list.GetData());
    EXPECT_NE(footprint2[0].GetData().GetPointer(), footprint1[0].GetData().GetPointer())
                    << "Pointer should have changed because of capacity increase (reallocation)";
    EXPECT_EQ(footprint2[0].Size,
              OTR_ALIGNED_OFFSET(sizeof(int) * list.GetCapacity(), OTR_PLATFORM_MEMORY_ALIGNMENT) +
              Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_NE(footprint2[0].Offset, footprint1[0].Offset)
                    << "Offset should have changed because of capacity increase (reallocation)";
    EXPECT_EQ(footprint2[0].Padding, 0);
    EXPECT_EQ(footprint2[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

    list.ClearDestructive();

    auto footprint3 = list.GetMemoryFootprint(OTR_NAME_OF(UnsafeList));
    EXPECT_EQ(footprint3.GetSize(), 1);

    EXPECT_EQ(footprint3[0].GetData().GetName(), OTR_NAME_OF(UnsafeList));
    EXPECT_EQ(footprint3[0].GetData().GetPointer(), nullptr);
    EXPECT_EQ(footprint3[0].Size, 0);
    EXPECT_EQ(footprint3[0].Offset, 0);
    EXPECT_EQ(footprint3[0].Padding, 0);
    EXPECT_EQ(footprint3[0].Alignment, 0);
}