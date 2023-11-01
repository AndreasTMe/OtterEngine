#include "Otter.PCH.h"

#include "Core/Memory.h"

namespace Otter::MemorySystem
{
    static bool       gs_HasInitialised = false;
    FreeListAllocator g_Allocator;

    void Initialise()
    {
        OTR_INTERNAL_ASSERT_MSG(!gs_HasInitialised, "Memory has already been initialised")

        const UInt64 memorySize = 5_KiB;
        void* memory = Platform::Allocate(memorySize);

        g_Allocator = FreeListAllocator(memory, memorySize, FreeListAllocator::Policy::FirstFit);

        OTR_LOG_DEBUG("Memory allocator initialized with {0} bytes", memorySize)

        gs_HasInitialised = true;
    }

    void Shutdown()
    {
        OTR_INTERNAL_ASSERT_MSG(gs_HasInitialised, "Memory has not been initialised")

        void* memoryBlock = g_Allocator.GetMemoryUnsafePointer();
        if (memoryBlock)
        {
            Platform::MemoryClear(memoryBlock, g_Allocator.GetMemorySize());
            Platform::Free(memoryBlock);
        }

        gs_HasInitialised = false;
    }

    UnsafeHandle Allocate(const UInt64& size, const UInt64& alignment)
    {
        if (!gs_HasInitialised)
            return { };

        OTR_INTERNAL_ASSERT_MSG(size > 0, "Allocation size must be greater than 0 bytes")
        OTR_INTERNAL_ASSERT_MSG(alignment >= OTR_PLATFORM_MEMORY_ALIGNMENT,
                                "Allocation alignment must be greater than or equal to the platform alignment")

        UnsafeHandle handle = { };
        handle.m_Pointer = g_Allocator.Allocate(size, alignment);
        handle.m_Size    = size;

        return handle;
    }

    // TODO: Use FreeListAllocator to reallocate memory
    UnsafeHandle Reallocate(UnsafeHandle& handle, const UInt64& size, const UInt64& alignment)
    {
        if (!gs_HasInitialised)
            return { };

        OTR_INTERNAL_ASSERT_MSG(handle.m_Pointer != nullptr, "Reallocation handle must not be null")
        OTR_INTERNAL_ASSERT_MSG(size > 0, "Reallocation size must be greater than 0 bytes")
        OTR_INTERNAL_ASSERT_MSG(alignment >= OTR_PLATFORM_MEMORY_ALIGNMENT,
                                "Allocation alignment must be greater than or equal to the platform alignment")

        UnsafeHandle newHandle = { };
        newHandle.m_Pointer = Platform::Reallocate(handle.m_Pointer, size);
        newHandle.m_Size    = size;

        return handle;
    }

    void Free(void* block)
    {
        if (!gs_HasInitialised)
            return;

        OTR_INTERNAL_ASSERT_MSG(block != nullptr, "Block to be freed must not be null")

        g_Allocator.Free(block);
    }

    // TODO: Use FreeListAllocator to clear memory
    void MemoryClear(void* block, const UInt64& size)
    {
        if (!gs_HasInitialised)
        {
            OTR_LOG_WARNING("Memory has not been initialised. Make sure to call Memory::Initialise() before using any"
                            " memory functions. Note that there might be some global/static variables that use memory.")
            return;
        }

        OTR_INTERNAL_ASSERT_MSG(block != nullptr, "Block to be cleared must not be null")
        OTR_INTERNAL_ASSERT_MSG(size > 0, "Clear size must be greater than 0 bytes")

        Platform::MemoryClear(block, size);
    }

    // TODO: Will probably be removed
    std::string GetTotalAllocation()
    {
        return std::to_string(g_Allocator.GetMemoryUsed()) + " / "
               + std::to_string(g_Allocator.GetMemorySize()) + " bytes";
    }
}
