#include "Otter.PCH.h"

#include "Core/Allocators/FreeListAllocator.h"

namespace Otter
{
    void* FreeListAllocator::Allocate(const UInt64& size, const UInt64& alignment)
    {
        OTR_INTERNAL_ASSERT_MSG(size > sizeof(Node),
                                "Allocation size must be greater than the size of a Free List node")

        Node* foundNode    = nullptr;
        Node* previousNode = nullptr;
        UInt16 headerPadding = 0;

        switch (m_Policy)
        {
            case Policy::FirstFit:
                foundNode = FindFirstFit(&previousNode, &headerPadding, size, alignment);
                break;
            case Policy::BestFit:
                foundNode = FindBestFit(&previousNode, &headerPadding, size, alignment);
                break;
            default:
            {
                OTR_INTERNAL_ASSERT_MSG(false, "Unknown allocation policy")
                break;
            }
        }

        if (!foundNode)
        {
            OTR_LOG_FATAL("Free list has no free memory! Unable to perform new allocation!")
            return nullptr;
        }

        UInt16 bodyPadding    = headerPadding - OTR_ALIGNED_OFFSET(sizeof(Header), OTR_PLATFORM_MEMORY_ALIGNMENT);
        UInt64 requiredSpace  = size + headerPadding;
        UInt64 remainingSpace = foundNode->m_Size - requiredSpace;

        if (remainingSpace > sizeof(Node))
        {
            Node* nextNode = (Node * )((UIntPtr) foundNode + requiredSpace);
            nextNode->m_Size = remainingSpace;
            nextNode->m_Next = nullptr;
            Insert(nextNode, foundNode);
        }
        Remove(foundNode, previousNode);

        auto* header = (Header * )((UIntPtr*) foundNode + headerPadding);
        header->m_Size = requiredSpace;
        header->m_Padding    = bodyPadding;

        m_MemoryUsed += requiredSpace;

        return (void*) ((UIntPtr) header + sizeof(Header));
    }

    void FreeListAllocator::Free(void* block)
    {
        const UIntPtr headerAddress = (UIntPtr) block - OTR_ALIGNED_OFFSET(sizeof(Header), OTR_PLATFORM_MEMORY_ALIGNMENT);
        const Header* const header = (Header*) headerAddress;

        Node* nodeToInsert = (Node * )(headerAddress);
        nodeToInsert->m_Size = header->m_Size + header->m_Padding;
        nodeToInsert->m_Next = nullptr;

        Node* iteratorCurrent  = m_Head;
        Node* iteratorPrevious = nullptr;
        while (iteratorCurrent)
        {
            if ((UIntPtr) block < (UIntPtr) iteratorCurrent)
            {
                Insert(nodeToInsert, iteratorPrevious);
                break;
            }
            iteratorPrevious = iteratorCurrent;
            iteratorCurrent  = iteratorCurrent->m_Next;
        }

        m_MemoryUsed -= nodeToInsert->m_Size;

        Merge(iteratorPrevious, nodeToInsert);
    }

    void FreeListAllocator::Clear()
    {
        m_MemoryUsed = 0;

        m_Head = (Node*) m_Memory;
        m_Head->m_Size = m_MemorySize;
        m_Head->m_Next = nullptr;
    }

    FreeListAllocator::Node* FreeListAllocator::FindFirstFit(Node** previous,
                                                             UInt16* padding,
                                                             const UInt64& size,
                                                             const UInt16& alignment)
    {
        Node* itCurrent  = m_Head;
        Node* itPrevious = nullptr;
        UIntPtr itPadding = 0;

        while (itCurrent)
        {
            itPadding = GetAlignmentPadding((UIntPtr) itCurrent, alignment);
            UInt64 requiredSpace = size + itPadding;

            if (itCurrent->m_Size >= requiredSpace)
                break;

            itPrevious = itCurrent;
            itCurrent  = itCurrent->m_Next;
        }

        if (padding)
            *padding = itPadding;

        if (previous)
            *previous = itPrevious;

        return itCurrent;
    }

    FreeListAllocator::Node* FreeListAllocator::FindBestFit(Node** previous,
                                                            UInt16* padding,
                                                            const UInt64& size,
                                                            const UInt16& alignment)
    {
        UInt64 smallestDifference = ~(UInt64) 0;

        Node* itCurrent  = m_Head;
        Node* itPrevious = nullptr;
        Node* itBest     = nullptr;
        UIntPtr itPadding = 0;

        while (itCurrent)
        {
            itPadding = GetAlignmentPadding((UIntPtr) itCurrent, alignment);
            UInt64 requiredSpace = size + itPadding;
            UInt64 difference    = itCurrent->m_Size - requiredSpace;

            if (difference == 0)
            {
                itBest = itCurrent;
                break;
            }
            else if (itCurrent->m_Size > requiredSpace && difference < smallestDifference)
            {
                smallestDifference = difference;
                itBest             = itCurrent;
            }

            itPrevious = itCurrent;
            itCurrent  = itCurrent->m_Next;
        }

        if (padding)
            *padding = itPadding;

        if (previous)
            *previous = itPrevious;

        return itBest;
    }

    void FreeListAllocator::Insert(Node* toInsert, Node* previous)
    {
        OTR_INTERNAL_ASSERT_MSG(toInsert, "Node to insert must not be null")

        if (!previous)
        {
            if (m_Head)
                toInsert->m_Next = m_Head;
            else
                m_Head = toInsert;
        }
        else
        {
            if (!previous->m_Next)
            {
                previous->m_Next = toInsert;
                toInsert->m_Next = nullptr;
            }
            else
            {
                toInsert->m_Next = previous->m_Next;
                previous->m_Next = toInsert;
            }
        }
    }

    void FreeListAllocator::Remove(Node* toRemove, Node* previous)
    {
        OTR_INTERNAL_ASSERT_MSG(toRemove, "Node to remove must not be null")

        if (!previous)
            m_Head = toRemove->m_Next;
        else
            previous->m_Next = toRemove->m_Next;
    }

    void FreeListAllocator::Merge(Node* left, Node* right)
    {
        if (right->m_Next && right->m_Size + ((UIntPtr) right) == (UIntPtr) right->m_Next)
        {
            right->m_Size += right->m_Next->m_Size;
            Remove(right, right->m_Next);
        }

        if (left && left->m_Size + ((UIntPtr) left) == (UIntPtr) right)
        {
            left->m_Size += right->m_Size;
            Remove(left, right);
        }
    }

    UIntPtr FreeListAllocator::GetAlignmentPadding(const UIntPtr& address, const UInt16& alignment)
    {
        OTR_INTERNAL_ASSERT_MSG(OTR_IS_POWER_OF_TWO(alignment), "Alignment must be a power of two")

        UIntPtr modulo  = address & (alignment - 1);
        UIntPtr padding = 0;

        if (modulo != 0)
            padding = alignment - modulo;

        UIntPtr requiredSpace = OTR_ALIGNED_OFFSET(sizeof(Header), OTR_PLATFORM_MEMORY_ALIGNMENT);

        if (padding < requiredSpace)
        {
            requiredSpace -= padding;

            if ((requiredSpace & (alignment - 1)) != 0)
                padding += alignment * (1 + (requiredSpace / alignment));
            else
                padding += alignment * (requiredSpace / alignment);
        }

        return padding;
    }
}