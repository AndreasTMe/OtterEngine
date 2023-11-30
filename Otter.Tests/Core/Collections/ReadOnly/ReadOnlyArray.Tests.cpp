#include <gtest/gtest.h>

#include "Core/Collections/Array.h"
#include "Core/Collections/ReadOnly/ReadOnlyArray.h"

template<typename T, UInt64 Size>
using ReadOnlyArray = Otter::ReadOnlyArray<T, Size>;

class ReadOnlyArray_Fixture : public ::testing::Test
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

TEST_F(ReadOnlyArray_Fixture, Initialisation_Default)
{
    ReadOnlyArray<int, 5> array;

    EXPECT_NE(array.GetData(), nullptr);
    EXPECT_EQ(array.GetSize(), 5);

    for (UInt64 i = 0; i < array.GetSize(); ++i)
        EXPECT_EQ(array[i], 0);
}

TEST_F(ReadOnlyArray_Fixture, Initialisation_FromList)
{
    ReadOnlyArray<int, 5> array = { 1, 2, 3, 4, 5 };

    EXPECT_NE(array.GetData(), nullptr);
    EXPECT_EQ(array.GetSize(), 5);

    for (UInt64 i = 0; i < array.GetSize(); ++i)
        EXPECT_EQ(array[i], i + 1);
}

TEST_F(ReadOnlyArray_Fixture, Initialisation_CopyArray)
{
    Otter::Array<int, 5>  array = { 1, 2, 3, 4, 5 };
    ReadOnlyArray<int, 5> readOnlyArray(array);

    EXPECT_NE(readOnlyArray.GetData(), nullptr);
    EXPECT_EQ(readOnlyArray.GetSize(), 5);

    for (UInt64 i = 0; i < readOnlyArray.GetSize(); ++i)
        EXPECT_EQ(readOnlyArray[i], i + 1);
}

TEST_F(ReadOnlyArray_Fixture, Initialisation_MoveArray)
{
    Otter::Array<int, 5>  array = { 1, 2, 3, 4, 5 };
    ReadOnlyArray<int, 5> readOnlyArray(std::move(array));

    EXPECT_NE(readOnlyArray.GetData(), nullptr);
    EXPECT_EQ(readOnlyArray.GetSize(), 5);

    for (UInt64 i = 0; i < readOnlyArray.GetSize(); ++i)
        EXPECT_EQ(readOnlyArray[i], i + 1);

    EXPECT_EQ(array.GetData(), nullptr);
}