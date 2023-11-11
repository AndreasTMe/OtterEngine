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

    namespace MemorySystem
    {
        void Initialise(const UInt64& memoryRequirements);
        void Shutdown();

        UnsafeHandle Allocate(const UInt64& size, const UInt64& alignment = OTR_PLATFORM_MEMORY_ALIGNMENT);
        UnsafeHandle Reallocate(UnsafeHandle& handle,
                                const UInt64& size,
                                const UInt64& alignment = OTR_PLATFORM_MEMORY_ALIGNMENT);
        void Free(void* block);
        void MemoryClear(void* block, const UInt64& size);

        // TODO: Will probably be removed
        [[nodiscard]] std::string GetTotalAllocation();
    }

    template<typename T, typename... TArgs>
    OTR_INLINE T* New(TArgs&& ... args)
    {
        UInt64 alignedSize = OTR_ALIGNED_OFFSET(sizeof(T), OTR_PLATFORM_MEMORY_ALIGNMENT);

        UnsafeHandle handle = MemorySystem::Allocate(alignedSize);
        T* ptr = new(handle.Pointer) T(args...);

        return ptr;
    }

    template<typename T>
    OTR_INLINE void Delete(T* ptr)
    {
        if (!std::is_destructible<T>::value)
            ptr->~T();

        MemorySystem::MemoryClear(ptr, sizeof(T));
        MemorySystem::Free(ptr);
    }

    class Buffer final
    {
    public:
        template<typename T>
        OTR_INLINE static T* New(const UInt64& length)
        {
            OTR_INTERNAL_ASSERT_MSG(length * sizeof(T) > 0, "Buffer length must be greater than 0")

            UInt64 alignedSize = OTR_ALIGNED_OFFSET(sizeof(T), OTR_PLATFORM_MEMORY_ALIGNMENT);
            UInt64 bufferSize  = length * alignedSize;

            UnsafeHandle handle = MemorySystem::Allocate(bufferSize);

            if (std::is_default_constructible<T>::value)
            {
                T* ptrCopy = (T*) handle.Pointer;
                for (UInt64 i = 0; i < length; i++)
                {
                    new(ptrCopy) T();
                    ++ptrCopy;
                }
            }

            return (T*) handle.Pointer;
        }

        template<typename T>
        OTR_INLINE static void Delete(T* ptr, const UInt64& length)
        {
            OTR_INTERNAL_ASSERT_MSG(ptr != nullptr, "Buffer pointer must not be null")
            OTR_INTERNAL_ASSERT_MSG(length * sizeof(T) > 0, "Buffer length must be greater than 0")

            if (!std::is_destructible<T>::value)
            {
                T* ptrCopy = ptr;
                for (UInt64 i = 0; i < length; i++)
                {
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
        OTR_INLINE static UnsafeHandle New(const UInt64& size)
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

OTR_WITH_TYPENAME(Otter::UnsafeHandle)

#endif //OTTERENGINE_MEMORY_H
