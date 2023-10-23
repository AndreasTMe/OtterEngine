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
        void* m_Pointer;
        UInt64 m_Size;
    };

    class Memory
    {
    public:
        OTR_DISABLE_OBJECT_COPIES(Memory)
        OTR_DISABLE_OBJECT_MOVES(Memory)

        OTR_INLINE static Memory* GetInstance()
        {
            static Memory instance;
            return &instance;
        }

        void Initialise();
        void Shutdown();

        UnsafeHandle Allocate(const UInt64& size, const UInt64& alignment = OTR_PLATFORM_MEMORY_ALIGNMENT);
        UnsafeHandle Reallocate(UnsafeHandle& handle,
                                const UInt64& size,
                                const UInt64& alignment = OTR_PLATFORM_MEMORY_ALIGNMENT);
        void Free(void* block);
        void MemoryClear(void* block, const UInt64& size);

        [[nodiscard]] OTR_INLINE const UInt64 GetTotalAllocation() { return m_Allocator.GetMemoryUsed(); }

    private:
        OTR_WITH_DEFAULT_CONSTRUCTOR(Memory)

        bool              m_HasInitialised = false;
        FreeListAllocator m_Allocator;
    };

    template<typename T, typename... TArgs>
    OTR_INLINE T* New(TArgs&& ... args)
    {
        UInt64 alignedSize = OTR_ALIGNED_OFFSET(sizeof(T), OTR_PLATFORM_MEMORY_ALIGNMENT);

        UnsafeHandle handle = Memory::GetInstance()->Allocate(alignedSize);
        T* ptr = new(handle.m_Pointer) T(args...);

        return ptr;
    }

    template<typename T>
    OTR_INLINE void Delete(T* ptr)
    {
        if (!std::is_trivially_destructible<T>::value)
            ptr->~T();

        Memory::GetInstance()->MemoryClear(ptr, sizeof(T));
        Memory::GetInstance()->Free(ptr);
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

            UnsafeHandle handle = Memory::GetInstance()->Allocate(bufferSize);
            T* buffer = new(handle.m_Pointer) T();

            return buffer;
        }

        template<typename T>
        OTR_INLINE static void Delete(T* ptr, const UInt64& length)
        {
            OTR_INTERNAL_ASSERT_MSG(length * sizeof(T) > 0, "Buffer length must be greater than 0")

            if (!std::is_trivially_destructible<T>::value)
            {
                T* ptrCopy = ptr;
                UInt64 lengthCopy = length;
                while (lengthCopy--)
                {
                    ptrCopy->~T();
                    ++ptrCopy;
                }
            }

            Memory::GetInstance()->MemoryClear(ptr, length * sizeof(T));
            Memory::GetInstance()->Free(ptr);
        }
    };

    class Unsafe final
    {
    public:
        OTR_INLINE static UnsafeHandle New(const UInt64& size)
        {
            OTR_INTERNAL_ASSERT_MSG(size > 0, "Allocation size must be greater than 0 bytes")

            UInt64 alignedSize = OTR_ALIGNED_OFFSET(size, OTR_PLATFORM_MEMORY_ALIGNMENT);

            UnsafeHandle handle{ };
            handle.m_Pointer = Memory::GetInstance()->Allocate(alignedSize).m_Pointer;
            handle.m_Size    = alignedSize;

            return handle;
        }

        OTR_INLINE static void Delete(UnsafeHandle handle)
        {
            Memory::GetInstance()->MemoryClear(handle.m_Pointer, handle.m_Size);
            Memory::GetInstance()->Free(handle.m_Pointer);
        }
    };
}

OTR_WITH_TYPENAME(Otter::UnsafeHandle)

#endif //OTTERENGINE_MEMORY_H
