#ifndef OTTERENGINE_FREELISTALLOCATOR_H
#define OTTERENGINE_FREELISTALLOCATOR_H

#include "Core/Defines.h"
#include "Core/Types.h"

#include "Core/Allocators/AbstractAllocator.h"

namespace Otter
{
    class FreeListAllocator final : public AbstractAllocator
    {
        struct Iterator;

    public:
        enum class Policy : UInt8
        {
            FirstFit = 0,
            BestFit  = 1
        };

        FreeListAllocator()
            : AbstractAllocator(), m_Head(nullptr), m_Policy(Policy::FirstFit)
        {
        }
        explicit FreeListAllocator(void* memory,
                                   const UInt64& memorySize,
                                   const Policy& policy = Policy::FirstFit)
            : AbstractAllocator(memory, memorySize), m_Head(nullptr), m_Policy(policy)
        {
            Clear();
        }
        ~FreeListAllocator() final { m_Memory = nullptr; }

        void* Allocate(const UInt64& size, const UInt64& alignment) final;
        void Free(void* block) final;

        void Clear();

        [[nodiscard]] OTR_INLINE constexpr UInt64 GetAllocatorHeaderSize() const final
        {
            return sizeof(Header);
        }
        [[nodiscard]] OTR_INLINE constexpr Policy GetAllocationPolicy() const { return m_Policy; }

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

        [[nodiscard]] OTR_INLINE Iterator begin() const noexcept { return Iterator(m_Head); }
        [[nodiscard]] OTR_INLINE Iterator end() const noexcept { return Iterator(nullptr); }

#pragma clang diagnostic pop

    private:
        struct Header
        {
            UInt64 Size;
            UInt16 Padding;
        };

        struct Node
        {
            UInt64 Size;
            Node* Next;
        };

        struct Iterator
        {
        public:
            explicit Iterator(Node* node)
                : m_Node(node)
            {
            }

            OTR_INLINE Iterator& operator++()
            {
                m_Node = m_Node->Next;
                return *this;
            }

            OTR_INLINE const Iterator operator++(int)
            {
                Iterator iterator = *this;
                ++(*this);
                return iterator;
            }

            OTR_INLINE Node& operator*() { return *m_Node; }
            OTR_INLINE bool operator==(const Iterator& other) const { return m_Node == other.m_Node; }
            OTR_INLINE bool operator!=(const Iterator& other) const { return !(*this == other); }

        private:
            Node* m_Node;
        };

        Node* m_Head;
        Policy m_Policy;

        Node* FindFirstFit(Node** previous, UInt16* padding, const UInt64& size, const UInt16& alignment);
        Node* FindBestFit(Node** previous, UInt16* padding, const UInt64& size, const UInt16& alignment);
        void Insert(Node* node, Node* previous);
        void Remove(Node* node, Node* previous);
        void Merge(Node* toMerge, Node* previousNode);

        static UIntPtr GetAlignmentPadding(const UIntPtr& address, const UInt16& alignment);
    };

    template<typename OStream>
    OStream& operator<<(OStream& os, const Otter::FreeListAllocator::Policy& policy)
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
