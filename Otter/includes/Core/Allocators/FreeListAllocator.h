#ifndef OTTERENGINE_FREELISTALLOCATOR_H
#define OTTERENGINE_FREELISTALLOCATOR_H

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Otter
{
    enum class FreeListAllocationPolicy : UInt8
    {
        FirstFit     = 0,
        BestFit      = 1,
        RedBlackTree = 2
    };

    template<typename OStream>
    OTR_INLINE OStream& operator<<(OStream& os, const Otter::FreeListAllocationPolicy& policy)
    {
        switch (policy)
        {
            case Otter::FreeListAllocationPolicy::FirstFit:
                os << "FreeListAllocationPolicy::FirstFit";
                break;
            case Otter::FreeListAllocationPolicy::BestFit:
                os << "FreeListAllocationPolicy::BestFit";
                break;
            case Otter::FreeListAllocationPolicy::RedBlackTree:
                os << "FreeListAllocationPolicy::RedBlackTree";
                break;
            default:
                os << "FreeListAllocationPolicy[Unknown]";
        }

        return os;
    }

    class FreeListAllocator final
    {
    public:
        OTR_INLINE explicit FreeListAllocator(void* memoryBlock,
                                              const UInt64& memorySize,
                                              const UInt64& memoryAlignment = OTR_DEFAULT_MEMORY_ALIGNMENT,
                                              const FreeListAllocationPolicy& policy = FreeListAllocationPolicy::FirstFit)
            : k_AllocationPolicy(policy), k_MemoryAlignment(memoryAlignment)
        {
            m_Data       = memoryBlock;
            m_MemorySize = memorySize;
            Clear();
        }
        OTR_INLINE ~FreeListAllocator() = default;

        [[nodiscard]] OTR_INLINE FreeListAllocationPolicy GetAllocationPolicy() const { return k_AllocationPolicy; }

        void Clear()
        {
            Node* first = (Node*) m_Data;
            first->m_Size = m_MemorySize;
            first->m_Next = nullptr;

            m_Head       = first;
            m_MemoryUsed = 0;
        }

    private:
        struct Node
        {
            Node* m_Next;
            UInt64 m_Size;
        };

        const FreeListAllocationPolicy k_AllocationPolicy;
        const UInt64                   k_MemoryAlignment;

        Node* m_Head = nullptr;
        void* m_Data = nullptr;
        UInt64 m_MemorySize = 0;
        UInt64 m_MemoryUsed = 0;
    };
}

#endif //OTTERENGINE_FREELISTALLOCATOR_H
