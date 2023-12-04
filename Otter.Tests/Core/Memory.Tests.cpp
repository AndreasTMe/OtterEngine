#include <gtest/gtest.h>

#include "Core/Memory.h"

TEST(Memory, Initialisation)
{
    Otter::MemorySystem::Initialise(1_KiB);

    EXPECT_EQ(Otter::MemorySystem::GetFreeMemory(), 1_KiB);
    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
    EXPECT_EQ(Otter::MemorySystem::GetMemorySize(), 1_KiB);

    EXPECT_DEATH(Otter::MemorySystem::Initialise(1_KiB), "");

    Otter::MemorySystem::Shutdown();

    EXPECT_DEATH(Otter::MemorySystem::Shutdown(), "");
}

TEST(Memory, Allocation)
{
    auto handleUninitialised = Otter::MemorySystem::Allocate(512);

    EXPECT_EQ(handleUninitialised.Pointer, nullptr);
    EXPECT_EQ(handleUninitialised.Size, 0);

    Otter::MemorySystem::Initialise(1_KiB);

    auto handle = Otter::MemorySystem::Allocate(512);

    EXPECT_EQ(Otter::MemorySystem::GetFreeMemory(),
              512 - Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(),
              512 + Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(Otter::MemorySystem::GetMemorySize(), 1_KiB);

    Otter::MemorySystem::Free(handle.Pointer);

    EXPECT_EQ(Otter::MemorySystem::GetFreeMemory(), 1_KiB);
    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
    EXPECT_EQ(Otter::MemorySystem::GetMemorySize(), 1_KiB);

    Otter::MemorySystem::Shutdown();
}

TEST(Memory, Reallocation)
{
    Otter::MemorySystem::Initialise(1_KiB);

    auto handle = Otter::MemorySystem::Allocate(512);

    EXPECT_EQ(Otter::MemorySystem::GetFreeMemory(),
              512 - Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(),
              512 + Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(Otter::MemorySystem::GetMemorySize(), 1_KiB);

    auto handleReallocated = Otter::MemorySystem::Reallocate(handle, 256);

    EXPECT_EQ(handle.Pointer, nullptr);
    EXPECT_NE(handleReallocated.Pointer, nullptr);
    EXPECT_EQ(handleReallocated.Size, 256);

    EXPECT_EQ(Otter::MemorySystem::GetFreeMemory(),
              768 - Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(),
              256 + Otter::FreeListAllocator::GetAllocatorHeaderSize());
    EXPECT_EQ(Otter::MemorySystem::GetMemorySize(), 1_KiB);

    Otter::MemorySystem::Free(handleReallocated.Pointer);

    EXPECT_EQ(Otter::MemorySystem::GetFreeMemory(), 1_KiB);
    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
    EXPECT_EQ(Otter::MemorySystem::GetMemorySize(), 1_KiB);

    Otter::MemorySystem::Shutdown();
}

TEST(Memory, MemoryCopy)
{
    int source[5] = { 1, 2, 3, 4, 5 };

    Otter::MemorySystem::Initialise(1_KiB);
    int* destination = Otter::Buffer::New<int>(5);

    for (UInt64 i = 0; i < 5; ++i)
        destination[i] = 0;

    Otter::MemorySystem::MemoryCopy(destination, source, 5 * sizeof(int));
    EXPECT_NE(destination, nullptr);

    for (UInt64 i = 0; i < 5; ++i)
        EXPECT_EQ(destination[i], source[i]);

    Otter::Buffer::Delete<int>(destination, 5);

    Otter::MemorySystem::Shutdown();
}

TEST(Memory, MemoryClear)
{
    Otter::MemorySystem::Initialise(1_KiB);

    int* destination = Otter::Buffer::New<int>(5);

    for (UInt64 i = 0; i < 5; ++i)
        destination[i] = i + 1;

    Otter::MemorySystem::MemoryClear(destination, 5 * sizeof(int));

    for (UInt64 i = 0; i < 5; ++i)
        EXPECT_EQ(destination[i], 0);

    Otter::Buffer::Delete<int>(destination, 5);

    Otter::MemorySystem::Shutdown();
}

TEST(Memory, New_Delete)
{
    Otter::MemorySystem::Initialise(1_KiB);

    int* num = Otter::New<int>(5);

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(),
              OTR_ALIGNED_OFFSET(sizeof(int), OTR_PLATFORM_MEMORY_ALIGNMENT)
              + Otter::FreeListAllocator::GetAllocatorHeaderSize());

    EXPECT_NE(num, nullptr);
    EXPECT_EQ(*num, 5);

    Otter::Delete<int>(num);

    EXPECT_EQ(*num, 0);
    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);

    Otter::MemorySystem::Shutdown();
}

TEST(Memory, Buffer_New_Delete)
{
    Otter::MemorySystem::Initialise(1_KiB);

    int* num = Otter::Buffer::New<int>(5);

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(),
              5 * OTR_ALIGNED_OFFSET(sizeof(int), OTR_PLATFORM_MEMORY_ALIGNMENT)
              + Otter::FreeListAllocator::GetAllocatorHeaderSize());

    EXPECT_NE(num, nullptr);

    for (UInt64 i = 0; i < 5; ++i)
        EXPECT_EQ(num[i], 0);

    Otter::Buffer::Delete<int>(num, 5);

    EXPECT_EQ(*num, 0);
    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);

    Otter::MemorySystem::Shutdown();
}

TEST(Memory, Unsafe_New_Delete)
{
    Otter::MemorySystem::Initialise(1_KiB);

    auto handle = Otter::Unsafe::New(5);

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(),
              handle.Size + Otter::FreeListAllocator::GetAllocatorHeaderSize());

    EXPECT_NE(handle.Pointer, nullptr);

    Otter::Unsafe::Delete(handle);

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);

    Otter::MemorySystem::Shutdown();
}