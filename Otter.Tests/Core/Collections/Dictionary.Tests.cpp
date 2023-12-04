#include <gtest/gtest.h>

#include "Core/Collections/Dictionary.h"

template<typename TKey, typename TValue>
using Dictionary = Otter::Dictionary<TKey, TValue>;

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

TEST_F(Dictionary_Fixture, GetMemoryFootprint)
{
    Dictionary<int, int> dictionary         = {{ 1, 1 },
                                               { 2, 2 },
                                               { 3, 3 },
                                               { 4, 4 },
                                               { 5, 5 }};
    UInt64               expectedCapacity   = Otter::Internal::HashUtils::GetNextPrime(5);
    UInt64               dictionaryByteSize = 0;

    UInt64 footprintsSize = 0;
    dictionary.GetMemoryFootprint(OTR_NAME_OF(Dictionary<int, int>), nullptr, &footprintsSize);
    EXPECT_EQ(footprintsSize, 1 + expectedCapacity);

    auto* footprints1 = Otter::Buffer::New<Otter::MemoryFootprint>(footprintsSize);
    dictionary.GetMemoryFootprint(OTR_NAME_OF(Dictionary<int, int>), footprints1, &footprintsSize);

    EXPECT_EQ(footprints1[0].GetData().GetName(), OTR_NAME_OF(Dictionary<int, int>));
    EXPECT_NE(footprints1[0].GetData().GetPointer(), nullptr);
    EXPECT_EQ(footprints1[0].Size, OTR_ALLOCATED_MEMORY(Otter::Bucket<int>, expectedCapacity));
    EXPECT_EQ(footprints1[0].Offset, Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(footprints1[0].Padding, 0);
    EXPECT_EQ(footprints1[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

    dictionaryByteSize += footprints1[0].Size;

    for (UInt64 i = 1; i < footprintsSize; ++i)
    {
        EXPECT_EQ(footprints1[i].GetData().GetName(), "bucket_" + std::to_string(i - 1));

        if (footprints1[i].GetData().GetPointer())
        {
            EXPECT_EQ(footprints1[i].Size, OTR_ALLOCATED_MEMORY(Otter::BucketItem<int>, 3));
            EXPECT_LT(footprints1[i].Offset, Otter::MemorySystem::GetMemorySize());
            EXPECT_EQ(footprints1[i].Padding, 0);
            EXPECT_EQ(footprints1[i].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

            dictionaryByteSize += footprints1[i].Size;
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

    EXPECT_EQ(dictionaryByteSize, Otter::MemorySystem::GetUsedMemory())
                    << "Dictionary should have allocated " << dictionaryByteSize
                    << " bytes of memory but has allocated " << Otter::MemorySystem::GetUsedMemory()
                    << " bytes instead";

    dictionary.TryAdd(6, 6);
    dictionary.TryAdd(7, 7);

    dictionary.GetMemoryFootprint(OTR_NAME_OF(Dictionary<int, int>), nullptr, &footprintsSize);
    EXPECT_EQ(footprintsSize, 1 + expectedCapacity);

    auto* footprints2 = Otter::Buffer::New<Otter::MemoryFootprint>(footprintsSize);
    dictionary.GetMemoryFootprint(OTR_NAME_OF(Dictionary<int, int>), footprints2, &footprintsSize);

    Otter::Buffer::Delete<Otter::MemoryFootprint>(footprints2, footprintsSize);

    dictionary.TryAdd(8, 8);

    expectedCapacity = Otter::Internal::HashUtils::GetNextPrime(8);
    dictionary.GetMemoryFootprint(OTR_NAME_OF(Dictionary<int, int>), nullptr, &footprintsSize);
    EXPECT_EQ(footprintsSize, 1 + expectedCapacity)
                    << "Capacity should have increased because of the new element, capacity should now be 11";

    auto* footprints3 = Otter::Buffer::New<Otter::MemoryFootprint>(footprintsSize);
    dictionary.GetMemoryFootprint(OTR_NAME_OF(Dictionary<int, int>), footprints3, &footprintsSize);

    EXPECT_EQ(footprints3[0].GetData().GetName(), OTR_NAME_OF(Dictionary<int, int>));
    EXPECT_NE(footprints3[0].GetData().GetPointer(), firstAllocationPointer);
    EXPECT_EQ(footprints3[0].Size, OTR_ALLOCATED_MEMORY(Otter::Bucket<int>, expectedCapacity));
    EXPECT_LT(footprints3[0].Offset, Otter::MemorySystem::GetMemorySize());
    EXPECT_EQ(footprints3[0].Padding, 0);
    EXPECT_EQ(footprints3[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

    dictionaryByteSize = footprints3[0].Size;

    for (UInt64 i = 1; i < footprintsSize; ++i)
    {
        EXPECT_EQ(footprints3[i].GetData().GetName(), "bucket_" + std::to_string(i - 1));

        if (footprints3[i].GetData().GetPointer())
        {
            EXPECT_EQ(footprints3[i].Size, OTR_ALLOCATED_MEMORY(Otter::BucketItem<int>, 3));
            EXPECT_LT(footprints3[i].Offset, Otter::MemorySystem::GetMemorySize());
            EXPECT_EQ(footprints3[i].Padding, 0);
            EXPECT_EQ(footprints3[i].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

            dictionaryByteSize += footprints3[i].Size;
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

    EXPECT_EQ(dictionaryByteSize, Otter::MemorySystem::GetUsedMemory())
                    << "Dictionary should have allocated " << dictionaryByteSize
                    << " bytes of memory but has allocated " << Otter::MemorySystem::GetUsedMemory()
                    << " bytes instead";

    dictionary.TryAdd(9, 9);
    dictionary.TryAdd(10, 10);
    dictionary.TryAdd(11, 11);
    dictionary.TryAdd(12, 12);
    dictionary.TryAdd(13, 13);
    dictionary.TryAdd(14, 14);

    expectedCapacity = Otter::Internal::HashUtils::GetNextPrime(14);
    dictionary.GetMemoryFootprint(OTR_NAME_OF(Dictionary<int, int>), nullptr, &footprintsSize);
    EXPECT_EQ(footprintsSize, 1 + expectedCapacity)
                    << "Capacity should have increased because of the new element, capacity should now be 17";

    auto* footprints4 = Otter::Buffer::New<Otter::MemoryFootprint>(footprintsSize);
    dictionary.GetMemoryFootprint(OTR_NAME_OF(Dictionary<int, int>), footprints4, &footprintsSize);

    EXPECT_EQ(footprints4[0].GetData().GetName(), OTR_NAME_OF(Dictionary<int, int>));
    EXPECT_NE(footprints4[0].GetData().GetPointer(), firstAllocationPointer);
    EXPECT_EQ(footprints4[0].Size, OTR_ALLOCATED_MEMORY(Otter::Bucket<int>, expectedCapacity));
    EXPECT_LT(footprints4[0].Offset, Otter::MemorySystem::GetMemorySize());
    EXPECT_EQ(footprints4[0].Padding, 0);
    EXPECT_EQ(footprints4[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

    dictionaryByteSize = footprints4[0].Size;

    for (UInt64 i = 1; i < footprintsSize; ++i)
    {
        EXPECT_EQ(footprints4[i].GetData().GetName(), "bucket_" + std::to_string(i - 1));

        if (footprints4[i].GetData().GetPointer())
        {
            EXPECT_EQ(footprints4[i].Size, OTR_ALLOCATED_MEMORY(Otter::BucketItem<int>, 3));
            EXPECT_LT(footprints4[i].Offset, Otter::MemorySystem::GetMemorySize());
            EXPECT_EQ(footprints4[i].Padding, 0);
            EXPECT_EQ(footprints4[i].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);

            dictionaryByteSize += footprints4[i].Size;
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

    EXPECT_EQ(dictionaryByteSize, Otter::MemorySystem::GetUsedMemory())
                    << "Dictionary should have allocated " << dictionaryByteSize
                    << " bytes of memory but has allocated " << Otter::MemorySystem::GetUsedMemory()
                    << " bytes instead";

    dictionary.TryAdd(15, 15);
    dictionary.TryAdd(16, 16);
    dictionary.TryAdd(17, 17);
    dictionary.TryAdd(18, 18);

    dictionary.ClearDestructive();

    dictionary.GetMemoryFootprint(OTR_NAME_OF(Dictionary<int, int>), nullptr, &footprintsSize);
    EXPECT_EQ(footprintsSize, 1)
                    << "Capacity should be 0 after destructive clear, should just contain the buckets list";

    auto* footprints5 = Otter::Buffer::New<Otter::MemoryFootprint>(footprintsSize);
    dictionary.GetMemoryFootprint(OTR_NAME_OF(Dictionary<int, int>), footprints5, &footprintsSize);

    EXPECT_EQ(footprints5[0].GetData().GetName(), OTR_NAME_OF(Dictionary<int, int>));
    EXPECT_EQ(footprints5[0].GetData().GetPointer(), nullptr);
    EXPECT_EQ(footprints5[0].Size, 0);
    EXPECT_EQ(footprints5[0].Offset, 0);
    EXPECT_EQ(footprints5[0].Padding, 0);
    EXPECT_EQ(footprints5[0].Alignment, 0);

    Otter::Buffer::Delete<Otter::MemoryFootprint>(footprints5, footprintsSize);

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0)
                    << "Dictionary should have allocated 0 bytes of memory but has allocated "
                    << Otter::MemorySystem::GetUsedMemory() << " bytes instead";
}