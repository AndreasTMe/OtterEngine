#include <gtest/gtest.h>

#include "Core/Collections/HashSet.h"

template<typename T>
using HashSet = Otter::HashSet<T>;

class HashSet_Fixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Otter::MemorySystem::Initialise(2_KiB);
    }

    void TearDown() override
    {
        Otter::MemorySystem::Shutdown();
    }
};

TEST_F(HashSet_Fixture, Initialisation_Default)
{
    HashSet<int> hashSet;

    EXPECT_EQ(hashSet.GetCount(), 0);
    EXPECT_TRUE(hashSet.IsEmpty());
    EXPECT_FALSE(hashSet.IsCreated());
}

TEST_F(HashSet_Fixture, Initialisation_FromInitialisationList)
{
    HashSet<int> hashSet = { 1, 2, 3, 4, 4, 4, 5 };

    EXPECT_EQ(hashSet.GetCount(), 5);
    EXPECT_FALSE(hashSet.IsEmpty());

    hashSet = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    EXPECT_EQ(hashSet.GetCount(), 10);
    EXPECT_FALSE(hashSet.IsEmpty());
}

TEST_F(HashSet_Fixture, Initialisation_Copy)
{
    HashSet<int> hashSet = { 1, 2, 3, 4, 5 };
    HashSet<int> copy    = hashSet;

    EXPECT_EQ(copy.GetCount(), hashSet.GetCount());
    EXPECT_FALSE(copy.IsEmpty());
}

TEST_F(HashSet_Fixture, Initialisation_Move)
{
    HashSet<int> hashSet = { 1, 2, 3, 4, 5 };
    HashSet<int> move    = std::move(hashSet);

    EXPECT_EQ(move.GetCount(), 5);
    EXPECT_FALSE(move.IsEmpty());
}

TEST_F(HashSet_Fixture, Assignment_Copy)
{
    HashSet<int> hashSet = { 1, 2, 3, 4, 5 };
    HashSet<int> copy    = { 6, 7, 8, 9, 10 };

    copy = hashSet;

    EXPECT_EQ(copy.GetCount(), hashSet.GetCount());
    EXPECT_FALSE(copy.IsEmpty());
}

TEST_F(HashSet_Fixture, Assignment_Move)
{
    HashSet<int> hashSet = { 1, 2, 3, 4, 5 };
    HashSet<int> move    = { 6, 7, 8, 9, 10 };

    move = std::move(hashSet);

    EXPECT_EQ(move.GetCount(), 5);
    EXPECT_FALSE(move.IsEmpty());
}

TEST_F(HashSet_Fixture, TryAdd)
{
    HashSet<int> hashSet;

    int value = 2;
    EXPECT_TRUE(hashSet.TryAdd(1));
    EXPECT_TRUE(hashSet.TryAdd(value));
    EXPECT_FALSE(hashSet.TryAdd(value));
    EXPECT_TRUE(hashSet.TryAdd(3));

    EXPECT_TRUE(hashSet.TryAdd(4));
    EXPECT_TRUE(hashSet.TryAdd(11)); // Collision with previous value
    EXPECT_TRUE(hashSet.TryAdd(18)); // Collision with previous value
    EXPECT_TRUE(hashSet.TryAdd(25)); // Collision with previous value

    EXPECT_TRUE(hashSet.TryAdd(5));
    EXPECT_FALSE(hashSet.TryAdd(5));
    value = 12;
    EXPECT_TRUE(hashSet.TryAdd(value)); // Collision with previous value

    EXPECT_EQ(hashSet.GetCount(), 9);
    EXPECT_FALSE(hashSet.IsEmpty());
}

TEST_F(HashSet_Fixture, TryRemove)
{
    HashSet<int> hashSet = { 1, 2, 3, 4, 5 };

    int value = 2;
    EXPECT_TRUE(hashSet.TryRemove(1));
    EXPECT_TRUE(hashSet.TryRemove(value));
    EXPECT_TRUE(hashSet.TryRemove(3));
    EXPECT_TRUE(hashSet.TryRemove(4));
    EXPECT_TRUE(hashSet.TryRemove(5));

    EXPECT_FALSE(hashSet.TryRemove(1));
    EXPECT_FALSE(hashSet.TryRemove(value));
    EXPECT_FALSE(hashSet.TryRemove(3));
    EXPECT_FALSE(hashSet.TryRemove(4));
    EXPECT_FALSE(hashSet.TryRemove(5));

    EXPECT_EQ(hashSet.GetCount(), 0);
    EXPECT_TRUE(hashSet.IsEmpty());
}

TEST_F(HashSet_Fixture, Contains)
{
    HashSet<int> hashSet = { 1, 2, 3, 4, 5, 11 };

    int value = 2;
    EXPECT_TRUE(hashSet.Contains(1));
    EXPECT_TRUE(hashSet.Contains(value));
    EXPECT_TRUE(hashSet.Contains(3));
    EXPECT_TRUE(hashSet.Contains(4));
    EXPECT_TRUE(hashSet.Contains(5));
    EXPECT_TRUE(hashSet.Contains(11)); // Collision with previous value

    EXPECT_TRUE(hashSet.TryRemove(1));
    EXPECT_FALSE(hashSet.Contains(1));
    EXPECT_FALSE(hashSet.Contains(6));
}

TEST_F(HashSet_Fixture, ForEach)
{
    HashSet<int> hashSet = { 1, 2, 3, 4, 5 };

    int count = 0;
    hashSet.ForEach([&count](int value)
                    {
                        EXPECT_EQ(value, ++count);
                    });
}

TEST_F(HashSet_Fixture, Clear)
{
    HashSet<int> hashSet = { 1, 2, 3, 4, 5 };

    EXPECT_EQ(hashSet.GetCount(), 5);
    EXPECT_FALSE(hashSet.IsEmpty());

    hashSet.Clear();

    EXPECT_EQ(hashSet.GetCount(), 0);
    EXPECT_TRUE(hashSet.IsEmpty());
    EXPECT_TRUE(hashSet.IsCreated());
}

TEST_F(HashSet_Fixture, ClearDestructive)
{
    HashSet<int> hashSet = { 1, 2, 3, 4, 5 };

    EXPECT_EQ(hashSet.GetCount(), 5);
    EXPECT_FALSE(hashSet.IsEmpty());

    hashSet.ClearDestructive();

    EXPECT_EQ(hashSet.GetCount(), 0);
    EXPECT_TRUE(hashSet.IsEmpty());
    EXPECT_FALSE(hashSet.IsCreated());
}