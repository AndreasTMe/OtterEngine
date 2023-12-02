#include <gtest/gtest.h>

#include "Core/Collections/Dictionary.h"

template<typename TKey, typename TValue>
using Dictionary = Otter::Dictionary<TKey, TValue>;

class Dictionary_Fixture : public ::testing::Test
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

TEST_F(Dictionary_Fixture, Initialisation_Default)
{
    Dictionary<int, int> dictionary;

    EXPECT_EQ(dictionary.GetCount(), 0);
    EXPECT_TRUE(dictionary.IsEmpty());
    EXPECT_FALSE(dictionary.IsCreated());
}

TEST_F(Dictionary_Fixture, Initialisation_FromInitialisationList)
{
    Dictionary<int, int> dictionary = {{ 1, 1 },
                                       { 2, 2 },
                                       { 3, 3 }};

    EXPECT_EQ(dictionary.GetCount(), 3);
    EXPECT_FALSE(dictionary.IsEmpty());

    dictionary = {{ 1, 1 },
                  { 2, 2 },
                  { 3, 3 },
                  { 4, 4 }};

    EXPECT_EQ(dictionary.GetCount(), 4);
    EXPECT_FALSE(dictionary.IsEmpty());
}

TEST_F(Dictionary_Fixture, Initialisation_Copy)
{
    Dictionary<int, int> dictionary = {{ 1, 1 },
                                       { 2, 2 },
                                       { 3, 3 }};

    Dictionary<int, int> copy = dictionary;

    EXPECT_EQ(copy.GetCount(), 3);
    EXPECT_FALSE(copy.IsEmpty());
}

TEST_F(Dictionary_Fixture, Initialisation_Move)
{
    Dictionary<int, int> dictionary = {{ 1, 1 },
                                       { 2, 2 },
                                       { 3, 3 }};

    Dictionary<int, int> move = std::move(dictionary);

    EXPECT_EQ(move.GetCount(), 3);
    EXPECT_FALSE(move.IsEmpty());
    EXPECT_FALSE(dictionary.IsCreated());
}

TEST_F(Dictionary_Fixture, Assignment_Copy)
{
    Dictionary<int, int> dictionary = {{ 1, 1 },
                                       { 2, 2 },
                                       { 3, 3 },
                                       { 4, 4 }};

    Dictionary<int, int> copy = {{ 5, 5 },
                                 { 6, 6 },
                                 { 7, 7 }};

    copy = dictionary;

    EXPECT_EQ(copy.GetCount(), 4);
    EXPECT_FALSE(copy.IsEmpty());
}

TEST_F(Dictionary_Fixture, Assignment_Move)
{
    Dictionary<int, int> dictionary = {{ 1, 1 },
                                       { 2, 2 },
                                       { 3, 3 },
                                       { 4, 4 }};

    Dictionary<int, int> move = {{ 5, 5 },
                                 { 6, 6 },
                                 { 7, 7 }};

    move = std::move(dictionary);

    EXPECT_EQ(move.GetCount(), 4);
    EXPECT_FALSE(move.IsEmpty());
    EXPECT_FALSE(dictionary.IsCreated());
}

TEST_F(Dictionary_Fixture, TryAdd)
{
    Dictionary<int, int> dictionary;

    int value = 2;
    EXPECT_TRUE(dictionary.TryAdd(1, 1));
    EXPECT_TRUE(dictionary.TryAdd(value, value));
    EXPECT_FALSE(dictionary.TryAdd(value, value));
    EXPECT_TRUE(dictionary.TryAdd(3, 3));

    EXPECT_TRUE(dictionary.TryAdd(4, 4));
    EXPECT_TRUE(dictionary.TryAdd(11, 11)); // Collision with previous value
    EXPECT_TRUE(dictionary.TryAdd(18, 18)); // Collision with previous value
    EXPECT_TRUE(dictionary.TryAdd(25, 25)); // Collision with previous value

    EXPECT_TRUE(dictionary.TryAdd(5, 5));
    EXPECT_FALSE(dictionary.TryAdd(5, 5));
    value = 12;
    EXPECT_TRUE(dictionary.TryAdd(value, value)); // Collision with previous value

    EXPECT_EQ(dictionary.GetCount(), 9);
    EXPECT_FALSE(dictionary.IsEmpty());
}

TEST_F(Dictionary_Fixture, TryGet)
{
    Dictionary<int, int> dictionary = {{ 1, 1 },
                                       { 2, 2 },
                                       { 3, 3 },
                                       { 4, 4 },
                                       { 5, 5 }};

    int value;
    EXPECT_TRUE(dictionary.TryGet(1, value));
    EXPECT_EQ(value, 1);

    EXPECT_TRUE(dictionary.TryGet(2, value));
    EXPECT_EQ(value, 2);

    EXPECT_TRUE(dictionary.TryGet(3, value));
    EXPECT_EQ(value, 3);

    EXPECT_TRUE(dictionary.TryGet(4, value));
    EXPECT_EQ(value, 4);

    EXPECT_TRUE(dictionary.TryGet(5, value));
    EXPECT_EQ(value, 5);

    EXPECT_FALSE(dictionary.TryGet(6, value));
    EXPECT_FALSE(dictionary.TryGet(7, value));
    EXPECT_FALSE(dictionary.TryGet(8, value));
    EXPECT_FALSE(dictionary.TryGet(9, value));
    EXPECT_FALSE(dictionary.TryGet(10, value));

    EXPECT_EQ(dictionary.GetCount(), 5);
    EXPECT_FALSE(dictionary.IsEmpty());
}

TEST_F(Dictionary_Fixture, TryRemove)
{
    Dictionary<int, int> dictionary = {{ 1, 1 },
                                       { 2, 2 },
                                       { 3, 3 },
                                       { 4, 4 },
                                       { 5, 5 }};

    int value = 2;
    EXPECT_TRUE(dictionary.TryRemove(1));
    EXPECT_TRUE(dictionary.TryRemove(value));
    EXPECT_TRUE(dictionary.TryRemove(3));
    EXPECT_TRUE(dictionary.TryRemove(4));
    EXPECT_TRUE(dictionary.TryRemove(5));

    EXPECT_FALSE(dictionary.TryRemove(1));
    EXPECT_FALSE(dictionary.TryRemove(value));
    EXPECT_FALSE(dictionary.TryRemove(3));
    EXPECT_FALSE(dictionary.TryRemove(4));
    EXPECT_FALSE(dictionary.TryRemove(5));

    EXPECT_EQ(dictionary.GetCount(), 0);
    EXPECT_TRUE(dictionary.IsEmpty());
}

TEST_F(Dictionary_Fixture, Contains)
{
    Dictionary<int, int> dictionary = {{ 1,  1 },
                                       { 2,  2 },
                                       { 3,  3 },
                                       { 4,  4 },
                                       { 5,  5 },
                                       { 11, 11 }};

    int value = 2;
    EXPECT_TRUE(dictionary.Contains(1));
    EXPECT_TRUE(dictionary.Contains(value));
    EXPECT_TRUE(dictionary.Contains(3));
    EXPECT_TRUE(dictionary.Contains(4));
    EXPECT_TRUE(dictionary.Contains(5));
    EXPECT_TRUE(dictionary.Contains(11)); // Collision with previous value

    EXPECT_TRUE(dictionary.TryRemove(1));
    EXPECT_FALSE(dictionary.Contains(1));
    EXPECT_FALSE(dictionary.Contains(6));

    EXPECT_EQ(dictionary.GetCount(), 5);
    EXPECT_FALSE(dictionary.IsEmpty());
}

TEST_F(Dictionary_Fixture, ForEach)
{
    Dictionary<int, int> dictionary = {{ 1, 1 },
                                       { 2, 2 },
                                       { 3, 3 },
                                       { 4, 4 },
                                       { 5, 5 }};

    int count = 0;
    dictionary.ForEach([&count](int key, int value)
                       {
                           EXPECT_EQ(key, value);
                           ++count;
                       });

    EXPECT_EQ(count, dictionary.GetCount());
}

TEST_F(Dictionary_Fixture, ForEachKey)
{
    Dictionary<int, int> dictionary = {{ 1, 1 },
                                       { 2, 2 },
                                       { 3, 3 },
                                       { 4, 4 },
                                       { 5, 5 }};

    int count = 0;
    dictionary.ForEachKey([&count](int key)
                          {
                              EXPECT_EQ(++count, key);
                          });

    EXPECT_EQ(count, dictionary.GetCount());
}

TEST_F(Dictionary_Fixture, ForEachValue)
{
    Dictionary<int, int> dictionary = {{ 1, 1 },
                                       { 2, 2 },
                                       { 3, 3 },
                                       { 4, 4 },
                                       { 5, 5 }};

    int count = 0;
    dictionary.ForEachValue([&count](int value)
                            {
                                EXPECT_EQ(++count, value);
                            });

    EXPECT_EQ(count, dictionary.GetCount());
}

TEST_F(Dictionary_Fixture, Clear)
{
    Dictionary<int, int> dictionary = {{ 1, 1 },
                                       { 2, 2 },
                                       { 3, 3 },
                                       { 4, 4 },
                                       { 5, 5 }};

    EXPECT_EQ(dictionary.GetCount(), 5);
    EXPECT_FALSE(dictionary.IsEmpty());

    dictionary.Clear();

    EXPECT_EQ(dictionary.GetCount(), 0);
    EXPECT_TRUE(dictionary.IsEmpty());
    EXPECT_TRUE(dictionary.IsCreated());
}

TEST_F(Dictionary_Fixture, ClearDestructive)
{
    Dictionary<int, int> dictionary = {{ 1, 1 },
                                       { 2, 2 },
                                       { 3, 3 },
                                       { 4, 4 },
                                       { 5, 5 }};

    EXPECT_EQ(dictionary.GetCount(), 5);
    EXPECT_FALSE(dictionary.IsEmpty());

    dictionary.ClearDestructive();

    EXPECT_EQ(dictionary.GetCount(), 0);
    EXPECT_TRUE(dictionary.IsEmpty());
    EXPECT_FALSE(dictionary.IsCreated());
}