#ifndef OTTERENGINE_FREELISTALLOCATOR_H
#define OTTERENGINE_FREELISTALLOCATOR_H

#include "Core/Defines.h"
#include "Core/Types.h"

#include "Core/Allocators/AbstractAllocator.h"

namespace Otter
{
    class FreeListAllocator final : public AbstractAllocator
    {
    public:
        enum class Policy : UInt8
        {
            FirstFit = 0,
            BestFit  = 1
        };

        OTR_INLINE FreeListAllocator()
            : AbstractAllocator(), m_Head(nullptr), m_Policy(Policy::FirstFit)
        {
        }
        OTR_INLINE explicit FreeListAllocator(void* memory,
                                              const UInt64& memorySize,
                                              const Policy& policy = Policy::FirstFit)
            : AbstractAllocator(memory, memorySize), m_Head(nullptr), m_Policy(Policy::FirstFit)
        {
            Clear();
        }
        OTR_INLINE ~FreeListAllocator() final { m_Memory = nullptr; }

        void* Allocate(const UInt64& size, const UInt64& alignment) final;
        void Free(void* block) final;

        void Clear();

        [[nodiscard]] OTR_INLINE Policy GetAllocationPolicy() const { return m_Policy; }

    private:
        struct Header
        {
            UInt64 m_Size;
            UInt16 m_Padding;
        };

        struct Node
        {
            UInt64 m_Size;
            Node* m_Next;
        };

        Node* m_Head;
        Policy m_Policy;

        Node* FindFirstFit(Node** previous, UInt16* padding, const UInt64& size, const UInt16& alignment);
        Node* FindBestFit(Node** previous, UInt16* padding, const UInt64& size, const UInt16& alignment);
        void Insert(Node* node, Node* previous);
        void Remove(Node* node, Node* previous);
        void Merge(Node* left, Node* right);

        static UIntPtr GetAlignmentPadding(const UIntPtr& address, const UInt16& alignment);
    };

    template<typename OStream>
    OTR_INLINE OStream& operator<<(OStream& os, const Otter::FreeListAllocator::Policy& policy)
    {
        switch (policy)
        {
            case Otter::FreeListAllocator::Policy::FirstFit:
                os << "FreeListAllocator::Policy::FirstFit";
                break;
            case Otter::FreeListAllocator::Policy::BestFit:
                os << "FreeListAllocator::Policy::BestFit";
                break;
            default:
                os << "FreeListAllocator::Policy[Unknown]";
        }

        return os;
    }
}

#endif //OTTERENGINE_FREELISTALLOCATOR_H
