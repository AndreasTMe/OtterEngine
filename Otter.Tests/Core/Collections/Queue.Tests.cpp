#include <gtest/gtest.h>

#include "Core/Collections/Queue.h"

template<typename T>
using Queue = Otter::Queue<T>;

class QueueFixture : public ::testing::Test
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

TEST_F(QueueFixture, Initialisation_Default)
{
    Queue<int> queue;

    EXPECT_EQ(queue.GetCapacity(), 0);
    EXPECT_EQ(queue.GetCount(), 0);

    EXPECT_TRUE(queue.IsEmpty());
}

TEST_F(QueueFixture, Initialisation_FromInitialisationList)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };

    EXPECT_EQ(queue.GetCapacity(), 5);
    EXPECT_EQ(queue.GetCount(), 5);

    EXPECT_FALSE(queue.IsEmpty());
}

TEST_F(QueueFixture, Initialisation_Copy)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };
    Queue<int> copy  = queue;

    EXPECT_EQ(copy.GetCapacity(), queue.GetCapacity());
    EXPECT_EQ(copy.GetCount(), queue.GetCount());

    EXPECT_FALSE(copy.IsEmpty());
}

TEST_F(QueueFixture, Initialisation_Move)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };
    Queue<int> move  = std::move(queue);

    EXPECT_EQ(move.GetCapacity(), 5);
    EXPECT_EQ(move.GetCount(), 5);

    EXPECT_FALSE(move.IsEmpty());
}

TEST_F(QueueFixture, Assignment_Copy)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };
    Queue<int> copy;

    copy = queue;

    EXPECT_EQ(copy.GetCapacity(), queue.GetCapacity());
    EXPECT_EQ(copy.GetCount(), queue.GetCount());

    EXPECT_FALSE(copy.IsEmpty());
}

TEST_F(QueueFixture, Assignment_Move)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };
    Queue<int> move;

    move = std::move(queue);

    EXPECT_EQ(move.GetCapacity(), 5);
    EXPECT_EQ(move.GetCount(), 5);

    EXPECT_FALSE(move.IsEmpty());
}

TEST_F(QueueFixture, TryEnqueue)
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

TEST_F(QueueFixture, TryDequeue)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };

    int value;
    EXPECT_TRUE(queue.TryDequeue(value));
    EXPECT_EQ(value, 1);
    EXPECT_EQ(queue.GetCount(), 4);

    EXPECT_TRUE(queue.TryDequeue());
    EXPECT_EQ(queue.GetCount(), 3);

    EXPECT_TRUE(queue.TryDequeue(value));
    EXPECT_EQ(value, 3);
    EXPECT_EQ(queue.GetCount(), 2);

    EXPECT_TRUE(queue.TryDequeue(value));
    EXPECT_EQ(value, 4);
    EXPECT_EQ(queue.GetCount(), 1);

    EXPECT_TRUE(queue.TryDequeue(value));
    EXPECT_EQ(value, 5);
    EXPECT_EQ(queue.GetCount(), 0);

    EXPECT_FALSE(queue.TryDequeue(value));
    EXPECT_EQ(value, 5);
    EXPECT_EQ(queue.GetCount(), 0);

    EXPECT_FALSE(queue.TryDequeue());
    EXPECT_EQ(queue.GetCount(), 0);
}

TEST_F(QueueFixture, TryPeek)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };

    int value;
    EXPECT_TRUE(queue.TryPeek(value));
    EXPECT_EQ(value, 1);
    EXPECT_EQ(queue.GetCount(), 5);

    EXPECT_TRUE(queue.TryDequeue());
    EXPECT_EQ(queue.GetCount(), 4);
    EXPECT_TRUE(queue.TryPeek(value));
    EXPECT_EQ(value, 2);

    EXPECT_TRUE(queue.TryDequeue());
    EXPECT_TRUE(queue.TryDequeue());
    EXPECT_TRUE(queue.TryDequeue());
    EXPECT_TRUE(queue.TryDequeue());
    EXPECT_FALSE(queue.TryDequeue());

    EXPECT_FALSE(queue.TryPeek(value));
}

TEST_F(QueueFixture, Reserve)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };
    EXPECT_EQ(queue.GetCapacity(), 5);
    EXPECT_EQ(queue.GetCount(), 5);

    queue.Reserve(10);

    EXPECT_EQ(queue.GetCapacity(), 10);
    EXPECT_EQ(queue.GetCount(), 0);
}

TEST_F(QueueFixture, Expand)
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
    EXPECT_TRUE(queueWithItems2.TryPeek(value));
    EXPECT_EQ(value, 3);
}

TEST_F(QueueFixture, Shrink)
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
    EXPECT_TRUE(queueWithItems2.TryPeek(value));
    EXPECT_EQ(value, 3);
}

TEST_F(QueueFixture, Contains)
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

TEST_F(QueueFixture, Clear)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };
    queue.Clear();

    EXPECT_TRUE(queue.IsCreated());
    EXPECT_TRUE(queue.IsEmpty());
    EXPECT_EQ(queue.GetCount(), 0);

    int value = 0;
    EXPECT_FALSE(queue.TryPeek(value));
    EXPECT_FALSE(queue.TryDequeue(value));
}

TEST_F(QueueFixture, ClearDestructive)
{
    Queue<int> queue = { 1, 2, 3, 4, 5 };
    queue.ClearDestructive();

    EXPECT_FALSE(queue.IsCreated());
    EXPECT_TRUE(queue.IsEmpty());
    EXPECT_EQ(queue.GetCount(), 0);

    int value = 0;
    EXPECT_FALSE(queue.TryPeek(value));
    EXPECT_FALSE(queue.TryDequeue(value));
}