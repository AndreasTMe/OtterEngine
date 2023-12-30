#include <gtest/gtest.h>

#include "Core/Collections/Queue.h"

template<typename T>
using Queue = Otter::Queue<T>;

class Queue_Fixture : public ::testing::Test
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

TEST_F(Queue_Fixture, Initialisation_Default)
{
    Queue<int> queue;

    EXPECT_EQ(queue.GetCapacity(), 0);
    EXPECT_EQ(queue.GetCount(), 0);

    EXPECT_TRUE(queue.IsEmpty());
}

TEST_F(Queue_Fixture, Initialisation_FromInitialisationList)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };

    EXPECT_EQ(queue.GetCapacity(), 5);
    EXPECT_EQ(queue.GetCount(), 5);

    EXPECT_FALSE(queue.IsEmpty());
}

TEST_F(Queue_Fixture, Initialisation_Copy)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };
    Queue<int> copy  = queue;

    EXPECT_EQ(copy.GetCapacity(), queue.GetCapacity());
    EXPECT_EQ(copy.GetCount(), queue.GetCount());

    EXPECT_FALSE(copy.IsEmpty());
}

TEST_F(Queue_Fixture, Initialisation_Move)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };
    Queue<int> move  = std::move(queue);

    EXPECT_EQ(move.GetCapacity(), 5);
    EXPECT_EQ(move.GetCount(), 5);

    EXPECT_FALSE(move.IsEmpty());
}

TEST_F(Queue_Fixture, Assignment_Copy)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };
    Queue<int> copy;

    copy = queue;

    EXPECT_EQ(copy.GetCapacity(), queue.GetCapacity());
    EXPECT_EQ(copy.GetCount(), queue.GetCount());

    EXPECT_FALSE(copy.IsEmpty());
}

TEST_F(Queue_Fixture, Assignment_Move)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };
    Queue<int> move;

    move = std::move(queue);

    EXPECT_EQ(move.GetCapacity(), 5);
    EXPECT_EQ(move.GetCount(), 5);

    EXPECT_FALSE(move.IsEmpty());
}

TEST_F(Queue_Fixture, TryEnqueue)
{
    Queue<int> queue       = { 1, 2, 3, 4, 5 };
    const auto oldCapacity = queue.GetCapacity();

    int value = 6;
    EXPECT_TRUE(queue.TryEnqueue(value));
    EXPECT_EQ(queue.GetCount(), 6);
    EXPECT_EQ(queue.GetCapacity(), static_cast<UInt64>(oldCapacity * 1.5));

    EXPECT_TRUE(queue.TryEnqueue(7));
    EXPECT_EQ(queue.GetCount(), 7);
    EXPECT_EQ(queue.GetCapacity(), static_cast<UInt64>(oldCapacity * 1.5));
}

TEST_F(Queue_Fixture, TryDequeue)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };

    int value;
    EXPECT_TRUE(queue.TryDequeue(&value));
    EXPECT_EQ(value, 1);
    EXPECT_EQ(queue.GetCount(), 4);

    EXPECT_TRUE(queue.TryDequeue());
    EXPECT_EQ(queue.GetCount(), 3);

    EXPECT_TRUE(queue.TryDequeue(&value));
    EXPECT_EQ(value, 3);
    EXPECT_EQ(queue.GetCount(), 2);

    EXPECT_TRUE(queue.TryDequeue(&value));
    EXPECT_EQ(value, 4);
    EXPECT_EQ(queue.GetCount(), 1);

    EXPECT_TRUE(queue.TryDequeue(&value));
    EXPECT_EQ(value, 5);
    EXPECT_EQ(queue.GetCount(), 0);

    EXPECT_FALSE(queue.TryDequeue(&value));
    EXPECT_EQ(value, 5);
    EXPECT_EQ(queue.GetCount(), 0);

    EXPECT_FALSE(queue.TryDequeue());
    EXPECT_EQ(queue.GetCount(), 0);
}

TEST_F(Queue_Fixture, TryPeek)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };

    int value;
    EXPECT_TRUE(queue.TryPeek(&value));
    EXPECT_EQ(value, 1);
    EXPECT_EQ(queue.GetCount(), 5);

    EXPECT_TRUE(queue.TryDequeue());
    EXPECT_EQ(queue.GetCount(), 4);
    EXPECT_TRUE(queue.TryPeek(&value));
    EXPECT_EQ(value, 2);

    EXPECT_TRUE(queue.TryDequeue());
    EXPECT_TRUE(queue.TryDequeue());
    EXPECT_TRUE(queue.TryDequeue());
    EXPECT_TRUE(queue.TryDequeue());
    EXPECT_FALSE(queue.TryDequeue());

    EXPECT_FALSE(queue.TryPeek(&value));
}

TEST_F(Queue_Fixture, Reserve)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };
    EXPECT_EQ(queue.GetCapacity(), 5);
    EXPECT_EQ(queue.GetCount(), 5);

    queue.Reserve(10);

    EXPECT_EQ(queue.GetCapacity(), 10);
    EXPECT_EQ(queue.GetCount(), 0);
}

TEST_F(Queue_Fixture, Expand)
{
    Queue<int> emptyQueue;
    EXPECT_EQ(emptyQueue.GetCapacity(), 0);
    EXPECT_EQ(emptyQueue.GetCount(), 0);

    emptyQueue.Expand(10);

    EXPECT_EQ(emptyQueue.GetCapacity(), 10);
    EXPECT_EQ(emptyQueue.GetCount(), 0);

    Queue<int> queueWithItems1 = { 1, 2, 3, 4, 5 };
    EXPECT_EQ(queueWithItems1.GetCapacity(), 5);
    EXPECT_EQ(queueWithItems1.GetCount(), 5);

    queueWithItems1.Expand(10);

    EXPECT_EQ(queueWithItems1.GetCapacity(), 15);
    EXPECT_EQ(queueWithItems1.GetCount(), 5);

    Queue<int> queueWithItems2 = { 1, 2, 3, 4, 5 };
    EXPECT_EQ(queueWithItems2.GetCapacity(), 5);
    EXPECT_EQ(queueWithItems2.GetCount(), 5);

    queueWithItems2.TryDequeue();
    queueWithItems2.TryDequeue();
    queueWithItems2.TryEnqueue(6);
    queueWithItems2.TryEnqueue(7); // m_EndIndex < m_StartIndex

    queueWithItems2.Expand(10);

    EXPECT_EQ(queueWithItems2.GetCapacity(), 15);
    EXPECT_EQ(queueWithItems2.GetCount(), 5);

    int value;
    EXPECT_TRUE(queueWithItems2.TryPeek(&value));
    EXPECT_EQ(value, 3);
}

TEST_F(Queue_Fixture, Shrink)
{
    Queue<int> emptyQueue;
    EXPECT_EQ(emptyQueue.GetCapacity(), 0);
    EXPECT_EQ(emptyQueue.GetCount(), 0);

    emptyQueue.Shrink(10);

    EXPECT_EQ(emptyQueue.GetCapacity(), 0);
    EXPECT_EQ(emptyQueue.GetCount(), 0);

    Queue<int> queueWithItems1 = { 1, 2, 3, 4, 5 };
    EXPECT_EQ(queueWithItems1.GetCapacity(), 5);
    EXPECT_EQ(queueWithItems1.GetCount(), 5);

    queueWithItems1.Shrink(3);

    EXPECT_EQ(queueWithItems1.GetCapacity(), 5);
    EXPECT_EQ(queueWithItems1.GetCount(), 5);

    queueWithItems1.Shrink(3, true);

    EXPECT_EQ(queueWithItems1.GetCapacity(), 2);
    EXPECT_EQ(queueWithItems1.GetCount(), 2);

    Queue<int> queueWithItems2 = { 1, 2, 3, 4, 5 };
    EXPECT_EQ(queueWithItems2.GetCapacity(), 5);
    EXPECT_EQ(queueWithItems2.GetCount(), 5);

    queueWithItems2.TryDequeue();
    queueWithItems2.TryDequeue();
    queueWithItems2.TryEnqueue(6);
    queueWithItems2.TryEnqueue(7); // m_EndIndex < m_StartIndex

    queueWithItems2.Shrink(3, true);

    EXPECT_EQ(queueWithItems2.GetCapacity(), 2);
    EXPECT_EQ(queueWithItems2.GetCount(), 2);

    int value;
    EXPECT_TRUE(queueWithItems2.TryPeek(&value));
    EXPECT_EQ(value, 3);
}

TEST_F(Queue_Fixture, Contains)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };

    int value = 3;
    EXPECT_TRUE(queue.Contains(1));
    EXPECT_TRUE(queue.Contains(2));
    EXPECT_TRUE(queue.Contains(value));
    EXPECT_TRUE(queue.Contains(4));
    EXPECT_TRUE(queue.Contains(5));

    queue.TryDequeue();
    EXPECT_FALSE(queue.Contains(1));
    queue.TryDequeue();
    EXPECT_FALSE(queue.Contains(2));
    queue.TryEnqueue(6);
    queue.TryEnqueue(7); // m_EndIndex < m_StartIndex

    value = 3;
    EXPECT_TRUE(queue.Contains(value));
    EXPECT_TRUE(queue.Contains(4));

    value = 6;
    EXPECT_TRUE(queue.Contains(value));
    EXPECT_TRUE(queue.Contains(7));
}

TEST_F(Queue_Fixture, Clear)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };
    queue.Clear();

    EXPECT_TRUE(queue.IsCreated());
    EXPECT_TRUE(queue.IsEmpty());
    EXPECT_EQ(queue.GetCount(), 0);

    int value = 0;
    EXPECT_FALSE(queue.TryPeek(&value));
    EXPECT_FALSE(queue.TryDequeue(&value));
}

TEST_F(Queue_Fixture, ClearDestructive)
{
    Queue<int> queue1 = { 1, 2, 3, 4, 5 };
    queue1.ClearDestructive();

    EXPECT_FALSE(queue1.IsCreated());
    EXPECT_TRUE(queue1.IsEmpty());
    EXPECT_EQ(queue1.GetCount(), 0);

    int value = 0;
    EXPECT_FALSE(queue1.TryPeek(&value));
    EXPECT_FALSE(queue1.TryDequeue(&value));

    Queue<Queue<int>> queue2 = {{ 1, 2, 3, 4, 5 },
                                { 6, 7, 8, 9, 10 }};
    queue2.ClearDestructive();

    EXPECT_FALSE(queue2.IsCreated());
    EXPECT_TRUE(queue2.IsEmpty());
    EXPECT_EQ(queue2.GetCount(), 0);

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
}

TEST_F(Queue_Fixture, GetMemoryFootprint)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };

    auto footprint1 = queue.GetMemoryFootprint(OTR_NAME_OF(Queue<int>));
    EXPECT_EQ(footprint1.GetSize(), 1);

    EXPECT_EQ(footprint1[0].GetData().GetName(), OTR_NAME_OF(Queue<int>));
    EXPECT_NE(footprint1[0].GetData().GetPointer(), nullptr);
    EXPECT_EQ(footprint1[0].Size, OTR_ALLOCATED_MEMORY(int, queue.GetCapacity()));
    EXPECT_EQ(footprint1[0].Offset, Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(footprint1[0].Padding, 0);
    EXPECT_EQ(footprint1[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

    queue.TryEnqueue(6);

    auto footprint2 = queue.GetMemoryFootprint(OTR_NAME_OF(Queue<int>));
    EXPECT_EQ(footprint2.GetSize(), 1);

    EXPECT_EQ(footprint2[0].GetData().GetName(), OTR_NAME_OF(Queue<int>));
    EXPECT_NE(footprint1[0].GetData().GetPointer(), nullptr);
    EXPECT_NE(footprint2[0].GetData().GetPointer(), footprint1[0].GetData().GetPointer())
                    << "Pointer should have changed because of capacity increase (reallocation)";
    EXPECT_EQ(footprint2[0].Size, OTR_ALLOCATED_MEMORY(int, queue.GetCapacity()));
    EXPECT_NE(footprint2[0].Offset, footprint1[0].Offset)
                    << "Offset should have changed because of capacity increase (reallocation)";
    EXPECT_EQ(footprint2[0].Padding, 0);
    EXPECT_EQ(footprint2[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

    queue.ClearDestructive();

    auto footprint3 = queue.GetMemoryFootprint(OTR_NAME_OF(Queue<int>));
    EXPECT_EQ(footprint3.GetSize(), 1);

    EXPECT_EQ(footprint3[0].GetData().GetName(), OTR_NAME_OF(Queue<int>));
    EXPECT_EQ(footprint3[0].GetData().GetPointer(), nullptr);
    EXPECT_EQ(footprint3[0].Size, 0);
    EXPECT_EQ(footprint3[0].Offset, 0);
    EXPECT_EQ(footprint3[0].Padding, 0);
    EXPECT_EQ(footprint3[0].Alignment, 0);
}