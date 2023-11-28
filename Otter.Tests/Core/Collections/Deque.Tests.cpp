#include <gtest/gtest.h>

#include "Core/Collections/Deque.h"

template<typename T>
using Deque = Otter::Deque<T>;

class DequeFixture : public ::testing::Test
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

TEST_F(DequeFixture, Initialisation_Default)
{
    Deque<int> deque;

    EXPECT_EQ(deque.GetData(), nullptr);
    EXPECT_EQ(deque.GetCapacity(), 0);
    EXPECT_EQ(deque.GetCount(), 0);

    EXPECT_TRUE(deque.IsEmpty());
}

TEST_F(DequeFixture, Initialisation_FromInitialisationList)
{
    Deque<int> deque = { 1, 2, 3, 4, 5 };

    EXPECT_NE(deque.GetData(), nullptr);
    EXPECT_EQ(deque.GetCapacity(), 5);
    EXPECT_EQ(deque.GetCount(), 5);
}

TEST_F(DequeFixture, Initialisation_Copy)
{
    Deque<int> deque = { 1, 2, 3, 4, 5 };
    Deque<int> copy  = deque;

    EXPECT_EQ(copy.GetData(), deque.GetData());
    EXPECT_EQ(copy.GetCapacity(), deque.GetCapacity());
    EXPECT_EQ(copy.GetCount(), deque.GetCount());
}

TEST_F(DequeFixture, Initialisation_Move)
{
    Deque<int> deque = { 1, 2, 3, 4, 5 };
    Deque<int> move  = std::move(deque);

    EXPECT_NE(move.GetData(), nullptr);
    EXPECT_EQ(move.GetCapacity(), 5);
    EXPECT_EQ(move.GetCount(), 5);
}

TEST_F(DequeFixture, PushFront)
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
    EXPECT_TRUE(deque.TryPeekFront(out));
    EXPECT_EQ(out, 3);
    EXPECT_TRUE(deque.TryPeekBack(out));
    EXPECT_EQ(out, 1);
}

TEST_F(DequeFixture, PushBack)
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
    EXPECT_TRUE(deque.TryPeekFront(out));
    EXPECT_EQ(out, 1);
    EXPECT_TRUE(deque.TryPeekBack(out));
    EXPECT_EQ(out, 3);
}

TEST_F(DequeFixture, TryPopFront)
{
    Deque<int> deque = { 1, 2, 3, 4, 5 };

    int out = 0;
    EXPECT_TRUE(deque.TryPopFront());
    EXPECT_TRUE(deque.TryPeekFront(out));
    EXPECT_EQ(out, 2);
    EXPECT_TRUE(deque.TryPeekBack(out));
    EXPECT_EQ(out, 5);

    EXPECT_TRUE(deque.TryPopFront());
    EXPECT_TRUE(deque.TryPopFront());
    EXPECT_TRUE(deque.TryPopFront());
    EXPECT_TRUE(deque.TryPopFront());
    EXPECT_FALSE(deque.TryPopFront());
}

TEST_F(DequeFixture, TryPopBack)
{
    Deque<int> deque = { 1, 2, 3, 4, 5 };

    int out = 0;
    EXPECT_TRUE(deque.TryPopBack());
    EXPECT_TRUE(deque.TryPeekFront(out));
    EXPECT_EQ(out, 1);
    EXPECT_TRUE(deque.TryPeekBack(out));
    EXPECT_EQ(out, 4);

    EXPECT_TRUE(deque.TryPopBack());
    EXPECT_TRUE(deque.TryPopBack());
    EXPECT_TRUE(deque.TryPopBack());
    EXPECT_TRUE(deque.TryPopBack());
    EXPECT_FALSE(deque.TryPopBack());
}

TEST_F(DequeFixture, TryRemove)
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

TEST_F(DequeFixture, Reserve)
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

TEST_F(DequeFixture, Expand)
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

TEST_F(DequeFixture, Clear)
{
    Deque<int> deque = { 1, 2, 3, 4, 5 };
    deque.Clear();

    EXPECT_NE(deque.GetData(), nullptr);
    EXPECT_EQ(deque.GetCapacity(), 5);
    EXPECT_EQ(deque.GetCount(), 0);
}

TEST_F(DequeFixture, ClearDestructive)
{
    Deque<int> deque = { 1, 2, 3, 4, 5 };
    deque.ClearDestructive();

    EXPECT_EQ(deque.GetData(), nullptr);
    EXPECT_EQ(deque.GetCapacity(), 0);
    EXPECT_EQ(deque.GetCount(), 0);
}