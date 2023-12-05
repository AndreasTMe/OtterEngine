#ifndef OTTERENGINE_MEMORY_H
#define OTTERENGINE_MEMORY_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Core/Function.h"

#include "Core/Allocators/FreeListAllocator.h"
#include "Core/Allocators/MemoryFootprint.h"

#define OTR_ALLOCATED_MEMORY(type, count)                                       \
    count * OTR_ALIGNED_OFFSET(sizeof(type), OTR_PLATFORM_MEMORY_ALIGNMENT) +   \
    Otter::FreeListAllocator::GetAllocatorHeaderSize()

namespace Otter
{
    struct UnsafeHandle final
    {
        void* Pointer;
        UInt64 Size;
    };

    class MemorySystem final
    {
    public:
        OTR_DISABLE_OBJECT_COPIES(MemorySystem)
        OTR_DISABLE_OBJECT_MOVES(MemorySystem)

        OTR_INLINE static MemorySystem& GetInstance()
        {
            static MemorySystem instance;
            return instance;
        }

        void Initialise(UInt64 memoryRequirements);
        void Shutdown();

        UnsafeHandle Allocate(UInt64 size, UInt16 alignment = OTR_PLATFORM_MEMORY_ALIGNMENT);
        UnsafeHandle Reallocate(UnsafeHandle& handle,
                                UInt64 size,
                                UInt16 alignment = OTR_PLATFORM_MEMORY_ALIGNMENT);
        void Free(void* block);
        void MemoryCopy(void* destination, const void* source, UInt64 size);
        void MemoryClear(void* block, UInt64 size);

        void CheckMemoryFootprint(const Function<MemoryDebugHandle()>& callback,
                                  MemoryFootprint* outFootprints,
                                  UInt64* outFootprintCount);

        [[nodiscard]] constexpr UInt64 GetUsedMemory() { return m_Allocator.GetMemoryUsed(); }
        [[nodiscard]] constexpr UInt64 GetFreeMemory() { return m_Allocator.GetMemoryFree(); }
        [[nodiscard]] constexpr UInt64 GetMemorySize() { return m_Allocator.GetMemorySize(); }

        [[nodiscard]] const FreeListAllocator* GetAllocator() const { return &m_Allocator; }

    private:
        OTR_WITH_DEFAULT_CONSTRUCTOR(MemorySystem)

        bool              m_HasInitialised = false;
        FreeListAllocator m_Allocator;
    };

#define OTR_MEMORY_SYSTEM Otter::MemorySystem::GetInstance()

    template<typename T, typename... TArgs>
    OTR_INLINE T* New(TArgs&& ... args)
    {
        UInt64 alignedSize = OTR_ALIGNED_OFFSET(sizeof(T), OTR_PLATFORM_MEMORY_ALIGNMENT);

        UnsafeHandle handle = OTR_MEMORY_SYSTEM.Allocate(alignedSize);
        T* ptr = ::new(handle.Pointer) T(args...);

        return ptr;
    }

    template<typename T>
    OTR_INLINE void Delete(T* ptr)
    {
        if (!std::is_trivially_destructible_v<T> && ptr != nullptr)
            ptr->~T();

        OTR_MEMORY_SYSTEM.MemoryClear(ptr, sizeof(T));
        OTR_MEMORY_SYSTEM.Free(ptr);
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

            UnsafeHandle handle = OTR_MEMORY_SYSTEM.Allocate(bufferSize);

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

            OTR_MEMORY_SYSTEM.MemoryClear(ptr, length * sizeof(T));
            OTR_MEMORY_SYSTEM.Free(ptr);
        }
    };

    class Unsafe final
    {
    public:
        OTR_INLINE static UnsafeHandle New(const UInt64 size)
        {
            OTR_INTERNAL_ASSERT_MSG(size > 0, "Allocation size must be greater than 0 bytes")

            UInt64 alignedSize = OTR_ALIGNED_OFFSET(size, OTR_PLATFORM_MEMORY_ALIGNMENT);

            return OTR_MEMORY_SYSTEM.Allocate(alignedSize);
        }

        OTR_INLINE static void Delete(const UnsafeHandle& handle)
        {
            OTR_INTERNAL_ASSERT_MSG(handle.Pointer != nullptr, "Handle pointer must not be null")
            OTR_INTERNAL_ASSERT_MSG(handle.Size > 0, "Handle size must be greater than 0")

            OTR_MEMORY_SYSTEM.MemoryClear(handle.Pointer, handle.Size);
            OTR_MEMORY_SYSTEM.Free(handle.Pointer);
        }
    };
}

#endif //OTTERENGINE_MEMORY_H
