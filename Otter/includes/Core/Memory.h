#ifndef OTTERENGINE_MEMORY_H
#define OTTERENGINE_MEMORY_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"

#include "Core/Allocators/FreeListAllocator.h"

namespace Otter
{
    struct UnsafeHandle
    {
        void* Pointer;
        UInt64 Size;
    };

    class MemorySystem final
    {
    public:
        OTR_DISABLE_OBJECT_COPIES(MemorySystem)
        OTR_DISABLE_OBJECT_MOVES(MemorySystem)

        static void Initialise(UInt64 memoryRequirements);
        static void Shutdown();

        static UnsafeHandle Allocate(UInt64 size, UInt64 alignment = OTR_PLATFORM_MEMORY_ALIGNMENT);
        static UnsafeHandle Reallocate(UnsafeHandle& handle,
                                       UInt64 size,
                                       UInt64 alignment = OTR_PLATFORM_MEMORY_ALIGNMENT);
        static void Free(void* block);
        static void MemoryCopy(void* destination, const void* source, UInt64 size);
        static void MemoryClear(void* block, UInt64 size);

        [[nodiscard]] static constexpr UInt64 GetUsedMemory() { return s_Allocator.GetMemoryUsed(); }
        [[nodiscard]] static constexpr UInt64 GetFreeMemory() { return s_Allocator.GetMemoryFree(); }
        [[nodiscard]] static constexpr UInt64 GetMemorySize() { return s_Allocator.GetMemorySize(); }

    private:
        OTR_DISABLE_CONSTRUCTION(MemorySystem)

        static bool              s_HasInitialised;
        static FreeListAllocator s_Allocator;
    };

    template<typename T, typename... TArgs>
    OTR_INLINE T* New(TArgs&& ... args)
    {
        UInt64 alignedSize = OTR_ALIGNED_OFFSET(sizeof(T), OTR_PLATFORM_MEMORY_ALIGNMENT);

        UnsafeHandle handle = MemorySystem::Allocate(alignedSize);
        T* ptr = ::new(handle.Pointer) T(args...);

        return ptr;
    }

    template<typename T>
    OTR_INLINE void Delete(T* ptr)
    {
        if (!std::is_trivially_destructible_v<T> && ptr != nullptr)
            ptr->~T();

        MemorySystem::MemoryClear(ptr, sizeof(T));
        MemorySystem::Free(ptr);
    }

    class Buffer final
    {
    public:
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

    class Unsafe final
    {
    public:
        OTR_INLINE static UnsafeHandle New(const UInt64 size)
        {
            OTR_INTERNAL_ASSERT_MSG(size > 0, "Allocation size must be greater than 0 bytes")

            UInt64 alignedSize = OTR_ALIGNED_OFFSET(size, OTR_PLATFORM_MEMORY_ALIGNMENT);

            return MemorySystem::Allocate(alignedSize);
        }

        OTR_INLINE static void Delete(const UnsafeHandle& handle)
        {
            OTR_INTERNAL_ASSERT_MSG(handle.Pointer != nullptr, "Handle pointer must not be null")
            OTR_INTERNAL_ASSERT_MSG(handle.Size > 0, "Handle size must be greater than 0")

            MemorySystem::MemoryClear(handle.Pointer, handle.Size);
            MemorySystem::Free(handle.Pointer);
        }

        OTR_INLINE static void Delete(UnsafeHandle&& handle)
        {
            OTR_INTERNAL_ASSERT_MSG(handle.Pointer != nullptr, "Handle pointer must not be null")
            OTR_INTERNAL_ASSERT_MSG(handle.Size > 0, "Handle size must be greater than 0")

            MemorySystem::MemoryClear(handle.Pointer, handle.Size);
            MemorySystem::Free(handle.Pointer);
        }
    };
}

#endif //OTTERENGINE_MEMORY_H
