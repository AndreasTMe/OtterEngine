#ifndef OTTERENGINE_MEMORY_H
#define OTTERENGINE_MEMORY_H

#include "Core/Defines.h"
#include "Core/BaseTypes.h"
#include "Core/Assert.h"
#include "Core/Function.h"

#include "Core/Allocators/FreeListAllocator.h"
#include "Core/Allocators/MemoryFootprint.h"

#define OTR_ALLOCATED_MEMORY(type, count)                                       \
    count * OTR_ALIGNED_OFFSET(sizeof(type), OTR_PLATFORM_MEMORY_ALIGNMENT) +   \
    Otter::FreeListAllocator::GetAllocatorHeaderSize()

namespace Otter
{
    /**
     * @brief Represents an unsafe handle to a resource and encapsulates a void pointer and the size of the resource.
     */
    struct UnsafeHandle final
    {
        void* Pointer;
        UInt64 Size;
    };

    /**
     * @brief The application's memory system that manages the allocation and de-allocation of memory.
     */
    class MemorySystem final
    {
    public:
        /**
         * @brief Deleted copy constructor.
         */
        MemorySystem(MemorySystem&) = delete;

        /**
         * @brief Deleted copy constructor.
         */
        MemorySystem(const MemorySystem&) = delete;

        /**
         * @brief Deleted copy assignment operator.
         */
        MemorySystem& operator=(const MemorySystem&) = delete;

        /**
         * @brief Deleted move constructor.
         */
        MemorySystem(MemorySystem&&) = delete;

        /**
         * @brief Deleted move assignment operator.
         */
        MemorySystem& operator=(MemorySystem&&) = delete;

        /**
         * @brief Initialises the memory system.
         *
         * @param memoryRequirements The amount of memory in bytes to allocate for the memory system.
         */
        static void Initialise(UInt64 memoryRequirements);

        /**
         * @brief Shuts down the memory system.
         */
        static void Shutdown();

        /**
         * @brief Allocates a block of memory.
         *
         * @param size The size of the memory block to allocate in bytes.
         * @param alignment The alignment of the memory block to allocate in bytes.
         *
         * @return An unsafe handle to the allocated memory block.
         */
        static UnsafeHandle Allocate(UInt64 size, UInt16 alignment = OTR_PLATFORM_MEMORY_ALIGNMENT);

        /**
         * @brief Reallocates a block of memory.
         *
         * @param handle The unsafe handle to the memory block to reallocate.
         * @param size The size of the memory block to reallocate in bytes.
         * @param alignment The alignment of the memory block to reallocate in bytes.
         *
         * @return An unsafe handle to the reallocated memory block.
         */
        static UnsafeHandle Reallocate(UnsafeHandle& handle,
                                       UInt64 size,
                                       UInt16 alignment = OTR_PLATFORM_MEMORY_ALIGNMENT);

        /**
         * @brief Frees a block of memory.
         *
         * @param block The block of memory to free.
         */
        static void Free(void* block);

        /**
         * @brief Copies a block of memory from one location to another.
         *
         * @param destination The destination of the memory copy.
         * @param source The source of the memory copy.
         * @param size The size of the memory copy in bytes.
         *
         * @note Copying overlapping memory blocks with this function is undefined behaviour.
         */
        static void MemoryCopy(void* destination, const void* source, UInt64 size);

        /**
         * @brief Copies a block of memory from one location to another.
         *
         * @param destination The destination of the memory copy.
         * @param source The source of the memory copy.
         * @param size The size of the memory copy in bytes.
         *
         * @note Allows copying overlapping memory blocks.
         */
        static void MemoryMove(void* destination, const void* source, UInt64 size);

        /**
         * @brief Clears a block of memory.
         *
         * @param block The block of memory to clear.
         * @param size The size of the memory block to clear in bytes.
         */
        static void MemoryClear(void* block, UInt64 size);

        /**
         * @brief Checks the memory footprint of a resource.
         *
         * @param callback The callback to invoke to get the memory footprint of a resource.
         * @param outFootprints The memory footprints of the resource.
         * @param outFootprintCount The number of memory footprints of the resource.
         */
        static void CheckMemoryFootprint(const Function<MemoryDebugHandle()>& callback,
                                         MemoryFootprint* outFootprints,
                                         UInt64* outFootprintCount);

        /**
         * @brief Gets the amount of memory used by the memory system.
         *
         * @return The amount of memory used by the memory system.
         */
        [[nodiscard]] static constexpr UInt64 GetUsedMemory() { return s_Allocator.GetMemoryUsed(); }

        /**
         * @brief Gets the amount of free memory available to the memory system.
         *
         * @return The amount of free memory available to the memory system.
         */
        [[nodiscard]] static constexpr UInt64 GetFreeMemory() { return s_Allocator.GetMemoryFree(); }

        /**
         * @brief Gets the total amount of memory available to the memory system.
         *
         * @return The total amount of memory available to the memory system.
         */
        [[nodiscard]] static constexpr UInt64 GetMemorySize() { return s_Allocator.GetMemorySize(); }

    private:
        /**
         * @brief Constructor.
         */
        MemorySystem() = default;

        /**
         * @brief Destructor.
         */
        ~MemorySystem() = default;

        static bool              s_HasInitialised;
        static FreeListAllocator s_Allocator;
    };

    /**
     * @brief Allocates a block of memory for a type.
     *
     * @tparam T The type to allocate memory for.
     * @tparam TArgs The arguments to pass to the type's constructor.
     *
     * @param args The arguments to pass to the type's constructor.
     *
     * @return A pointer to the allocated memory block.
     */
    template<typename T, typename... TArgs>
    OTR_INLINE T* New(TArgs&& ... args)
    {
        UInt64 alignedSize = OTR_ALIGNED_OFFSET(sizeof(T), OTR_PLATFORM_MEMORY_ALIGNMENT);

        UnsafeHandle handle = MemorySystem::Allocate(alignedSize);
        T* ptr = ::new(handle.Pointer) T(args...);

        return ptr;
    }

    /**
     * @brief Deallocates a block of memory for a type.
     *
     * @tparam T The type to deallocate memory for.
     *
     * @param ptr The pointer to the memory block to deallocate.
     */
    template<typename T>
    OTR_INLINE void Delete(T* ptr)
    {
        if (!std::is_trivially_destructible_v<T> && ptr != nullptr)
            ptr->~T();

        MemorySystem::MemoryClear(ptr, sizeof(T));
        MemorySystem::Free(ptr);
    }

    /**
     * @brief Used to allocate and deallocate a buffer of memory for a type.
     */
    class Buffer final
    {
    public:
        /**
         * @brief Allocates a buffer of memory for a type.
         *
         * @tparam T The type to allocate memory for.
         * @param length The number of elements to allocate memory for.
         *
         * @return A pointer to the allocated memory block.
         */
        template<typename T>
        OTR_INLINE static T* New(const UInt64 length)
        {
            OTR_INTERNAL_ASSERT_MSG(length * sizeof(T) > 0, "Buffer length must be greater than 0")

            UInt64 alignedSize = OTR_ALIGNED_OFFSET(sizeof(T), OTR_PLATFORM_MEMORY_ALIGNMENT);
            UInt64 bufferSize  = length * alignedSize;

            UnsafeHandle handle = MemorySystem::Allocate(bufferSize);

            if (!std::is_trivially_constructible<T>::value)
            {
                T* ptrCopy = (T*) handle.Pointer;
                for (UInt64 i = 0; i < length; i++)
                {
                    ::new(ptrCopy) T();
                    ++ptrCopy;
                }
            }

            return (T*) handle.Pointer;
        }

        /**
         * @brief Deallocates a buffer of memory for a type.
         *
         * @tparam T The type to deallocate memory for.
         *
         * @param ptr The pointer to the memory block to deallocate.
         * @param length The number of elements to deallocate memory for.
         */
        template<typename T>
        OTR_INLINE static void Delete(T* ptr, const UInt64 length)
        {
            OTR_INTERNAL_ASSERT_MSG(ptr != nullptr, "Buffer pointer must not be null")
            OTR_INTERNAL_ASSERT_MSG(length * sizeof(T) > 0, "Buffer length must be greater than 0")

            if (!std::is_trivially_destructible_v<T>)
            {
                T* ptrCopy = ptr;
                for (UInt64 i = 0; i < length; i++)
                {
                    if (ptrCopy != nullptr)
                        ptrCopy->~T();

                    ++ptrCopy;
                }
            }

            MemorySystem::MemoryClear(ptr, length * sizeof(T));
            MemorySystem::Free(ptr);
        }
    };

    /**
     * @brief Used to allocate and deallocate a handle to a resource.
     */
    class Unsafe final
    {
    public:
        /**
         * @brief Allocates a handle to a resource.
         *
         * @param size The size of the resource to allocate in bytes.
         *
         * @return An unsafe handle to the allocated resource.
         */
        OTR_INLINE static UnsafeHandle New(const UInt64 size)
        {
            OTR_INTERNAL_ASSERT_MSG(size > 0, "Allocation size must be greater than 0 bytes")

            UInt64 alignedSize = OTR_ALIGNED_OFFSET(size, OTR_PLATFORM_MEMORY_ALIGNMENT);

            return MemorySystem::Allocate(alignedSize);
        }

        /**
         * @brief Deallocates a handle to a resource.
         *
         * @param handle The unsafe handle to the resource to deallocate.
         */
        OTR_INLINE static void Delete(const UnsafeHandle& handle)
        {
            OTR_INTERNAL_ASSERT_MSG(handle.Pointer != nullptr, "Handle pointer must not be null")
            OTR_INTERNAL_ASSERT_MSG(handle.Size > 0, "Handle size must be greater than 0")

            MemorySystem::MemoryClear(handle.Pointer, OTR_ALIGNED_OFFSET(handle.Size, OTR_PLATFORM_MEMORY_ALIGNMENT));
            MemorySystem::Free(handle.Pointer);
        }
    };
}

#endif //OTTERENGINE_MEMORY_H
