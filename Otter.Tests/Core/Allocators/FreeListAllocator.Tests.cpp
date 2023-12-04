#include <gtest/gtest.h>

#include "Core/Allocators/FreeListAllocator.h"

using FreeListAllocator = Otter::FreeListAllocator;

TEST(FreeListAllocator, Initialisation_Valid)
{
    void* block = malloc(1_KiB);
    Otter::FreeListAllocator allocator(block, 1_KiB);

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
    EXPECT_EQ(allocator.GetMemoryUsed(), firstAllocationSize + FreeListAllocator::GetAllocatorHeaderSize());

    void* allocation2 = allocator.Allocate(secondAllocationSize, alignment);
    EXPECT_NE(allocation2, nullptr);
    EXPECT_EQ(allocator.GetMemoryUsed(), firstAllocationSize + FreeListAllocator::GetAllocatorHeaderSize()
                                         + secondAllocationSize + FreeListAllocator::GetAllocatorHeaderSize());

    auto count = 0;
    for (auto& node: allocator)
    {
        EXPECT_GT(node.Size, 0);
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
    EXPECT_EQ(allocator.GetMemoryUsed(), firstAllocationSize + FreeListAllocator::GetAllocatorHeaderSize());

    allocator.Free(allocation1);
    EXPECT_EQ(allocator.GetMemoryUsed(), 0);

    auto count = 0;
    for (auto& node: allocator)
    {
        EXPECT_GT(node.Size, 0);
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
    EXPECT_EQ(allocator.GetMemoryUsed(), firstAllocationSize + FreeListAllocator::GetAllocatorHeaderSize());

    void* allocation2 = allocator.Allocate(secondAllocationSize, alignment);
    EXPECT_NE(allocation2, nullptr);
    EXPECT_EQ(allocator.GetMemoryUsed(), firstAllocationSize + FreeListAllocator::GetAllocatorHeaderSize()
                                         + secondAllocationSize + FreeListAllocator::GetAllocatorHeaderSize());

    allocator.Free(allocation1);
    EXPECT_EQ(allocator.GetMemoryUsed(), secondAllocationSize + FreeListAllocator::GetAllocatorHeaderSize());

    auto count = 0;
    for (auto& node: allocator)
    {
        EXPECT_GT(node.Size, 0);
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
    EXPECT_EQ(allocator.GetMemoryUsed(), firstAllocationSize + FreeListAllocator::GetAllocatorHeaderSize());

    void* allocation2 = allocator.Allocate(secondAllocationSize, alignment);
    EXPECT_NE(allocation2, nullptr);
    EXPECT_EQ(allocator.GetMemoryUsed(), firstAllocationSize + FreeListAllocator::GetAllocatorHeaderSize()
                                         + secondAllocationSize + FreeListAllocator::GetAllocatorHeaderSize());

    allocator.Free(allocation1);
    EXPECT_EQ(allocator.GetMemoryUsed(), secondAllocationSize + FreeListAllocator::GetAllocatorHeaderSize());

    auto count = 0;
    for (auto& node: allocator)
    {
        EXPECT_GT(node.Size, 0);
        count++;
    }

    EXPECT_EQ(count, 2);

    allocator.Free(allocation2);
    EXPECT_EQ(allocator.GetMemoryUsed(), 0);

    count = 0;
    for (auto& node: allocator)
    {
        EXPECT_GT(node.Size, 0);
        count++;
    }

    EXPECT_EQ(count, 1);

    free(block);
}

TEST(FreeListAllocator, GetMemoryFootprint)
{
    void* block = malloc(1_KiB);
    Otter::FreeListAllocator allocator(block, 1_KiB);

    const UInt64 firstAllocationSize  = 64;
    const UInt64 secondAllocationSize = 32;

    void* allocation1 = allocator.Allocate(firstAllocationSize, 4);
    EXPECT_NE(allocation1, nullptr);
    EXPECT_EQ(allocator.GetMemoryUsed(), firstAllocationSize + FreeListAllocator::GetAllocatorHeaderSize());

    void* allocation2 = allocator.Allocate(secondAllocationSize, 8);
    EXPECT_NE(allocation2, nullptr);
    EXPECT_EQ(allocator.GetMemoryUsed(), firstAllocationSize + FreeListAllocator::GetAllocatorHeaderSize()
                                         + secondAllocationSize + FreeListAllocator::GetAllocatorHeaderSize());

    UInt64 size    = 0;
    UInt64 offset  = 0;
    UInt16 padding = 0;
    UInt16 align   = 0;
    allocator.GetMemoryFootprint(allocation1, &size, &offset, &padding, &align);

    EXPECT_EQ(size, firstAllocationSize + FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(offset, FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(padding, 0);
    EXPECT_EQ(align, OTR_PLATFORM_MEMORY_ALIGNMENT);

    allocator.GetMemoryFootprint(allocation2, &size, &offset, &padding, &align);

    EXPECT_EQ(size, secondAllocationSize + FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(offset, firstAllocationSize + FreeListAllocator::GetAllocatorHeaderSize() * 2);
    EXPECT_EQ(padding, 0);
    EXPECT_EQ(align, OTR_PLATFORM_MEMORY_ALIGNMENT);

    free(block);
}