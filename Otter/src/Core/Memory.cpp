#include "Otter.PCH.h"

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

    UnsafeHandle MemorySystem::Allocate(const UInt64 size, const UInt64 alignment /*= OTR_PLATFORM_MEMORY_ALIGNMENT*/)
    {
        if (!s_HasInitialised)
            return { };

        OTR_INTERNAL_ASSERT_MSG(size > 0, "Allocation size must be greater than 0 bytes")
        OTR_INTERNAL_ASSERT_MSG(alignment >= OTR_PLATFORM_MEMORY_ALIGNMENT,
                                "Allocation alignment must be greater than or equal to the platform alignment")

        UnsafeHandle handle{ };
        handle.Pointer = s_Allocator.Allocate(size, alignment);
        handle.Size    = size;

        return handle;
    }

    // TODO: Use FreeListAllocator to reallocate memory
    UnsafeHandle MemorySystem::Reallocate(UnsafeHandle& handle,
                                          const UInt64 size,
                                          const UInt64 alignment /*= OTR_PLATFORM_MEMORY_ALIGNMENT*/)
    {
        if (!s_HasInitialised)
            return { };

        OTR_INTERNAL_ASSERT_MSG(handle.Pointer != nullptr, "Reallocation handle must not be null")
        OTR_INTERNAL_ASSERT_MSG(size > 0, "Reallocation size must be greater than 0 bytes")
        OTR_INTERNAL_ASSERT_MSG(alignment >= OTR_PLATFORM_MEMORY_ALIGNMENT,
                                "Allocation alignment must be greater than or equal to the platform alignment")

        UnsafeHandle newHandle{ };
        newHandle.Pointer = Platform::Reallocate(handle.Pointer, size);
        newHandle.Size    = size;

        return handle;
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

    // TODO: Will probably be removed
    std::string MemorySystem::GetTotalAllocation()
    {
        return std::to_string(s_Allocator.GetMemoryUsed()) + " / "
               + std::to_string(s_Allocator.GetMemorySize()) + " bytes";
    }
}
