#include <gtest/gtest.h>

#include "Core/Collections/Deque.h"

template<typename T>
using Deque = Otter::Deque<T>;

class Deque_Fixture : public ::testing::Test
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

TEST_F(Deque_Fixture, Initialisation_Default)
{
    Deque<int> deque;

    EXPECT_EQ(deque.GetData(), nullptr);
    EXPECT_EQ(deque.GetCapacity(), 0);
    EXPECT_EQ(deque.GetCount(), 0);

    EXPECT_TRUE(deque.IsEmpty());
}

TEST_F(Deque_Fixture, Initialisation_FromInitialisationList)
{
    Deque<int> deque = { 1, 2, 3, 4, 5 };

    EXPECT_NE(deque.GetData(), nullptr);
    EXPECT_EQ(deque.GetCapacity(), 5);
    EXPECT_EQ(deque.GetCount(), 5);
}

TEST_F(Deque_Fixture, Initialisation_Copy)
{
    Deque<int> deque = { 1, 2, 3, 4, 5 };
    Deque<int> copy  = deque;

    EXPECT_NE(copy.GetData(), deque.GetData());
    EXPECT_EQ(copy.GetCapacity(), deque.GetCapacity());
    EXPECT_EQ(copy.GetCount(), deque.GetCount());
}

TEST_F(Deque_Fixture, Initialisation_Move)
{
    Deque<int> deque = { 1, 2, 3, 4, 5 };
    Deque<int> move  = std::move(deque);

    EXPECT_NE(move.GetData(), nullptr);
    EXPECT_EQ(move.GetCapacity(), 5);
    EXPECT_EQ(move.GetCount(), 5);
}

TEST_F(Deque_Fixture, Equality)
{
    Deque<int> deque1 = { 1, 2, 3, 4, 5 };
    Deque<int> deque2 = { 1, 2, 3, 4, 5 };
    Deque<int> deque3 = { 1, 2, 3, 4, 6 };

    EXPECT_TRUE(deque1 == deque2);
    EXPECT_FALSE(deque1 == deque3);
    EXPECT_TRUE(deque1 != deque3);
    EXPECT_FALSE(deque1 != deque2);
}

TEST_F(Deque_Fixture, PushFront)
{
    Deque<int> deque;
    deque.PushFront(1);

    EXPECT_NE(deque.GetData(), nullptr);
    EXPECT_EQ(deque.GetCapacity(), 2);
    EXPECT_EQ(deque.GetCount(), 1);
    auto oldCapacity = deque.GetCapacity();

    int value = 2;
    deque.PushFront(value);
    deque.PushFront(3);

    EXPECT_EQ(deque.GetCapacity(), oldCapacity * 1.5);
    EXPECT_EQ(deque.GetCount(), 3);

    int out = 0;
    EXPECT_TRUE(deque.TryPeekFront(&out));
    EXPECT_EQ(out, 3);
    EXPECT_TRUE(deque.TryPeekBack(&out));
    EXPECT_EQ(out, 1);
}

TEST_F(Deque_Fixture, PushBack)
{
    Deque<int> deque;
    deque.PushBack(1);

    EXPECT_NE(deque.GetData(), nullptr);
    EXPECT_EQ(deque.GetCapacity(), 2);
    EXPECT_EQ(deque.GetCount(), 1);
    auto oldCapacity = deque.GetCapacity();

    int value = 2;
    deque.PushBack(value);
    deque.PushBack(3);

    EXPECT_EQ(deque.GetCapacity(), oldCapacity * 1.5);
    EXPECT_EQ(deque.GetCount(), 3);

    int out = 0;
    EXPECT_TRUE(deque.TryPeekFront(&out));
    EXPECT_EQ(out, 1);
    EXPECT_TRUE(deque.TryPeekBack(&out));
    EXPECT_EQ(out, 3);
}

TEST_F(Deque_Fixture, TryPopFront)
{
    Deque<int> deque = { 1, 2, 3, 4, 5 };

    int out = 0;
    EXPECT_TRUE(deque.TryPopFront());
    EXPECT_TRUE(deque.TryPeekFront(&out));
    EXPECT_EQ(out, 2);
    EXPECT_TRUE(deque.TryPeekBack(&out));
    EXPECT_EQ(out, 5);

    EXPECT_TRUE(deque.TryPopFront(&out));
    EXPECT_EQ(out, 2);
    EXPECT_TRUE(deque.TryPopFront());
    EXPECT_TRUE(deque.TryPopFront());
    EXPECT_TRUE(deque.TryPopFront());
    EXPECT_FALSE(deque.TryPopFront());
}

TEST_F(Deque_Fixture, TryPopBack)
{
    Deque<int> deque = { 1, 2, 3, 4, 5 };

    int out = 0;
    EXPECT_TRUE(deque.TryPopBack());
    EXPECT_TRUE(deque.TryPeekFront(&out));
    EXPECT_EQ(out, 1);
    EXPECT_TRUE(deque.TryPeekBack(&out));
    EXPECT_EQ(out, 4);

    EXPECT_TRUE(deque.TryPopBack(&out));
    EXPECT_EQ(out, 4);
    EXPECT_TRUE(deque.TryPopBack());
    EXPECT_TRUE(deque.TryPopBack());
    EXPECT_TRUE(deque.TryPopBack());
    EXPECT_FALSE(deque.TryPopBack());
}

TEST_F(Deque_Fixture, TryRemove)
{
    int        value = 2;
    Deque<int> deque = { 1, value, 3, 4, 5 };

    EXPECT_TRUE(deque.TryRemove(3));
    EXPECT_EQ(deque.GetCount(), 4);

    EXPECT_TRUE(deque.TryRemove(1));
    EXPECT_EQ(deque.GetCount(), 3);

    EXPECT_TRUE(deque.TryRemove(5));
    EXPECT_EQ(deque.GetCount(), 2);

    EXPECT_TRUE(deque.TryRemove(value));
    EXPECT_EQ(deque.GetCount(), 1);

    EXPECT_TRUE(deque.TryRemove(4));
    EXPECT_EQ(deque.GetCount(), 0);

    EXPECT_FALSE(deque.TryRemove(4));
    EXPECT_EQ(deque.GetCount(), 0);
}

TEST_F(Deque_Fixture, Reserve)
{
    Deque<int> deque;
    deque.Reserve(5);
    deque.PushFront(1);

    EXPECT_NE(deque.GetData(), nullptr);
    EXPECT_EQ(deque.GetCapacity(), 5);
    EXPECT_EQ(deque.GetCount(), 1);

    deque.Reserve(10);
    deque.PushFront(1);

    EXPECT_EQ(deque.GetCapacity(), 10);
    EXPECT_EQ(deque.GetCount(), 2);

    deque.Reserve(5);

    EXPECT_EQ(deque.GetCapacity(), 10);
    EXPECT_EQ(deque.GetCount(), 2);
}

TEST_F(Deque_Fixture, Expand)
{
    Deque<int> deque;
    deque.Expand(5);
    deque.PushFront(1);

    EXPECT_NE(deque.GetData(), nullptr);
    EXPECT_EQ(deque.GetCapacity(), 5);
    EXPECT_EQ(deque.GetCount(), 1);

    deque.Expand(10);
    deque.PushFront(1);

    EXPECT_EQ(deque.GetCapacity(), 15);
    EXPECT_EQ(deque.GetCount(), 2);

    deque.Expand(5);

    EXPECT_EQ(deque.GetCapacity(), 20);
    EXPECT_EQ(deque.GetCount(), 2);
}

TEST_F(Deque_Fixture, Shrink)
{
    Deque<int> deque = { 1, 2, 3, 4, 5 };
    deque.Shrink(3);

    EXPECT_NE(deque.GetData(), nullptr);
    EXPECT_EQ(deque.GetCapacity(), 5);
    EXPECT_EQ(deque.GetCount(), 5);

    deque.Shrink(3, true);

    EXPECT_NE(deque.GetData(), nullptr);
    EXPECT_EQ(deque.GetCapacity(), 2);
    EXPECT_EQ(deque.GetCount(), 2);
}

TEST_F(Deque_Fixture, Clear)
{
    Deque<int> deque = { 1, 2, 3, 4, 5 };
    deque.Clear();

    EXPECT_NE(deque.GetData(), nullptr);
    EXPECT_EQ(deque.GetCapacity(), 5);
    EXPECT_EQ(deque.GetCount(), 0);
}

TEST_F(Deque_Fixture, ClearDestructive)
{
    Deque<int> deque1 = { 1, 2, 3, 4, 5 };
    deque1.ClearDestructive();

    EXPECT_EQ(deque1.GetData(), nullptr);
    EXPECT_EQ(deque1.GetCapacity(), 0);
    EXPECT_EQ(deque1.GetCount(), 0);

    Deque<Deque<int>> deque2 = {{ 1, 2, 3, 4, 5 },
                                { 6, 7, 8, 9, 10 }};
    deque2.ClearDestructive();

    EXPECT_FALSE(deque2.IsCreated());
    EXPECT_TRUE(deque2.IsEmpty());
    EXPECT_EQ(deque2.GetCount(), 0);
}

TEST_F(Deque_Fixture, GetMemoryFootprint)
{
    Deque<int> deque = { 1, 2, 3, 4, 5 };

    auto footprint1 = deque.GetMemoryFootprint(OTR_NAME_OF(Deque<int>));
    EXPECT_EQ(footprint1.GetSize(), 1);

    EXPECT_EQ(footprint1[0].GetData().GetName(), OTR_NAME_OF(Deque<int>));
    EXPECT_NE(footprint1[0].GetData().GetPointer(), nullptr);
    EXPECT_EQ(footprint1[0].Size, OTR_ALLOCATED_MEMORY(int, deque.GetCapacity()));
    EXPECT_EQ(footprint1[0].Offset, Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(footprint1[0].Padding, 0);
    EXPECT_EQ(footprint1[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

    deque.PushFront(6);
    deque.PushBack(7);

    auto footprint2 = deque.GetMemoryFootprint(OTR_NAME_OF(Deque<int>));
    EXPECT_EQ(footprint2.GetSize(), 1);

    EXPECT_EQ(footprint2[0].GetData().GetName(), OTR_NAME_OF(Deque<int>));
    EXPECT_NE(footprint2[0].GetData().GetPointer(), nullptr);
    EXPECT_NE(footprint2[0].GetData().GetPointer(), footprint1[0].GetData().GetPointer())
                    << "Pointer should have changed because of capacity increase (reallocation)";
    EXPECT_EQ(footprint2[0].Size, OTR_ALLOCATED_MEMORY(int, deque.GetCapacity()));
    EXPECT_NE(footprint2[0].Offset, footprint1[0].Offset)
                    << "Offset should have changed because of capacity increase (reallocation)";
    EXPECT_EQ(footprint2[0].Padding, 0);
    EXPECT_EQ(footprint2[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

    deque.ClearDestructive();

    auto footprint3 = deque.GetMemoryFootprint(OTR_NAME_OF(Deque<int>));
    EXPECT_EQ(footprint3.GetSize(), 1);

    EXPECT_EQ(footprint3[0].GetData().GetName(), OTR_NAME_OF(Deque<int>));
    EXPECT_EQ(footprint3[0].GetData().GetPointer(), nullptr);
    EXPECT_EQ(footprint3[0].Size, 0);
    EXPECT_EQ(footprint3[0].Offset, 0);
    EXPECT_EQ(footprint3[0].Padding, 0);
    EXPECT_EQ(footprint3[0].Alignment, 0);
}

TEST_F(Deque_Fixture, Iterator)
{
    Deque<int> deque = { 1, 2, 3, 4, 5 };

    UInt64 i = 0;
    for (auto& element: deque)
        EXPECT_EQ(element, ++i);

    for (auto it = deque.rbegin(); it != deque.rend(); --it)
        EXPECT_EQ(*it, i--);

    for (auto it = deque.cbegin(); it != deque.cend(); ++it)
        EXPECT_EQ(*it, ++i);

    for (auto it = deque.crbegin(); it != deque.crend(); --it)
        EXPECT_EQ(*it, i--);
}