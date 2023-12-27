#include <gtest/gtest.h>

#include "Core/Collections/Dictionary.h"
#include "Core/Collections/List.h"

template<typename TKey, typename TValue>
using Dictionary = Otter::Dictionary<TKey, TValue>;

using HashUtils = Otter::Internal::HashUtils;

class Dictionary_Fixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Otter::MemorySystem::Initialise(4_KiB);
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

    EXPECT_EQ(dictionary.GetCapacity(), 3);
    EXPECT_EQ(dictionary.GetCount(), 3);
    EXPECT_FALSE(dictionary.IsEmpty());

    dictionary = {{ 1, 1 },
                  { 2, 2 },
                  { 3, 3 },
                  { 4, 4 }};

    EXPECT_EQ(dictionary.GetCapacity(), 7);
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

TEST_F(Dictionary_Fixture, TryAdd_SimpleCases)
{
    Dictionary<int, int> dictionary;

    EXPECT_TRUE(dictionary.TryAdd(1, 1));
    EXPECT_FALSE(dictionary.TryAdd(1, 2));
    EXPECT_TRUE(dictionary.TryAdd(1, 2, true));

    int value = 2;
    EXPECT_TRUE(dictionary.TryAdd(value, value));
    EXPECT_FALSE(dictionary.TryAdd(value, value)) << "Value already exists";
    EXPECT_FALSE(dictionary.TryAdd(value, value + 1)) << "Value already exists";
    EXPECT_TRUE(dictionary.TryAdd(3, 3));

    UInt64 capacity = Dictionary<int, int>::GetDefaultInitialCapacity();
    EXPECT_EQ(dictionary.GetCapacity(), capacity);

    EXPECT_TRUE(dictionary.TryAdd(4, 4));
    EXPECT_NE(dictionary.GetCapacity(), capacity) << "Capacity should have increased";
    EXPECT_EQ(dictionary.GetCapacity(),
              HashUtils::GetNextPrime(dictionary.GetCount() * Dictionary<int, int>::GetResizingFactor()));
    capacity = dictionary.GetCapacity();

    EXPECT_TRUE(dictionary.TryAdd(11, 11)) << "Collision with 4, should be added";
    EXPECT_TRUE(dictionary.TryAdd(18, 18)) << "Collision with 4, should be added";
    EXPECT_TRUE(dictionary.TryAdd(25, 25)) << "Collision with 4, should be added";
    EXPECT_NE(dictionary.GetCapacity(), capacity) << "Capacity should have increased";
    EXPECT_EQ(dictionary.GetCapacity(),
              HashUtils::GetNextPrime(dictionary.GetCount() * Dictionary<int, int>::GetResizingFactor()));
}

TEST_F(Dictionary_Fixture, TryAdd_CollisionSlots)
{
    Dictionary<int, int> dictionary;

    EXPECT_TRUE(dictionary.TryAdd(0, 0));
    UInt64 capacity = Dictionary<int, int>::GetDefaultInitialCapacity();
    EXPECT_EQ(dictionary.GetCapacity(), capacity);

    UInt64 index = 0;
    EXPECT_TRUE(dictionary.TryGetIndex(0, &index));
    EXPECT_EQ(index, 0) << "0 should be at index 0";

    EXPECT_TRUE(dictionary.TryAdd(3, 3)) << "Collision with 0, should be added";
    EXPECT_TRUE(dictionary.TryGetIndex(3, &index));
    EXPECT_EQ(index, 1) << "0 should be at index 0, and since 3 is a collision, it should be at index 1 (next "
                           "available slot)";

    EXPECT_TRUE(dictionary.TryAdd(1, 1)) << "Collision with item at index 1, should be added";
    EXPECT_TRUE(dictionary.TryGetIndex(1, &index));
    EXPECT_EQ(index, 1) << "1 should replace item at index 1";
    EXPECT_TRUE(dictionary.TryGetIndex(3, &index));
    EXPECT_EQ(index, 2) << "3 should have been moved to index 2, since it was a collision (next available slot)";

    EXPECT_EQ(dictionary.GetCapacity(), capacity) << "Capacity should not have increased";
}

TEST_F(Dictionary_Fixture, TryGet)
{
    Dictionary<int, int> dictionary = {{ 1, 1 },
                                       { 2, 2 },
                                       { 3, 3 },
                                       { 4, 4 },
                                       { 5, 5 }};

    int value;
    EXPECT_TRUE(dictionary.TryGet(1, &value));
    EXPECT_EQ(value, 1);

    EXPECT_TRUE(dictionary.TryGet(2, &value));
    EXPECT_EQ(value, 2);

    EXPECT_TRUE(dictionary.TryGet(3, &value));
    EXPECT_EQ(value, 3);

    EXPECT_TRUE(dictionary.TryGet(4, &value));
    EXPECT_EQ(value, 4);

    EXPECT_TRUE(dictionary.TryGet(5, &value));
    EXPECT_EQ(value, 5);

    EXPECT_FALSE(dictionary.TryGet(6, &value));
    EXPECT_FALSE(dictionary.TryGet(7, &value));
    EXPECT_FALSE(dictionary.TryGet(8, &value));
    EXPECT_FALSE(dictionary.TryGet(9, &value));
    EXPECT_FALSE(dictionary.TryGet(10, &value));

    EXPECT_EQ(dictionary.GetCount(), 5);
    EXPECT_FALSE(dictionary.IsEmpty());
}

TEST_F(Dictionary_Fixture, TryRemove_SimpleCases)
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
    EXPECT_FALSE(dictionary.TryRemove(11)) << "Collision with 4, should not be removed";
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

TEST_F(Dictionary_Fixture, TryRemove_Collisions)
{
    Dictionary<int, int> dictionary = {{ 0, 0 },
                                       { 3, 3 },
                                       { 6, 6 }};

    EXPECT_TRUE(dictionary.TryRemove(3));

    dictionary = {{ 0, 0 },
                  { 3, 3 },
                  { 6, 6 }};

    EXPECT_FALSE(dictionary.TryRemove(9)) << "Collision with all items, should not be removed";
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

    dictionary.ForEach([](int key, int value)
                       {
                           value++;
                       });

    int count = 0;
    dictionary.ForEach([&count](int key, int value)
                       {
                           EXPECT_EQ(key, value);
                           ++count;
                       });

    EXPECT_EQ(count, dictionary.GetCount());

    dictionary.ForEach([](int key, int* value)
                       {
                           (*value)++;
                       });

    count = 1;
    dictionary.ForEach([&count](int key, int* value)
                       {
                           EXPECT_EQ(++count, *value);
                       });

    EXPECT_EQ(count, dictionary.GetCount() + 1);
}

TEST_F(Dictionary_Fixture, TryForKey)
{
    Dictionary<int, Otter::List<int>> dictionary = {{ 1, { 1, 2 }},
                                                    { 2, { 1, 2 }}};

    EXPECT_EQ(dictionary.GetCount(), 2);

    Otter::List<int> list;
    EXPECT_TRUE(dictionary.TryGet(1, &list));

    EXPECT_EQ(list.GetCount(), 2);

    int count = 0;
    for (auto& value: list)
        EXPECT_EQ(value, ++count);

    EXPECT_EQ(count, list.GetCount());

    list.Add(0);
    EXPECT_EQ(list.GetCount(), 3);

    EXPECT_TRUE(dictionary.TryGet(1, &list));
    EXPECT_EQ(list.GetCount(), 2);

    EXPECT_TRUE(dictionary.TryForKey(1, [](Otter::List<int>& value)
    {
        value.Add(3);
    }));

    EXPECT_TRUE(dictionary.TryGet(1, &list));
    EXPECT_EQ(list.GetCount(), 3);
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

    dictionary.ForEachValue([](int value)
                            {
                                value++;
                            });

    int count = 0;
    dictionary.ForEachValue([&count](int value)
                            {
                                EXPECT_EQ(++count, value);
                            });

    EXPECT_EQ(count, dictionary.GetCount());

    dictionary.ForEachValue([](int* value)
                            {
                                (*value)++;
                            });

    count = 1;
    dictionary.ForEachValue([&count](int* value)
                            {
                                EXPECT_EQ(++count, *value);
                            });

    EXPECT_EQ(count, dictionary.GetCount() + 1);
}

TEST_F(Dictionary_Fixture, EnsureCapacity)
{
    Dictionary<int, int> dictionary;

    EXPECT_EQ(dictionary.GetCapacity(), 0);

    dictionary.EnsureCapacity(10);

    EXPECT_GE(dictionary.GetCapacity(), 10);

    EXPECT_TRUE(dictionary.TryAdd(1, 1));
    dictionary.EnsureCapacity(15);
    EXPECT_GE(dictionary.GetCapacity(), 15);
    EXPECT_TRUE(dictionary.Contains(1));
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

TEST_F(Dictionary_Fixture, GetMemoryFootprint)
{
    Dictionary<int, int> dictionary = {{ 1, 1 },
                                       { 2, 2 },
                                       { 3, 3 },
                                       { 4, 4 },
                                       { 5, 5 }};

    auto footprint1 = dictionary.GetMemoryFootprint(OTR_NAME_OF(Dictionary<int, int>));
    EXPECT_EQ(footprint1.GetSize(), 3);

    auto* pointer1 = footprint1[0].GetData().GetPointer();

    EXPECT_EQ(footprint1[0].GetData().GetName(), OTR_NAME_OF(Dictionary<int, int>));
    EXPECT_NE(pointer1, nullptr);
    EXPECT_NE(footprint1[0].Offset, Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(footprint1[0].Padding, 0);
    EXPECT_EQ(footprint1[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

    EXPECT_EQ(footprint1[1].GetData().GetName(), OTR_NAME_OF(BitSet));
    EXPECT_NE(footprint1[1].GetData().GetPointer(), nullptr);
    EXPECT_EQ(footprint1[2].GetData().GetName(), OTR_NAME_OF(BitSet));
    EXPECT_NE(footprint1[2].GetData().GetPointer(), nullptr);

    dictionary.TryAdd(6, 6);
    dictionary.TryAdd(7, 7);
    dictionary.TryAdd(8, 8);

    auto footprint2 = dictionary.GetMemoryFootprint(OTR_NAME_OF(Dictionary<int, int>));
    EXPECT_EQ(footprint2.GetSize(), 3);

    EXPECT_EQ(footprint2[0].GetData().GetName(), OTR_NAME_OF(Dictionary<int, int>));
    EXPECT_NE(footprint2[0].GetData().GetPointer(), pointer1);
    EXPECT_NE(footprint2[0].GetData().GetPointer(), nullptr);
    EXPECT_NE(footprint2[0].GetData().GetPointer(), footprint1[0].GetData().GetPointer())
                    << "Pointer should have changed because of capacity increase (reallocation)";
    EXPECT_NE(footprint2[0].Offset, footprint1[0].Offset)
                    << "Offset should have changed because of capacity increase (reallocation)";
    EXPECT_EQ(footprint2[0].Padding, 0);
    EXPECT_EQ(footprint2[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

    EXPECT_EQ(footprint2[1].GetData().GetName(), OTR_NAME_OF(BitSet));
    EXPECT_NE(footprint2[1].GetData().GetPointer(), nullptr);
    EXPECT_EQ(footprint2[2].GetData().GetName(), OTR_NAME_OF(BitSet));
    EXPECT_NE(footprint2[2].GetData().GetPointer(), nullptr);

    dictionary.ClearDestructive();

    auto footprint3 = dictionary.GetMemoryFootprint(OTR_NAME_OF(Dictionary<int, int>));
    EXPECT_EQ(footprint3.GetSize(), 3);

    EXPECT_EQ(footprint3[0].GetData().GetName(), OTR_NAME_OF(Dictionary<int, int>));
    EXPECT_EQ(footprint3[0].GetData().GetPointer(), nullptr);
    EXPECT_EQ(footprint3[0].Size, 0);
    EXPECT_EQ(footprint3[0].Offset, 0);
    EXPECT_EQ(footprint3[0].Padding, 0);
    EXPECT_EQ(footprint3[0].Alignment, 0);

    EXPECT_EQ(footprint3[1].GetData().GetName(), OTR_NAME_OF(BitSet));
    EXPECT_EQ(footprint3[1].GetData().GetPointer(), nullptr);
    EXPECT_EQ(footprint3[2].GetData().GetName(), OTR_NAME_OF(BitSet));
    EXPECT_EQ(footprint3[2].GetData().GetPointer(), nullptr);
}

TEST_F(Dictionary_Fixture, Iterator)
{
    int temp[] = { 1, 2, 5, 6 };

    Dictionary<int, int> dictionary = {{ 1, 1 },
                                       { 2, 2 },
                                       { 5, 5 },
                                       { 6, 6 }};

    UInt64 i = 0;
    for (const auto& [key, value]: dictionary)
    {
        EXPECT_EQ(key, temp[i]);
        EXPECT_EQ(value, temp[i]);
        ++i;
    }

    for (auto it = dictionary.rbegin(); it != dictionary.rend(); --it)
    {
        EXPECT_EQ((*it).Key, temp[i]);
        EXPECT_EQ((*it).Value, temp[i]);
        --i;
    }
}