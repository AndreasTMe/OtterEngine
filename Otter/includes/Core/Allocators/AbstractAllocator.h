#ifndef OTTERENGINE_ABSTRACTALLOCATOR_H
#define OTTERENGINE_ABSTRACTALLOCATOR_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"

namespace Otter
{
    /**
     * @brief Abstract allocator class
     */
    class AbstractAllocator
    {
    public:
        /**
         * @brief Default constructor
         */
        AbstractAllocator()
            : m_Memory(nullptr), m_MemorySize(0), m_MemoryUsed(0)
        {
        }

        /**
         * @brief Constructor
         *
         * @param memory Pointer to a memory block
         * @param memorySize Memory size
         */
        explicit AbstractAllocator(void* memory, const UInt64 memorySize)
            : m_Memory(memory), m_MemorySize(memorySize), m_MemoryUsed(0)
        {
            OTR_INTERNAL_ASSERT_MSG((m_Memory && m_MemorySize) || (!m_Memory && !m_MemorySize),
                                    "Memory and memory size must both be valid")
        }

        /**
         * @brief Destructor
         */
        virtual ~AbstractAllocator() = default;

        /**
         * @brief Allocates a memory block
         *
         * @param size Size of the memory to allocate
         * @param alignment Alignment of the memory to allocate
         *
         * @return Pointer to the allocated memory block
         */
        [[nodiscard]] virtual void* Allocate(UInt64 size, UInt16 alignment) = 0;

        /**
         * @brief Frees a memory block
         *
         * @param block Pointer to the memory block to free
         */
        virtual void Free(void* block) = 0;

        /**
         * @brief Retrieves the memory footprint of a memory block in the allocator
         *
         * @param block Pointer to the memory block
         * @param outSize The size of the memory block inside the allocator
         * @param outOffset The offset of the memory block from the start of the allocator
         * @param outPadding The padding of the memory block
         * @param outAlignment The alignment of the memory block
         */
        virtual void GetMemoryFootprint(const void* block,
                                        UInt64* outSize,
                                        UInt64* outOffset,
                                        UInt16* outPadding,
                                        UInt16* outAlignment) const = 0;

        /**
         * @brief Retrieves the memory size of the allocator
         *
         * @return Memory size of the allocator
         */
        [[nodiscard]] OTR_INLINE constexpr UInt64 GetMemorySize() const { return m_MemorySize; }

        /**
         * @brief Retrieves the used memory of the allocator
         *
         * @return Used memory of the allocator
         */
        [[nodiscard]] OTR_INLINE constexpr UInt64 GetMemoryUsed() const { return m_MemoryUsed; }

        /**
         * @brief Retrieves the free memory of the allocator
         *
         * @return Free memory of the allocator
         */
        [[nodiscard]] OTR_INLINE constexpr UInt64 GetMemoryFree() const { return m_MemorySize - m_MemoryUsed; }

    protected:
        void* m_Memory;
        UInt64 m_MemorySize;
        UInt64 m_MemoryUsed;

    private:
        /**
         * @brief Retrieves the pointer to the memory block of the allocator
         *
         * @return Pointer to the memory block
         */
        [[nodiscard]] OTR_INLINE constexpr void* GetMemoryUnsafePointer() const { return m_Memory; }

        friend class MemorySystem;
    };
}

#endif //OTTERENGINE_ABSTRACTALLOCATOR_H
