#include <gtest/gtest.h>

#include "Core/Collections/HashSet.h"
#include "Core/Collections/BitSet.h"

template<typename T>
using HashSet = Otter::HashSet<T>;

using HashUtils = Otter::Internal::HashUtils;

using BitSet = Otter::BitSet;

class HashSet_Fixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Otter::MemorySystem::Initialise(4_KiB);
    }

    void TearDown() override
    {
        EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
        Otter::MemorySystem::Shutdown();
    }
};

TEST_F(HashSet_Fixture, Initialisation_Default)
{
    HashSet<int> hashSet;

    EXPECT_EQ(hashSet.GetCapacity(), 0);
    EXPECT_EQ(hashSet.GetCount(), 0);
    EXPECT_TRUE(hashSet.IsEmpty());
    EXPECT_FALSE(hashSet.IsCreated());
}

TEST_F(HashSet_Fixture, Initialisation_FromInitialisationList)
{
    HashSet<int> hashSet = { 1, 2, 3, 4, 4, 4, 5 };

    EXPECT_EQ(hashSet.GetCapacity(), 7);
    EXPECT_EQ(hashSet.GetCount(), 5);
    EXPECT_FALSE(hashSet.IsEmpty());

    hashSet = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 10, 10, 10 };

    EXPECT_EQ(hashSet.GetCapacity(), 11);
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

TEST_F(HashSet_Fixture, Equality)
{
    HashSet<int> hashSet1 = { 1, 2, 3, 4, 5 };
    HashSet<int> hashSet2 = { 1, 2, 3, 4, 5 };
    HashSet<int> hashSet3 = { 1, 2, 3, 4, 5, 6 };

    EXPECT_TRUE(hashSet1 == hashSet2);
    EXPECT_FALSE(hashSet1 == hashSet3);
}

TEST_F(HashSet_Fixture, TryAdd_SimpleCases)
{
    HashSet<int> hashSet;

    EXPECT_TRUE(hashSet.TryAdd(1));

    int value = 2;
    EXPECT_TRUE(hashSet.TryAdd(value));
    EXPECT_FALSE(hashSet.TryAdd(value)) << "Value already exists";
    EXPECT_TRUE(hashSet.TryAdd(3));

    UInt64 capacity = HashSet<int>::GetDefaultInitialCapacity();
    EXPECT_EQ(hashSet.GetCapacity(), capacity);

    EXPECT_TRUE(hashSet.TryAdd(4));
    EXPECT_NE(hashSet.GetCapacity(), capacity) << "Capacity should have increased";
    EXPECT_EQ(hashSet.GetCapacity(), HashUtils::GetNextPrime(hashSet.GetCount() * HashSet<int>::GetResizingFactor()));
    capacity = hashSet.GetCapacity();

    EXPECT_TRUE(hashSet.TryAdd(11)) << "Collision with 4, should be added";
    EXPECT_TRUE(hashSet.TryAdd(18)) << "Collision with 4, should be added";
    EXPECT_TRUE(hashSet.TryAdd(25)) << "Collision with 4, should be added";
    EXPECT_NE(hashSet.GetCapacity(), capacity) << "Capacity should have increased";
    EXPECT_EQ(hashSet.GetCapacity(), HashUtils::GetNextPrime(hashSet.GetCount() * HashSet<int>::GetResizingFactor()));
}

TEST_F(HashSet_Fixture, TryAdd_CollisionSlots)
{
    HashSet<int> hashSet;

    EXPECT_TRUE(hashSet.TryAdd(0));
    UInt64 capacity = HashSet<int>::GetDefaultInitialCapacity();
    EXPECT_EQ(hashSet.GetCapacity(), capacity);

    UInt64 index = 0;
    EXPECT_TRUE(hashSet.TryGetIndex(0, &index));
    EXPECT_EQ(index, 0) << "0 should be at index 0";

    EXPECT_TRUE(hashSet.TryAdd(3)) << "Collision with 0, should be added";
    EXPECT_TRUE(hashSet.TryGetIndex(3, &index));
    EXPECT_EQ(index, 1) << "0 should be at index 0, and since 3 is a collision, it should be at index 1 (next "
                           "available slot)";

    EXPECT_TRUE(hashSet.TryAdd(1)) << "Collision with item at index 1, should be added";
    EXPECT_TRUE(hashSet.TryGetIndex(1, &index));
    EXPECT_EQ(index, 1) << "1 should replace item at index 1";
    EXPECT_TRUE(hashSet.TryGetIndex(3, &index));
    EXPECT_EQ(index, 2) << "3 should have been moved to index 2, since it was a collision (next available slot)";

    EXPECT_EQ(hashSet.GetCapacity(), capacity) << "Capacity should not have increased";
}

TEST_F(HashSet_Fixture, TryRemove_SimpleCases)
{
    HashSet<int> hashSet = { 1, 2, 3, 4, 5 };

    int value = 2;
    EXPECT_TRUE(hashSet.TryRemove(1));
    EXPECT_TRUE(hashSet.TryRemove(value));
    EXPECT_TRUE(hashSet.TryRemove(3));
    EXPECT_FALSE(hashSet.TryRemove(11)) << "Collision with 4, should not be removed";
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

TEST_F(HashSet_Fixture, TryRemove_Collisions)
{
    HashSet<int> hashSet = { 0, 3, 6 };

    EXPECT_TRUE(hashSet.TryRemove(3));

    hashSet = { 0, 3, 6 };

    EXPECT_FALSE(hashSet.TryRemove(9)) << "Collision with all items, should not be removed";
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

    EXPECT_EQ(count, 5);
}

TEST_F(HashSet_Fixture, EnsureCapacity)
{
    HashSet<int> hashSet;

    EXPECT_EQ(hashSet.GetCapacity(), 0);

    hashSet.EnsureCapacity(10);

    EXPECT_GE(hashSet.GetCapacity(), 10);

    EXPECT_TRUE(hashSet.TryAdd(1));
    hashSet.EnsureCapacity(15);
    EXPECT_GE(hashSet.GetCapacity(), 15);
    EXPECT_TRUE(hashSet.Contains(1));
}

TEST_F(HashSet_Fixture, Clear)
{
    HashSet<int> hashSet = { 1, 2, 3, 4, 5 };

    EXPECT_EQ(hashSet.GetCount(), 5);
    EXPECT_FALSE(hashSet.IsEmpty());

    EXPECT_FALSE(hashSet.TryAdd(1));
    EXPECT_FALSE(hashSet.TryAdd(2));
    EXPECT_FALSE(hashSet.TryAdd(3));
    EXPECT_FALSE(hashSet.TryAdd(4));
    EXPECT_FALSE(hashSet.TryAdd(5));

    hashSet.Clear();

    EXPECT_EQ(hashSet.GetCount(), 0);
    EXPECT_TRUE(hashSet.IsEmpty());
    EXPECT_TRUE(hashSet.IsCreated());

    EXPECT_TRUE(hashSet.TryAdd(1));
    EXPECT_TRUE(hashSet.TryAdd(2));
    EXPECT_TRUE(hashSet.TryAdd(3));
    EXPECT_TRUE(hashSet.TryAdd(4));
    EXPECT_TRUE(hashSet.TryAdd(5));
}

TEST_F(HashSet_Fixture, ClearDestructive)
{
    HashSet<int> hashSet1 = { 1, 2, 3, 4, 5 };

    EXPECT_EQ(hashSet1.GetCount(), 5);
    EXPECT_FALSE(hashSet1.IsEmpty());

    hashSet1.ClearDestructive();

    EXPECT_EQ(hashSet1.GetCount(), 0);
    EXPECT_TRUE(hashSet1.IsEmpty());
    EXPECT_FALSE(hashSet1.IsCreated());

    HashSet<BitSet> hashSet2 = { BitSet(), BitSet(), BitSet(), BitSet(), BitSet() };

    EXPECT_EQ(hashSet2.GetCount(), 1);
    EXPECT_FALSE(hashSet2.IsEmpty());

    hashSet2.ClearDestructive();

    EXPECT_EQ(hashSet2.GetCount(), 0);
    EXPECT_TRUE(hashSet2.IsEmpty());
    EXPECT_FALSE(hashSet2.IsCreated());
}

TEST_F(HashSet_Fixture, GetMemoryFootprint)
{
    HashSet<int> hashSet = { 1, 2, 3, 4, 5 };

    auto footprint1 = hashSet.GetMemoryFootprint(OTR_NAME_OF(HashSet<int>));
    EXPECT_EQ(footprint1.GetSize(), 3);

    auto* pointer1 = footprint1[0].GetData().GetPointer();

    EXPECT_EQ(footprint1[0].GetData().GetName(), OTR_NAME_OF(HashSet<int>));
    EXPECT_NE(pointer1, nullptr);
    EXPECT_NE(footprint1[0].Offset, Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(footprint1[0].Padding, 0);
    EXPECT_EQ(footprint1[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

    EXPECT_EQ(footprint1[1].GetData().GetName(), OTR_NAME_OF(BitSet));
    EXPECT_NE(footprint1[1].GetData().GetPointer(), nullptr);
    EXPECT_EQ(footprint1[2].GetData().GetName(), OTR_NAME_OF(BitSet));
    EXPECT_NE(footprint1[2].GetData().GetPointer(), nullptr);

    hashSet.TryAdd(6);
    hashSet.TryAdd(7);
    hashSet.TryAdd(8);

    auto footprint2 = hashSet.GetMemoryFootprint(OTR_NAME_OF(HashSet<int>));
    EXPECT_EQ(footprint2.GetSize(), 3);

    EXPECT_EQ(footprint2[0].GetData().GetName(), OTR_NAME_OF(HashSet<int>));
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

    hashSet.ClearDestructive();

    auto footprint3 = hashSet.GetMemoryFootprint(OTR_NAME_OF(HashSet<int>));
    EXPECT_EQ(footprint3.GetSize(), 3);

    EXPECT_EQ(footprint3[0].GetData().GetName(), OTR_NAME_OF(HashSet<int>));
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

TEST_F(HashSet_Fixture, Iterator)
{
    int          temp[]  = { 1, 2, 5, 6 };
    HashSet<int> hashSet = { 1, 2, 5, 6 };

    UInt64 i = 0;
    for (const auto& item: hashSet)
    {
        EXPECT_EQ(item, temp[i]);
        ++i;
    }

    for (auto it = hashSet.rbegin(); it != hashSet.rend(); --it)
    {
        EXPECT_EQ(*it, temp[i]);
        --i;
    }
}