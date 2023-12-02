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

TEST_F(Array_Fixture, AsReadOnly)
{
    Array<int, 5> array    = { 1, 2, 3, 4, 5 };
    auto          readOnly = array.AsReadOnly();

    EXPECT_NE(readOnly.GetData(), nullptr);
    EXPECT_EQ(readOnly.GetSize(), 5);

    for (UInt64 i = 0; i < readOnly.GetSize(); ++i)
        EXPECT_EQ(readOnly[i], i + 1);
}