#include <gtest/gtest.h>

#include "Core/Collections/Stack.h"

template<typename T>
using Stack = Otter::Stack<T>;

class Stack_Fixture : public ::testing::Test
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

TEST_F(Stack_Fixture, Initialisation_Default)
{
    Stack<int> stack;

    EXPECT_EQ(stack.GetData(), nullptr);
    EXPECT_EQ(stack.GetCapacity(), 0);
    EXPECT_EQ(stack.GetCount(), 0);

    EXPECT_TRUE(stack.IsEmpty());
}

TEST_F(Stack_Fixture, Initialisation_FromInitialisationList)
{
    Stack<int> stack = { 1, 2, 3, 4, 5 };

    EXPECT_NE(stack.GetData(), nullptr);
    EXPECT_EQ(stack.GetCapacity(), 5);
    EXPECT_EQ(stack.GetCount(), 5);
}

TEST_F(Stack_Fixture, Initialisation_Copy)
{
    Stack<int> stack = { 1, 2, 3, 4, 5 };
    Stack<int> copy  = stack;

    EXPECT_EQ(copy.GetData(), stack.GetData());
    EXPECT_EQ(copy.GetCapacity(), stack.GetCapacity());
    EXPECT_EQ(copy.GetCount(), stack.GetCount());
}

TEST_F(Stack_Fixture, Initialisation_Move)
{
    Stack<int> stack = { 1, 2, 3, 4, 5 };
    Stack<int> move  = std::move(stack);

    EXPECT_NE(move.GetData(), nullptr);
    EXPECT_EQ(move.GetCapacity(), 5);
    EXPECT_EQ(move.GetCount(), 5);

    EXPECT_EQ(stack.GetData(), nullptr);
}

TEST_F(Stack_Fixture, Assignment_Copy)
{
    Stack<int> stack = { 1, 2, 3, 4, 5 };
    Stack<int> copy;
    copy = stack;

    EXPECT_EQ(copy.GetData(), stack.GetData());
    EXPECT_EQ(copy.GetCapacity(), stack.GetCapacity());
    EXPECT_EQ(copy.GetCount(), stack.GetCount());
}

TEST_F(Stack_Fixture, Assignment_Move)
{
    Stack<int> stack = { 1, 2, 3, 4, 5 };
    Stack<int> move;
    move = std::move(stack);

    EXPECT_NE(move.GetData(), nullptr);
    EXPECT_EQ(move.GetCapacity(), 5);
    EXPECT_EQ(move.GetCount(), 5);

    EXPECT_EQ(stack.GetData(), nullptr);
}

TEST_F(Stack_Fixture, Push)
{
    Stack<int> stack;
    stack.Push(1);

    EXPECT_NE(stack.GetData(), nullptr);
    EXPECT_EQ(stack.GetCapacity(), 2);
    EXPECT_EQ(stack.GetCount(), 1);
    auto oldCapacity = stack.GetCapacity();

    int value = 2;
    stack.Push(value);
    stack.Push(3);

    EXPECT_EQ(stack.GetCapacity(), oldCapacity * 1.5);
    EXPECT_EQ(stack.GetCount(), 3);
}

TEST_F(Stack_Fixture, TryPop)
{
    Stack<int> stack = { 1, 2, 3, 4, 5 };

    int value;
    EXPECT_TRUE(stack.TryPop(value));
    EXPECT_EQ(value, 5);

    EXPECT_TRUE(stack.TryPop(value));
    EXPECT_EQ(value, 4);

    EXPECT_TRUE(stack.TryPop(value));
    EXPECT_EQ(value, 3);

    EXPECT_TRUE(stack.TryPop(value));
    EXPECT_EQ(value, 2);

    EXPECT_TRUE(stack.TryPop(value));
    EXPECT_EQ(value, 1);

    EXPECT_FALSE(stack.TryPop(value));
}

TEST_F(Stack_Fixture, TryPeek)
{
    Stack<int> stack = { 1, 2, 3, 4, 5 };

    int value;
    EXPECT_TRUE(stack.TryPeek(value));
    EXPECT_EQ(value, 5);
}