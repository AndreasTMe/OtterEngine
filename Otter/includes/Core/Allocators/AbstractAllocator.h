#ifndef OTTERENGINE_ABSTRACTALLOCATOR_H
#define OTTERENGINE_ABSTRACTALLOCATOR_H

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Otter
{
    class AbstractAllocator
    {
    public:
        AbstractAllocator()
            : m_Memory(nullptr), m_MemorySize(0), m_MemoryUsed(0)
        {
        }
        explicit AbstractAllocator(void* memory, const UInt64 memorySize)
            : m_Memory(memory), m_MemorySize(memorySize), m_MemoryUsed(0)
        {
            OTR_INTERNAL_ASSERT_MSG((m_Memory && m_MemorySize) || (!m_Memory && !m_MemorySize),
                                    "Memory and memory size must both be valid")
        }
        virtual ~AbstractAllocator() = default;

        virtual void* Allocate(UInt64 size, UInt64 alignment) = 0;
        virtual void Free(void* block) = 0;

        [[nodiscard]] OTR_INLINE constexpr void* GetMemoryUnsafePointer() const { return m_Memory; }
        [[nodiscard]] OTR_INLINE constexpr UInt64 GetMemorySize() const { return m_MemorySize; }
        [[nodiscard]] OTR_INLINE constexpr UInt64 GetMemoryUsed() const { return m_MemoryUsed; }
        [[nodiscard]] OTR_INLINE constexpr UInt64 GetMemoryFree() const { return m_MemorySize - m_MemoryUsed; }

        [[nodiscard]] OTR_INLINE constexpr virtual UInt64 GetAllocatorHeaderSize() const = 0;
    protected:
        void* m_Memory;
        UInt64 m_MemorySize;
        UInt64 m_MemoryUsed;
    };
}

#endif //OTTERENGINE_ABSTRACTALLOCATOR_H
