#include "Otter.PCH.h"

#include "Core/Memory.h"

namespace Otter
{
    UInt64 Memory::s_TotalAllocation = 0;

    Memory::Handle Memory::Allocate(const UInt64 size, const UInt64 alignment)
    {
        OTR_INTERNAL_ASSERT_MSG(size > 0, "Allocation size must be greater than 0 bytes")

        return Memory::Handle{ Platform::Allocate(size) };
    }

    Memory::Handle Memory::Reallocate(Memory::Handle& handle,
                                      const UInt64 size,
                                      const UInt64 alignment)
    {
        OTR_INTERNAL_ASSERT_MSG(handle.m_Pointer != nullptr, "Reallocation handle must not be null")
        OTR_INTERNAL_ASSERT_MSG(size > 0, "Reallocation size must be greater than 0 bytes")

        return Memory::Handle{ Platform::Reallocate(handle.m_Pointer, size) };
    }

    void Memory::Free(void* block)
    {
        OTR_INTERNAL_ASSERT_MSG(block != nullptr, "Block to be freed must not be null")

        Platform::Free(block);
    }

    void Memory::MemoryClear(void* block, const UInt64 size)
    {
        OTR_INTERNAL_ASSERT_MSG(block != nullptr, "Block to be cleared must not be null")
        OTR_INTERNAL_ASSERT_MSG(size > 0, "Clear size must be greater than 0 bytes")

        Platform::MemoryClear(block, size);
    }

    void Memory::MemoryCopy(void* dest, const void* src, const UInt64 size)
    {
        OTR_INTERNAL_ASSERT_MSG(src != nullptr, "Source block must not be null")
        OTR_INTERNAL_ASSERT_MSG(size > 0, "Copy size must be greater than 0 bytes")

        Platform::MemoryCopy(dest, src, size);
    }
}
