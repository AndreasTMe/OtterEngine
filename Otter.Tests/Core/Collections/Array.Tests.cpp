#include <gtest/gtest.h>

#include "Core/Collections/Array.h"
#include "Core/Collections/ReadOnly/ReadOnlyArray.h"

template<typename T, UInt64 Size>
using Array = Otter::Array<T, Size>;

class Array_Fixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Otter::MemorySystem::Initialise(1_KiB);
    }

    void TearDown() override
    {
        EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
        Otter::MemorySystem::Shutdown();
    }
};

TEST_F(Array_Fixture, Initialisation_Default)
{
    Array<int, 5> array;

    EXPECT_NE(array.GetData(), nullptr);
    EXPECT_EQ(array.GetSize(), 5);

    for (UInt64 i = 0; i < array.GetSize(); ++i)
        EXPECT_EQ(array[i], 0);
}

TEST_F(Array_Fixture, Initialisation_FromList)
{
    Array<int, 5> array = { 1, 2, 3, 4, 5 };

    EXPECT_NE(array.GetData(), nullptr);
    EXPECT_EQ(array.GetSize(), 5);

    for (UInt64 i = 0; i < array.GetSize(); ++i)
        EXPECT_EQ(array[i], i + 1);
}

TEST_F(Array_Fixture, Initialisation_CopyArray)
{
    Array<int, 5> array = { 1, 2, 3, 4, 5 };
    Array<int, 5> copy(array);

    EXPECT_NE(copy.GetData(), nullptr);
    EXPECT_EQ(copy.GetSize(), 5);

    for (UInt64 i = 0; i < copy.GetSize(); ++i)
        EXPECT_EQ(copy[i], i + 1);
}

TEST_F(Array_Fixture, Initialisation_MoveArray)
{
    Array<int, 5> array = { 1, 2, 3, 4, 5 };
    Array<int, 5> move(std::move(array));

    EXPECT_NE(move.GetData(), nullptr);
    EXPECT_EQ(move.GetSize(), 5);

    for (UInt64 i = 0; i < move.GetSize(); ++i)
        EXPECT_EQ(move[i], i + 1);

    EXPECT_EQ(array.GetData(), nullptr);
}

TEST_F(Array_Fixture, Assignment_CopyArray)
{
    Array<int, 5> array = { 1, 2, 3, 4, 5 };
    Array<int, 5> copy;
    copy = array;

    EXPECT_NE(copy.GetData(), nullptr);
    EXPECT_EQ(copy.GetSize(), 5);

    for (UInt64 i = 0; i < copy.GetSize(); ++i)
        EXPECT_EQ(copy[i], i + 1);
}

TEST_F(Array_Fixture, Assignment_MoveArray)
{
    Array<int, 5> array = { 1, 2, 3, 4, 5 };
    Array<int, 5> move;
    move = std::move(array);

    EXPECT_NE(move.GetData(), nullptr);
    EXPECT_EQ(move.GetSize(), 5);

    for (UInt64 i = 0; i < move.GetSize(); ++i)
        EXPECT_EQ(move[i], i + 1);

    EXPECT_EQ(array.GetData(), nullptr);
}

TEST_F(Array_Fixture, Equality)
{
    Array<int, 5> array1 = { 1, 2, 3, 4, 5 };
    Array<int, 5> array2 = { 1, 2, 3, 4, 5 };
    Array<int, 5> array3 = { 5, 4, 3, 2, 1 };

    EXPECT_TRUE(array1 == array2);
    EXPECT_FALSE(array1 == array3);
}

TEST_F(Array_Fixture, AsReadOnly)
{
    Array<int, 5> array    = { 1, 2, 3, 4, 5 };
    auto          readOnly = array.AsReadOnly();

    EXPECT_NE(readOnly.GetData(), nullptr);
    EXPECT_EQ(readOnly.GetSize(), 5);

    for (UInt64 i = 0; i < readOnly.GetSize(); ++i)
        EXPECT_EQ(readOnly[i], i + 1);
}

TEST_F(Array_Fixture, GetMemoryFootprint)
{
    Array<int, 5> array = { 1, 2, 3, 4, 5 };

    auto footprint = array.GetMemoryFootprint(OTR_NAME_OF(Array<int, 5>));
    EXPECT_EQ(footprint.GetSize(), 1);

    EXPECT_EQ(footprint[0].GetData().GetName(), OTR_NAME_OF(Array<int, 5>));
    EXPECT_EQ(footprint[0].GetData().GetPointer(), array.GetData());
    EXPECT_EQ(footprint[0].Size, OTR_ALLOCATED_MEMORY(int, array.GetSize()));
    EXPECT_EQ(footprint[0].Offset, Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(footprint[0].Padding, 0);
    EXPECT_EQ(footprint[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);
}

TEST_F(Array_Fixture, Iterator)
{
    Array<int, 5> array = { 1, 2, 3, 4, 5 };

    UInt64 i = 0;
    for (auto& element: array)
        EXPECT_EQ(element, ++i);

    for (auto it = array.rbegin(); it != array.rend(); --it)
        EXPECT_EQ(*it, i--);

    for (auto it = array.cbegin(); it != array.cend(); ++it)
        EXPECT_EQ(*it, ++i);

    for (auto it = array.crbegin(); it != array.crend(); --it)
        EXPECT_EQ(*it, i--);
}