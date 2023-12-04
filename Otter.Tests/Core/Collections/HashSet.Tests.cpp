#include <gtest/gtest.h>

#include "Core/Collections/HashSet.h"

template<typename T>
using HashSet = Otter::HashSet<T>;

class HashSet_Fixture : public ::testing::Test
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

TEST_F(HashSet_Fixture, GetMemoryFootprint)
{
    HashSet<int> hashSet          = { 1, 2, 3, 4, 5 };
    UInt64       expectedCapacity = Otter::Internal::HashUtils::GetNextPrime(5);
    UInt64       hashSetByteSize  = 0;

    UInt64 footprintsSize = 0;
    hashSet.GetMemoryFootprint(OTR_NAME_OF(HashSet<int>), nullptr, &footprintsSize);
    EXPECT_EQ(footprintsSize, 1 + expectedCapacity);

    auto* footprints1 = Otter::Buffer::New<Otter::MemoryFootprint>(footprintsSize);
    hashSet.GetMemoryFootprint(OTR_NAME_OF(HashSet<int>), footprints1, &footprintsSize);

    EXPECT_EQ(footprints1[0].GetData().GetName(), OTR_NAME_OF(HashSet<int>));
    EXPECT_NE(footprints1[0].GetData().GetPointer(), nullptr);
    EXPECT_EQ(footprints1[0].Size, OTR_ALLOCATED_MEMORY(Otter::Bucket<int>, expectedCapacity));
    EXPECT_EQ(footprints1[0].Offset, Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(footprints1[0].Padding, 0);
    EXPECT_EQ(footprints1[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

    hashSetByteSize += footprints1[0].Size;

    for (UInt64 i = 1; i < footprintsSize; ++i)
    {
        EXPECT_EQ(footprints1[i].GetData().GetName(), "bucket_" + std::to_string(i - 1));

        if (footprints1[i].GetData().GetPointer())
        {
            EXPECT_EQ(footprints1[i].Size, OTR_ALLOCATED_MEMORY(Otter::BucketItem<int>, 3));
            EXPECT_LT(footprints1[i].Offset, Otter::MemorySystem::GetMemorySize());
            EXPECT_EQ(footprints1[i].Padding, 0);
            EXPECT_EQ(footprints1[i].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

            hashSetByteSize += footprints1[i].Size;
        }
        else
        {
            EXPECT_EQ(footprints1[i].Size, 0);
            EXPECT_EQ(footprints1[i].Offset, 0);
            EXPECT_EQ(footprints1[i].Padding, 0);
            EXPECT_EQ(footprints1[i].Alignment, 0);
        }
    }

    const void* const firstAllocationPointer = footprints1[0].GetData().GetPointer();

    Otter::Buffer::Delete<Otter::MemoryFootprint>(footprints1, footprintsSize);

    EXPECT_EQ(hashSetByteSize, Otter::MemorySystem::GetUsedMemory())
                    << "HashSet should have allocated " << hashSetByteSize
                    << " bytes of memory but has allocated " << Otter::MemorySystem::GetUsedMemory()
                    << " bytes instead";

    hashSet.TryAdd(6);
    hashSet.TryAdd(7);

    hashSet.GetMemoryFootprint(OTR_NAME_OF(HashSet<int>), nullptr, &footprintsSize);
    EXPECT_EQ(footprintsSize, 1 + expectedCapacity);

    auto* footprints2 = Otter::Buffer::New<Otter::MemoryFootprint>(footprintsSize);
    hashSet.GetMemoryFootprint(OTR_NAME_OF(HashSet<int>), footprints2, &footprintsSize);

    Otter::Buffer::Delete<Otter::MemoryFootprint>(footprints2, footprintsSize);

    hashSet.TryAdd(8);

    expectedCapacity = Otter::Internal::HashUtils::GetNextPrime(8);
    hashSet.GetMemoryFootprint(OTR_NAME_OF(HashSet<int>), nullptr, &footprintsSize);
    EXPECT_EQ(footprintsSize, 1 + expectedCapacity)
                    << "Capacity should have increased because of the new element, capacity should now be 11";

    auto* footprints3 = Otter::Buffer::New<Otter::MemoryFootprint>(footprintsSize);
    hashSet.GetMemoryFootprint(OTR_NAME_OF(HashSet<int>), footprints3, &footprintsSize);

    EXPECT_EQ(footprints3[0].GetData().GetName(), OTR_NAME_OF(HashSet<int>));
    EXPECT_NE(footprints3[0].GetData().GetPointer(), firstAllocationPointer);
    EXPECT_EQ(footprints3[0].Size, OTR_ALLOCATED_MEMORY(Otter::Bucket<int>, expectedCapacity));
    EXPECT_LT(footprints3[0].Offset, Otter::MemorySystem::GetMemorySize());
    EXPECT_EQ(footprints3[0].Padding, 0);
    EXPECT_EQ(footprints3[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

    hashSetByteSize = footprints3[0].Size;

    for (UInt64 i = 1; i < footprintsSize; ++i)
    {
        EXPECT_EQ(footprints3[i].GetData().GetName(), "bucket_" + std::to_string(i - 1));

        if (footprints3[i].GetData().GetPointer())
        {
            EXPECT_EQ(footprints3[i].Size, OTR_ALLOCATED_MEMORY(Otter::BucketItem<int>, 3));
            EXPECT_LT(footprints3[i].Offset, Otter::MemorySystem::GetMemorySize());
            EXPECT_EQ(footprints3[i].Padding, 0);
            EXPECT_EQ(footprints3[i].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

            hashSetByteSize += footprints3[i].Size;
        }
        else
        {
            EXPECT_EQ(footprints3[i].Size, 0);
            EXPECT_EQ(footprints3[i].Offset, 0);
            EXPECT_EQ(footprints3[i].Padding, 0);
            EXPECT_EQ(footprints3[i].Alignment, 0);
        }
    }

    Otter::Buffer::Delete<Otter::MemoryFootprint>(footprints3, footprintsSize);

    EXPECT_EQ(hashSetByteSize, Otter::MemorySystem::GetUsedMemory())
                    << "HashSet should have allocated " << hashSetByteSize
                    << " bytes of memory but has allocated " << Otter::MemorySystem::GetUsedMemory()
                    << " bytes instead";

    hashSet.TryAdd(9);
    hashSet.TryAdd(10);
    hashSet.TryAdd(11);
    hashSet.TryAdd(12);
    hashSet.TryAdd(13);
    hashSet.TryAdd(14);

    expectedCapacity = Otter::Internal::HashUtils::GetNextPrime(14);
    hashSet.GetMemoryFootprint(OTR_NAME_OF(HashSet<int>), nullptr, &footprintsSize);
    EXPECT_EQ(footprintsSize, 1 + expectedCapacity)
                    << "Capacity should have increased because of the new element, capacity should now be 17";

    auto* footprints4 = Otter::Buffer::New<Otter::MemoryFootprint>(footprintsSize);
    hashSet.GetMemoryFootprint(OTR_NAME_OF(HashSet<int>), footprints4, &footprintsSize);

    EXPECT_EQ(footprints4[0].GetData().GetName(), OTR_NAME_OF(HashSet<int>));
    EXPECT_NE(footprints4[0].GetData().GetPointer(), firstAllocationPointer);
    EXPECT_EQ(footprints4[0].Size, OTR_ALLOCATED_MEMORY(Otter::Bucket<int>, expectedCapacity));
    EXPECT_LT(footprints4[0].Offset, Otter::MemorySystem::GetMemorySize());
    EXPECT_EQ(footprints4[0].Padding, 0);
    EXPECT_EQ(footprints4[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

    hashSetByteSize = footprints4[0].Size;

    for (UInt64 i = 1; i < footprintsSize; ++i)
    {
        EXPECT_EQ(footprints4[i].GetData().GetName(), "bucket_" + std::to_string(i - 1));

        if (footprints4[i].GetData().GetPointer())
        {
            EXPECT_EQ(footprints4[i].Size, OTR_ALLOCATED_MEMORY(Otter::BucketItem<int>, 3));
            EXPECT_LT(footprints4[i].Offset, Otter::MemorySystem::GetMemorySize());
            EXPECT_EQ(footprints4[i].Padding, 0);
            EXPECT_EQ(footprints4[i].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

            hashSetByteSize += footprints4[i].Size;
        }
        else
        {
            EXPECT_EQ(footprints4[i].Size, 0);
            EXPECT_EQ(footprints4[i].Offset, 0);
            EXPECT_EQ(footprints4[i].Padding, 0);
            EXPECT_EQ(footprints4[i].Alignment, 0);
        }
    }

    Otter::Buffer::Delete<Otter::MemoryFootprint>(footprints4, footprintsSize);

    EXPECT_EQ(hashSetByteSize, Otter::MemorySystem::GetUsedMemory())
                    << "HashSet should have allocated " << hashSetByteSize
                    << " bytes of memory but has allocated " << Otter::MemorySystem::GetUsedMemory()
                    << " bytes instead";

    hashSet.TryAdd(15);
    hashSet.TryAdd(16);
    hashSet.TryAdd(17);
    hashSet.TryAdd(18);

    hashSet.ClearDestructive();

    hashSet.GetMemoryFootprint(OTR_NAME_OF(HashSet<int>), nullptr, &footprintsSize);
    EXPECT_EQ(footprintsSize, 1)
                    << "Capacity should be 0 after destructive clear, should just contain the buckets list";

    auto* footprints5 = Otter::Buffer::New<Otter::MemoryFootprint>(footprintsSize);
    hashSet.GetMemoryFootprint(OTR_NAME_OF(HashSet<int>), footprints5, &footprintsSize);

    EXPECT_EQ(footprints5[0].GetData().GetName(), OTR_NAME_OF(HashSet<int>));
    EXPECT_EQ(footprints5[0].GetData().GetPointer(), nullptr);
    EXPECT_EQ(footprints5[0].Size, 0);
    EXPECT_EQ(footprints5[0].Offset, 0);
    EXPECT_EQ(footprints5[0].Padding, 0);
    EXPECT_EQ(footprints5[0].Alignment, 0);

    Otter::Buffer::Delete<Otter::MemoryFootprint>(footprints5, footprintsSize);

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0)
                    << "HashSet should have allocated 0 bytes of memory but has allocated "
                    << Otter::MemorySystem::GetUsedMemory() << " bytes instead";
}