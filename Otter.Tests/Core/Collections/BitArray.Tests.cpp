#include <gtest/gtest.h>

#include "Core/Collections/BitSet.h"

template<typename T>
using BitSet = Otter::BitSet;

class BitSet_Fixture : public ::testing::Test
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

TEST_F(BitSet_Fixture, Initialisation_Default)
{
    BitSet<int> bitset;

    EXPECT_EQ(bitset.GetSize(), 0);
    EXPECT_TRUE(bitset.IsEmpty());
    EXPECT_FALSE(bitset.IsCreated());
}

TEST_F(BitSet_Fixture, Initialisation_FromInitialisationList)
{
    BitSet<int> bitset = { true, false, true, false, true, false, true, false };

    EXPECT_EQ(bitset.GetSize(), 2);
    EXPECT_FALSE(bitset.IsEmpty());
    EXPECT_TRUE(bitset.IsCreated());
}

TEST_F(BitSet_Fixture, Initialisation_Copy)
{
    BitSet<int> bitset = { true, false, true, false, true, false, true, false };
    BitSet<int> copy   = bitset;

    EXPECT_EQ(copy.GetSize(), bitset.GetSize());
    EXPECT_FALSE(copy.IsEmpty());
    EXPECT_TRUE(copy.IsCreated());

    for (UInt64 i = 0; i < copy.GetSize(); ++i)
        EXPECT_EQ(copy.Get(i), bitset.Get(i));
}

TEST_F(BitSet_Fixture, Initialisation_Move)
{
    BitSet<int> bitset = { true, false, true, false, true, false, true, false };
    BitSet<int> move   = std::move(bitset);

    EXPECT_EQ(move.GetSize(), 2);
    EXPECT_FALSE(move.IsEmpty());
    EXPECT_TRUE(move.IsCreated());

    EXPECT_FALSE(bitset.IsCreated());
}

TEST_F(BitSet_Fixture, Assignment_Copy)
{
    BitSet<int> bitset = { true, false, true, false, true, false, true, false };
    BitSet<int> copy;
    copy = bitset;

    EXPECT_EQ(copy.GetSize(), bitset.GetSize());
    EXPECT_FALSE(copy.IsEmpty());
    EXPECT_TRUE(copy.IsCreated());

    for (UInt64 i = 0; i < copy.GetSize(); ++i)
        EXPECT_EQ(copy.Get(i), bitset.Get(i));
}

TEST_F(BitSet_Fixture, Assignment_Move)
{
    BitSet<int> bitset = { true, false, true, false, true, false, true, false };
    BitSet<int> move;
    move = std::move(bitset);

    EXPECT_EQ(move.GetSize(), 2);
    EXPECT_FALSE(move.IsEmpty());
    EXPECT_TRUE(move.IsCreated());

    EXPECT_FALSE(bitset.IsCreated());
}

TEST_F(BitSet_Fixture, Equality)
{
    BitSet<int> bitset1 = { true, false, true, false, true, false, true, false };
    BitSet<int> bitset2 = { true, false, true, false, true, false, true, false };
    BitSet<int> bitset3 = { false, true, false, true, false, true, false, true };

    EXPECT_TRUE(bitset1 == bitset2);
    EXPECT_FALSE(bitset1 == bitset3);
    EXPECT_FALSE(bitset2 == bitset3);
}

TEST_F(BitSet_Fixture, Inequality)
{
    BitSet<int> bitset1 = { true, false, true, false, true, false, true, false };
    BitSet<int> bitset2 = { true, false, true, false, true, false, true, false };
    BitSet<int> bitset3 = { false, true, false, true, false, true, false, true };

    EXPECT_FALSE(bitset1 != bitset2);
    EXPECT_TRUE(bitset1 != bitset3);
    EXPECT_TRUE(bitset2 != bitset3);
}

TEST_F(BitSet_Fixture, Get)
{
    BitSet<int> bitset = { true, false, true, false, true, false, true, false };

    EXPECT_TRUE(bitset.Get(0));
    EXPECT_FALSE(bitset.Get(1));
    EXPECT_TRUE(bitset.Get(2));
    EXPECT_FALSE(bitset.Get(3));
    EXPECT_TRUE(bitset.Get(4));
    EXPECT_FALSE(bitset.Get(5));
    EXPECT_TRUE(bitset.Get(6));
    EXPECT_FALSE(bitset.Get(7));
}

TEST_F(BitSet_Fixture, Set)
{
    BitSet<int> bitset = { true, false, true, false, true, false, true, false };

    bitset.Set(1, true);
    bitset.Set(3, true);
    bitset.Set(5, true);
    bitset.Set(7, true);

    EXPECT_TRUE(bitset.Get(0));
    EXPECT_TRUE(bitset.Get(1));
    EXPECT_TRUE(bitset.Get(2));
    EXPECT_TRUE(bitset.Get(3));
    EXPECT_TRUE(bitset.Get(4));
    EXPECT_TRUE(bitset.Get(5));
    EXPECT_TRUE(bitset.Get(6));
    EXPECT_TRUE(bitset.Get(7));
}

TEST_F(BitSet_Fixture, Includes)
{
    BitSet<int> bitset1 = { true, false, true, false, true, false, true, false };
    BitSet<int> bitset2 = { true, false, true, false, false, false, false, false };
    BitSet<int> bitset3 = { false, true, false, false, false, false, false, false };

    EXPECT_TRUE(bitset1.Includes(bitset2));
    EXPECT_FALSE(bitset1.Includes(bitset3));
}

TEST_F(BitSet_Fixture, Reserve)
{
    BitSet<int> bitset = { true, false, true, false, true, false, true, false };

    EXPECT_EQ(bitset.GetSize(), 2);

    bitset.Reserve(200);

    EXPECT_EQ(bitset.GetSize(), 4);
    EXPECT_FALSE(bitset.IsEmpty());
    EXPECT_TRUE(bitset.IsCreated());

    for (UInt64 i = 0; i < bitset.GetBitsSize(); ++i)
        EXPECT_EQ(bitset.Get(i), false);
}

TEST_F(BitSet_Fixture, Expand)
{
    BitSet<int> bitset = { true, false, true, false, true, false, true, false };

    EXPECT_EQ(bitset.GetSize(), 2);

    bitset.Expand(200); // 200 / 64 = 3.125 -> 4 UInt64s

    EXPECT_EQ(bitset.GetSize(), 6);
    EXPECT_FALSE(bitset.IsEmpty());
    EXPECT_TRUE(bitset.IsCreated());

    for (UInt64 i = 0; i < 8; ++i)
        EXPECT_EQ(bitset.Get(i), i % 2 == 0);

    for (UInt64 i = 8; i < bitset.GetBitsSize(); ++i)
        EXPECT_EQ(bitset.Get(i), false);
}

TEST_F(BitSet_Fixture, Shrink)
{
    BitSet<int> bitset;
    bitset.Reserve(200); // 200 / 64 = 3.125 -> 4 UInt64s

    EXPECT_EQ(bitset.GetSize(), 4);

    for (UInt64 i = 0; i < bitset.GetBitsSize(); ++i)
        bitset.Set(i, i % 2 == 0);

    bitset.Shrink(100); // 100 / 64 = 1.5625 -> 1 UInt64s

    EXPECT_EQ(bitset.GetSize(), 3);

    for (UInt64 i = 0; i < bitset.GetBitsSize(); ++i)
        EXPECT_EQ(bitset.Get(i), i % 2 == 0);
}

TEST_F(BitSet_Fixture, Clear)
{
    BitSet<int> bitset = { true, false, true, false, true, false, true, false };

    EXPECT_EQ(bitset.GetSize(), 2);

    bitset.Clear();

    EXPECT_EQ(bitset.GetSize(), 2);
    EXPECT_FALSE(bitset.IsEmpty());
    EXPECT_TRUE(bitset.IsCreated());

    for (UInt64 i = 0; i < bitset.GetBitsSize(); ++i)
        EXPECT_EQ(bitset.Get(i), false);
}

TEST_F(BitSet_Fixture, ClearDestructive)
{
    BitSet<int> bitset = { true, false, true, false, true, false, true, false };

    EXPECT_EQ(bitset.GetSize(), 2);

    bitset.ClearDestructive();

    EXPECT_EQ(bitset.GetSize(), 0);
    EXPECT_TRUE(bitset.IsEmpty());
    EXPECT_FALSE(bitset.IsCreated());
}

TEST_F(BitSet_Fixture, GetTrueCount)
{
    BitSet<int> bitset = { true, false, true, false, true, false, true, false };

    EXPECT_EQ(bitset.GetTrueCount(), 4);
}

TEST_F(BitSet_Fixture, GetMemoryFootprint)
{
    BitSet<int> bitset = { true, false, true, false, true, false, true, false };

    auto footprint = bitset.GetMemoryFootprint(OTR_NAME_OF(BitSet<int>));
    EXPECT_EQ(footprint.GetSize(), 1);

    EXPECT_EQ(footprint[0].GetData().GetName(), OTR_NAME_OF(BitSet<int>));
    EXPECT_EQ(footprint[0].Size, OTR_ALLOCATED_MEMORY(int, bitset.GetSize()));
    EXPECT_EQ(footprint[0].Offset, Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(footprint[0].Padding, 0);
    EXPECT_EQ(footprint[0].Alignment, OTR_PLATFORM_MEMORY_ALIGNMENT);
}