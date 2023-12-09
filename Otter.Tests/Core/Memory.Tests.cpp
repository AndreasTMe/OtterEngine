#include <gtest/gtest.h>

#include "Core/Memory.h"

using MemorySystem = Otter::MemorySystem;

TEST(Memory, Initialisation)
{
    MemorySystem::Initialise(1_KiB);

    EXPECT_EQ(MemorySystem::GetFreeMemory(), 1_KiB);
    EXPECT_EQ(MemorySystem::GetUsedMemory(), 0);
    EXPECT_EQ(MemorySystem::GetMemorySize(), 1_KiB);

    EXPECT_DEATH(MemorySystem::Initialise(1_KiB), "");

    MemorySystem::Shutdown();

    EXPECT_DEATH(MemorySystem::Shutdown(), "");
}

TEST(Memory, Allocation)
{
    auto handleUninitialised = MemorySystem::Allocate(512);

    EXPECT_EQ(handleUninitialised.Pointer, nullptr);
    EXPECT_EQ(handleUninitialised.Size, 0);

    MemorySystem::Initialise(1_KiB);

    auto handle = MemorySystem::Allocate(512);

    EXPECT_EQ(MemorySystem::GetFreeMemory(),
              512 - Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(MemorySystem::GetUsedMemory(),
              512 + Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(MemorySystem::GetMemorySize(), 1_KiB);

    MemorySystem::Free(handle.Pointer);

    EXPECT_EQ(MemorySystem::GetFreeMemory(), 1_KiB);
    EXPECT_EQ(MemorySystem::GetUsedMemory(), 0);
    EXPECT_EQ(MemorySystem::GetMemorySize(), 1_KiB);

    MemorySystem::Shutdown();
}

TEST(Memory, Reallocation)
{
    MemorySystem::Initialise(1_KiB);

    auto handle = MemorySystem::Allocate(512);

    EXPECT_EQ(MemorySystem::GetFreeMemory(),
              512 - Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(MemorySystem::GetUsedMemory(),
              512 + Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(MemorySystem::GetMemorySize(), 1_KiB);

    auto handleReallocated = MemorySystem::Reallocate(handle, 256);

    EXPECT_EQ(handle.Pointer, nullptr);
    EXPECT_NE(handleReallocated.Pointer, nullptr);
    EXPECT_EQ(handleReallocated.Size, 256);

    EXPECT_EQ(MemorySystem::GetFreeMemory(),
              768 - Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(MemorySystem::GetUsedMemory(),
              256 + Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(MemorySystem::GetMemorySize(), 1_KiB);

    MemorySystem::Free(handleReallocated.Pointer);

    EXPECT_EQ(MemorySystem::GetFreeMemory(), 1_KiB);
    EXPECT_EQ(MemorySystem::GetUsedMemory(), 0);
    EXPECT_EQ(MemorySystem::GetMemorySize(), 1_KiB);

    MemorySystem::Shutdown();
}

TEST(Memory, MemoryCopy)
{
    int source[5] = { 1, 2, 3, 4, 5 };

    MemorySystem::Initialise(1_KiB);
    int* destination = Otter::Buffer::New<int>(5);

    for (UInt64 i = 0; i < 5; ++i)
        destination[i] = 0;

    MemorySystem::MemoryCopy(destination, source, 5 * sizeof(int));
    EXPECT_NE(destination, nullptr);

    for (UInt64 i = 0; i < 5; ++i)
        EXPECT_EQ(destination[i], source[i]);

    Otter::Buffer::Delete<int>(destination, 5);

    MemorySystem::Shutdown();
}

TEST(Memory, MemoryClear)
{
    MemorySystem::Initialise(1_KiB);

    int* destination = Otter::Buffer::New<int>(5);

    for (UInt64 i = 0; i < 5; ++i)
        destination[i] = i + 1;

    MemorySystem::MemoryClear(destination, 5 * sizeof(int));

    for (UInt64 i = 0; i < 5; ++i)
        EXPECT_EQ(destination[i], 0);

    Otter::Buffer::Delete<int>(destination, 5);

    MemorySystem::Shutdown();
}

TEST(Memory, New_Delete)
{
    MemorySystem::Initialise(1_KiB);

    int* num = Otter::New<int>(5);

    EXPECT_EQ(MemorySystem::GetUsedMemory(),
              OTR_ALIGNED_OFFSET(sizeof(int), OTR_PLATFORM_MEMORY_ALIGNMENT)
              + Otter::FreeListAllocator::GetAllocatorHeaderSize());

    EXPECT_NE(num, nullptr);
    EXPECT_EQ(*num, 5);

    Otter::Delete<int>(num);

    EXPECT_EQ(*num, 0);
    EXPECT_EQ(MemorySystem::GetUsedMemory(), 0);

    MemorySystem::Shutdown();
}

TEST(Memory, Buffer_New_Delete)
{
    MemorySystem::Initialise(1_KiB);

    int* num = Otter::Buffer::New<int>(5);

    EXPECT_EQ(MemorySystem::GetUsedMemory(),
              5 * OTR_ALIGNED_OFFSET(sizeof(int), OTR_PLATFORM_MEMORY_ALIGNMENT)
              + Otter::FreeListAllocator::GetAllocatorHeaderSize());

    EXPECT_NE(num, nullptr);

    for (UInt64 i = 0; i < 5; ++i)
        EXPECT_EQ(num[i], 0);

    Otter::Buffer::Delete<int>(num, 5);

    EXPECT_EQ(*num, 0);
    EXPECT_EQ(MemorySystem::GetUsedMemory(), 0);

    MemorySystem::Shutdown();
}

TEST(Memory, Unsafe_New_Delete)
{
    MemorySystem::Initialise(1_KiB);

    auto handle = Otter::Unsafe::New(5);

    EXPECT_EQ(MemorySystem::GetUsedMemory(),
              handle.Size + Otter::FreeListAllocator::GetAllocatorHeaderSize());

    EXPECT_NE(handle.Pointer, nullptr);

    Otter::Unsafe::Delete(handle);

    EXPECT_EQ(MemorySystem::GetUsedMemory(), 0);

    MemorySystem::Shutdown();
}