#include "Core/Memory.h"

namespace Otter
{
    bool              MemorySystem::s_HasInitialised = false;
    FreeListAllocator MemorySystem::s_Allocator{ };

    void MemorySystem::Initialise(const UInt64 memoryRequirements)
    {
        OTR_INTERNAL_ASSERT_MSG(!s_HasInitialised, "Memory has already been initialised")

        void* memory = Platform::Allocate(memoryRequirements);

        s_Allocator      = FreeListAllocator(memory, memoryRequirements, FreeListAllocator::Policy::FirstFit);
        s_HasInitialised = true;

        OTR_LOG_DEBUG("Memory system initialized with {0} bytes available", memoryRequirements)
    }

    void MemorySystem::Shutdown()
    {
        OTR_LOG_DEBUG("Shutting down memory system...")
        OTR_INTERNAL_ASSERT_MSG(s_HasInitialised, "Memory has not been initialised")

        void* memoryBlock = s_Allocator.GetMemoryUnsafePointer();
        if (memoryBlock)
        {
            Platform::MemoryClear(memoryBlock, s_Allocator.GetMemorySize());
            Platform::Free(memoryBlock);
        }

        s_HasInitialised = false;
    }

    UnsafeHandle MemorySystem::Allocate(const UInt64 size, const UInt16 alignment /*= OTR_PLATFORM_MEMORY_ALIGNMENT*/)
    {
        if (!s_HasInitialised)
            return { };

        OTR_INTERNAL_ASSERT_MSG(size > 0, "Allocation size must be greater than 0 bytes")
        OTR_INTERNAL_ASSERT_MSG(alignment >= OTR_PLATFORM_MEMORY_ALIGNMENT,
                                "Allocation alignment must be greater than or equal to the platform alignment")

        UnsafeHandle handle{ };
        handle.Pointer = s_Allocator.Allocate(size, alignment);
        handle.Size    = size;

        Platform::MemoryClear(handle.Pointer, handle.Size);

        return handle;
    }

    UnsafeHandle MemorySystem::Reallocate(UnsafeHandle& handle,
                                          const UInt64 size,
                                          const UInt16 alignment /*= OTR_PLATFORM_MEMORY_ALIGNMENT*/)
    {
        if (!s_HasInitialised)
            return { };

        OTR_INTERNAL_ASSERT_MSG(handle.Pointer != nullptr, "Existing handle must not be null")
        OTR_INTERNAL_ASSERT_MSG(handle.Size > 0, "Existing handle size must be greater than 0 bytes")
        OTR_INTERNAL_ASSERT_MSG(size > 0, "Reallocation size must be greater than 0 bytes")
        OTR_INTERNAL_ASSERT_MSG(alignment >= OTR_PLATFORM_MEMORY_ALIGNMENT,
                                "Allocation alignment must be greater than or equal to the platform alignment")

        if (size < handle.Size)
        {
            OTR_LOG_WARNING("Reallocation size is smaller than the existing handle size. "
                            "Make sure you are not losing data.")
        }

        UnsafeHandle newHandle{ };
        newHandle.Pointer = s_Allocator.Allocate(size, alignment);
        newHandle.Size    = size;

        Platform::MemoryClear(newHandle.Pointer, newHandle.Size);
        Platform::MemoryCopy(newHandle.Pointer, handle.Pointer, size);

        s_Allocator.Free(handle.Pointer);
        handle.Pointer = nullptr;
        handle.Size    = 0;

        return newHandle;
    }

    void MemorySystem::Free(void* block)
    {
        if (!s_HasInitialised)
            return;

        OTR_INTERNAL_ASSERT_MSG(block != nullptr, "Block to be freed must not be null")

        s_Allocator.Free(block);
    }

    void MemorySystem::MemoryCopy(void* destination, const void* source, UInt64 size)
    {
        if (!s_HasInitialised)
        {
            OTR_LOG_WARNING(
                "Memory has not been initialised. Make sure to call Memory::Initialise() before using any"
                " memory functions. Note that there might be some global/static variables that use memory.")
            return;
        }

        OTR_INTERNAL_ASSERT_MSG(source != nullptr, "Source to be copied must not be null")
        OTR_INTERNAL_ASSERT_MSG(size > 0, "Copy size must be greater than 0 bytes")

        Platform::MemoryCopy(destination, source, size);
    }

    void MemorySystem::MemoryMove(void* destination, const void* source, UInt64 size)
    {
        if (!s_HasInitialised)
        {
            OTR_LOG_WARNING(
                "Memory has not been initialised. Make sure to call Memory::Initialise() before using any"
                " memory functions. Note that there might be some global/static variables that use memory.")
            return;
        }

        OTR_INTERNAL_ASSERT_MSG(source != nullptr, "Source to be moved must not be null")
        OTR_INTERNAL_ASSERT_MSG(size > 0, "Move size must be greater than 0 bytes")

        Platform::MemoryMove(destination, source, size);
    }

    void MemorySystem::MemoryClear(void* block, const UInt64 size)
    {
        if (!s_HasInitialised)
        {
            OTR_LOG_WARNING(
                "Memory has not been initialised. Make sure to call Memory::Initialise() before using any"
                " memory functions. Note that there might be some global/static variables that use memory.")
            return;
        }

        OTR_INTERNAL_ASSERT_MSG(block != nullptr, "Block to be cleared must not be null")
        OTR_INTERNAL_ASSERT_MSG(size > 0, "Clear size must be greater than 0 bytes")

        Platform::MemoryClear(block, size);
    }

    void MemorySystem::CheckMemoryFootprint(const Function<MemoryDebugHandle()>& callback,
                                            MemoryFootprint* outFootprints,
                                            UInt64* outFootprintCount)
    {
        OTR_INTERNAL_ASSERT_MSG(outFootprints != nullptr, "Out-Footprints pointer must not be null")

        if (!s_HasInitialised)
        {
            OTR_LOG_WARNING(
                "Memory has not been initialised. Make sure to call Memory::Initialise() before using any"
                " memory functions. Note that there might be some global/static variables that use memory.")
            return;
        }

        MemoryDebugHandle handle = callback();
        OTR_INTERNAL_ASSERT_MSG(handle.Pairs != nullptr, "Handle pointer must not be null")
        OTR_INTERNAL_ASSERT_MSG(handle.Size > 0, "Handle size must be greater than 0")

        for (UInt64 i = 0; i < handle.Size; i++)
        {
            const auto data = handle.Pairs[i];
            outFootprints[i] = MemoryFootprint::For(data);

            if (!data.GetPointer())
                continue;

            s_Allocator.GetMemoryFootprint(data.GetPointer(),
                                           &outFootprints[i].Size,
                                           &outFootprints[i].Offset,
                                           &outFootprints[i].Padding,
                                           &outFootprints[i].Alignment);
        }

        if (outFootprintCount)
            *outFootprintCount = handle.Size;
    }
}
