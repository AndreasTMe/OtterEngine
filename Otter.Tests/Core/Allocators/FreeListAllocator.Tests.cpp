#include <gtest/gtest.h>

#include "Core/Allocators/FreeListAllocator.h"

TEST(FreeListAllocator, Initialisation_Valid)
{
    void* block = malloc(1_KiB);
    Otter::FreeListAllocator allocator(block, 1_KiB);

    EXPECT_EQ(allocator.GetMemoryUnsafePointer(), block);
    EXPECT_EQ(allocator.GetMemorySize(), 1_KiB);
    EXPECT_EQ(allocator.GetMemoryUsed(), 0);
    EXPECT_EQ(allocator.GetMemoryFree(), 1_KiB);

    EXPECT_EQ(allocator.GetAllocationPolicy(), Otter::FreeListAllocator::Policy::FirstFit);

    free(block);
}

TEST(FreeListAllocator, Initialisation_Invalid)
{
    ASSERT_DEATH(Otter::FreeListAllocator allocator(nullptr, 1_KiB), "");
}

TEST(FreeListAllocator, Allocate_FindFirstFit)
{
    void* block = malloc(1_KiB);
    Otter::FreeListAllocator allocator(block, 1_KiB, Otter::FreeListAllocator::Policy::FirstFit);

    const UInt64 alignment            = 4;
    const UInt64 firstAllocationSize  = 64;
    const UInt64 secondAllocationSize = 32;

    void* allocation1 = allocator.Allocate(firstAllocationSize, alignment);
    EXPECT_NE(allocation1, nullptr);
    EXPECT_EQ(allocator.GetMemoryUsed(), firstAllocationSize + allocator.GetAllocatorHeaderSize());

    void* allocation2 = allocator.Allocate(secondAllocationSize, alignment);
    EXPECT_NE(allocation2, nullptr);
    EXPECT_EQ(allocator.GetMemoryUsed(), firstAllocationSize + allocator.GetAllocatorHeaderSize()
                                         + secondAllocationSize + allocator.GetAllocatorHeaderSize());

    auto count = 0;
    for (auto& node: allocator)
    {
        EXPECT_GT(node.m_Size, 0);
        count++;
    }

    EXPECT_EQ(count, 1);

    free(block);
}

// TODO: Failing test, BestFit policy is not implemented correctly
//TEST(FreeListAllocator, Allocate_FindBestFit)
//{
//}

TEST(FreeListAllocator, FreeSingleAllocation)
{
    void* block = malloc(1_KiB);
    Otter::FreeListAllocator allocator(block, 1_KiB);

    const UInt64 alignment           = 4;
    const UInt64 firstAllocationSize = 64;

    void* allocation1 = allocator.Allocate(firstAllocationSize, alignment);
    EXPECT_NE(allocation1, nullptr);
    EXPECT_EQ(allocator.GetMemoryUsed(), firstAllocationSize + allocator.GetAllocatorHeaderSize());

    allocator.Free(allocation1);
    EXPECT_EQ(allocator.GetMemoryUsed(), 0);

    auto count = 0;
    for (auto& node: allocator)
    {
        EXPECT_GT(node.m_Size, 0);
        count++;
    }

    EXPECT_EQ(count, 1);

    free(block);
}

TEST(FreeListAllocator, FreeAllocationWhenOthersPresent)
{
    void* block = malloc(1_KiB);
    Otter::FreeListAllocator allocator(block, 1_KiB);

    const UInt64 alignment            = 4;
    const UInt64 firstAllocationSize  = 64;
    const UInt64 secondAllocationSize = 32;

    void* allocation1 = allocator.Allocate(firstAllocationSize, alignment);
    EXPECT_NE(allocation1, nullptr);
    EXPECT_EQ(allocator.GetMemoryUsed(), firstAllocationSize + allocator.GetAllocatorHeaderSize());

    void* allocation2 = allocator.Allocate(secondAllocationSize, alignment);
    EXPECT_NE(allocation2, nullptr);
    EXPECT_EQ(allocator.GetMemoryUsed(), firstAllocationSize + allocator.GetAllocatorHeaderSize()
                                         + secondAllocationSize + allocator.GetAllocatorHeaderSize());

    allocator.Free(allocation1);
    EXPECT_EQ(allocator.GetMemoryUsed(), secondAllocationSize + allocator.GetAllocatorHeaderSize());

    auto count = 0;
    for (auto& node: allocator)
    {
        EXPECT_GT(node.m_Size, 0);
        count++;
    }

    EXPECT_EQ(count, 2);

    free(block);
}

TEST(FreeListAllocator, FreeMultipleAllocations)
{
    void* block = malloc(1_KiB);
    Otter::FreeListAllocator allocator(block, 1_KiB);

    const UInt64 alignment            = 4;
    const UInt64 firstAllocationSize  = 64;
    const UInt64 secondAllocationSize = 32;

    void* allocation1 = allocator.Allocate(firstAllocationSize, alignment);
    EXPECT_NE(allocation1, nullptr);
    EXPECT_EQ(allocator.GetMemoryUsed(), firstAllocationSize + allocator.GetAllocatorHeaderSize());

    void* allocation2 = allocator.Allocate(secondAllocationSize, alignment);
    EXPECT_NE(allocation2, nullptr);
    EXPECT_EQ(allocator.GetMemoryUsed(), firstAllocationSize + allocator.GetAllocatorHeaderSize()
                                         + secondAllocationSize + allocator.GetAllocatorHeaderSize());

    allocator.Free(allocation1);
    EXPECT_EQ(allocator.GetMemoryUsed(), secondAllocationSize + allocator.GetAllocatorHeaderSize());

    auto count = 0;
    for (auto& node: allocator)
    {
        EXPECT_GT(node.m_Size, 0);
        count++;
    }

    EXPECT_EQ(count, 2);

    allocator.Free(allocation2);
    EXPECT_EQ(allocator.GetMemoryUsed(), 0);

    count = 0;
    for (auto& node: allocator)
    {
        EXPECT_GT(node.m_Size, 0);
        count++;
    }

    EXPECT_EQ(count, 1);

    free(block);
}