#ifndef OTTERENGINE_MEMORY_H
#define OTTERENGINE_MEMORY_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"

namespace Otter
{
    class Memory
    {
    public:
        struct Handle
        {
            void* m_Pointer;

            template<typename T, typename... TArgs>
            OTR_INLINE T* As(TArgs... args)
            {
                new(m_Pointer) T(args...);
                return (T*) m_Pointer;
            }
        };

        OTR_DISABLE_OBJECT_COPIES(Memory)
        OTR_DISABLE_OBJECT_MOVES(Memory)

        OTR_INLINE static Memory* GetInstance()
        {
            static Memory instance;
            return &instance;
        }

        Handle Allocate(UInt64 size, UInt64 alignment = OTR_DEFAULT_MEMORY_ALIGNMENT);
        Handle Reallocate(Handle& handle,
                          UInt64 size,
                          UInt64 alignment = OTR_DEFAULT_MEMORY_ALIGNMENT);
        void Free(void* block);
        void MemoryClear(void* block, UInt64 size);
        void MemoryCopy(void* dest, const void* src, UInt64 size);

        [[nodiscard]] OTR_INLINE static UInt64 GetTotalAllocation() { return s_TotalAllocation; }

        // TODO: Capture total allocation differently
        template<typename T>
        OTR_INLINE static void UpdateTotalAllocation(Size size, bool isAllocating = true)
        {
            if (isAllocating)
                s_TotalAllocation += size;
            else
                s_TotalAllocation -= size;

            OTR_LOG_INFO("{0} => Size of {1} bytes\n\t(Total allocation: {2} bytes)",
                         TypeOf<T>::GetName(),
                         sizeof(T),
                         s_TotalAllocation)
        }

    private:
        OTR_WITH_DEFAULT_CONSTRUCTOR(Memory)

        static UInt64 s_TotalAllocation;
    };

    template<typename T, typename... TArgs>
    OTR_INLINE T* New(TArgs&& ... args)
    {
        Memory::UpdateTotalAllocation<T>(sizeof(T));

        return Memory::GetInstance()->Allocate(sizeof(T))
            .As<T>(std::forward<TArgs>(args)...);
    }

    template<typename T>
    OTR_INLINE void Delete(T* ptr)
    {
        Memory::UpdateTotalAllocation<T>(sizeof(T), false);

        if (!std::is_trivially_destructible<T>::value)
            ptr->~T();

        Memory::GetInstance()->Free(ptr);
    }

    template<typename T>
    OTR_INLINE T* NewBuffer(UInt64 length)
    {
        OTR_INTERNAL_ASSERT_MSG(length * sizeof(T) > 0, "Buffer length must be greater than 0")

        Memory::UpdateTotalAllocation<T>(length * sizeof(T));

        return Memory::GetInstance()->Allocate(length * sizeof(T))
            .As<T>();
    }

    template<typename T>
    OTR_INLINE void DeleteBuffer(T* ptr, UInt64 length)
    {
        OTR_INTERNAL_ASSERT_MSG(length * sizeof(T) > 0, "Buffer length must be greater than 0")

        if (!std::is_trivially_destructible<T>::value)
        {
            T* ptrCopy = ptr;
            while (length--)
            {
                ptrCopy->~T();
                ++ptrCopy;
            }
        }

        Memory::GetInstance()->Free(ptr);

        Memory::UpdateTotalAllocation<T>(length * sizeof(T), false);
    }
}

#endif //OTTERENGINE_MEMORY_H
