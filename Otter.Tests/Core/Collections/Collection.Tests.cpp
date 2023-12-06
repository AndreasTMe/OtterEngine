#include <gtest/gtest.h>

#include "Core/Collections/Collection.h"

template<typename T>
using Collection = Otter::Collection<T>;

using Collections = Otter::Collections;

class Collection_Fixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        OTR_MEMORY_SYSTEM.Initialise(1_KiB);
    }

    void TearDown() override
    {
        OTR_MEMORY_SYSTEM.Shutdown();
    }
};

TEST_F(Collection_Fixture, Collections_NewInitialiserList)
{
    Collection<int> collection = Collections::New<int>({ 1, 2, 3, 4, 5 });

    EXPECT_NE(collection.GetData(), nullptr);
    EXPECT_EQ(collection.GetCapacity(), 5);
    EXPECT_EQ(collection.GetCount(), 5);
}

TEST_F(Collection_Fixture, Collections_NewInitialiserListOut)
{
    Collection<int> collection = Collections::Empty<int>();
    EXPECT_EQ(collection.GetData(), nullptr);
    EXPECT_EQ(collection.GetCapacity(), 0);
    EXPECT_EQ(collection.GetCount(), 0);

    Collections::New({ 1, 2, 3, 4, 5 }, collection);

    EXPECT_NE(collection.GetData(), nullptr);
    EXPECT_EQ(collection.GetCapacity(), 5);
    EXPECT_EQ(collection.GetCount(), 5);
}

TEST_F(Collection_Fixture, Collections_NewRawPointer)
{
    int* data = Otter::Buffer::New<int>(5);
    for (int i = 0; i < 5; i++)
        data[i] = i + 1;

    Collection<int> collection = Collections::Empty<int>();
    EXPECT_EQ(collection.GetData(), nullptr);
    EXPECT_EQ(collection.GetCapacity(), 0);
    EXPECT_EQ(collection.GetCount(), 0);

    Collections::New(data, 5, collection);

    EXPECT_NE(collection.GetData(), nullptr);
    EXPECT_EQ(collection.GetCapacity(), 5);
    EXPECT_EQ(collection.GetCount(), 5);

    Otter::Buffer::Delete(data, 5);
}

TEST_F(Collection_Fixture, Reserve)
{
    Collection<int> collection = Collections::New<int>({ 1, 2, 3, 4, 5 });
    collection.Reserve(10);

    EXPECT_NE(collection.GetData(), nullptr);
    EXPECT_EQ(collection.GetCapacity(), 10);
    EXPECT_EQ(collection.GetCount(), 0);
}

TEST_F(Collection_Fixture, Expand)
{
    Collection<int> collection = Collections::New<int>({ 1, 2, 3, 4, 5 });

    const auto oldCapacity  = collection.GetCapacity();
    const auto oldCount     = collection.GetCount();
    const auto expandAmount = 10;
    collection.Expand(expandAmount);

    EXPECT_NE(collection.GetData(), nullptr);
    EXPECT_EQ(collection.GetCapacity(), oldCapacity + expandAmount);
    EXPECT_EQ(collection.GetCount(), oldCount);
}

TEST_F(Collection_Fixture, Shrink)
{
    Collection<int> emptyCollection = Collections::Empty<int>();
    EXPECT_EQ(emptyCollection.GetCapacity(), 0);
    EXPECT_EQ(emptyCollection.GetCount(), 0);

    emptyCollection.Shrink(10);

    EXPECT_EQ(emptyCollection.GetCapacity(), 0);
    EXPECT_EQ(emptyCollection.GetCount(), 0);

    Collection<int> collection = Collections::New<int>({ 1, 2, 3, 4, 5 });

    const auto oldCapacity  = collection.GetCapacity();
    const auto oldCount     = collection.GetCount();
    const auto shrinkAmount = 3;

    collection.Shrink(shrinkAmount, true);

    EXPECT_NE(collection.GetData(), nullptr);
    EXPECT_EQ(collection.GetCapacity(), oldCapacity - shrinkAmount);
    EXPECT_EQ(collection.GetCount(), oldCount - shrinkAmount);
}

TEST_F(Collection_Fixture, Contains)
{
    Collection<int> collection = Collections::New<int>({ 1, 2, 3 });

    int value = 3;
    EXPECT_TRUE(collection.Contains(1));
    EXPECT_TRUE(collection.Contains(2));
    EXPECT_TRUE(collection.Contains(value));
    EXPECT_FALSE(collection.Contains(123));
}

TEST_F(Collection_Fixture, TryGetIndexOf)
{
    Collection<int> collection = Collections::New<int>({ 1, 2, 3 });

    UInt64 index = 0;
    EXPECT_TRUE(collection.TryGetIndexOf(1, index));
    EXPECT_EQ(index, 0);

    index = 0;
    EXPECT_TRUE(collection.TryGetIndexOf(2, index));
    EXPECT_EQ(index, 1);

    index = 0;
    int value = 3;
    EXPECT_TRUE(collection.TryGetIndexOf(value, index));
    EXPECT_EQ(index, 2);

    index = 0;
    EXPECT_FALSE(collection.TryGetIndexOf(123, index));
    EXPECT_EQ(index, 0);
}

TEST_F(Collection_Fixture, Clear)
{
    Collection<int> collection = Collections::New<int>({ 1, 2, 3 });
    collection.Clear();

    EXPECT_EQ(collection.GetCount(), 0);

    UInt64 index = 0;
    EXPECT_FALSE(collection.TryGetIndexOf(0, index));
}

TEST_F(Collection_Fixture, ClearDestructive)
{
    Collection<int> collection = Collections::New<int>({ 1, 2, 3 });
    collection.ClearDestructive();

    EXPECT_EQ(collection.GetCount(), 0);
    EXPECT_EQ(collection.GetCapacity(), 0);
    EXPECT_EQ(collection.GetData(), nullptr);
}