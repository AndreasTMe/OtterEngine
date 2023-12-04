#include "Otter.PCH.h"

#include "Core/Allocators/FreeListAllocator.h"

namespace Otter
{
    void* FreeListAllocator::Allocate(const UInt64 size, const UInt16 alignment)
    {
        if (size < sizeof(Node))
        {
            OTR_LOG_WARNING("Allocation size is less than or equal to the size of a Free List node ({0} < {1})."
                            "\n\tConsider using a different allocator for this.", size, sizeof(Node))
        }

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
        UInt64 remainingSpace = foundNode->Size - requiredSpace;

        if (remainingSpace > sizeof(Node))
        {
            Node* nextNode = (Node * )((UIntPtr) foundNode + requiredSpace);
            nextNode->Size = remainingSpace;
            nextNode->Next = nullptr;
            Insert(nextNode, foundNode);
        }
        Remove(foundNode, previousNode);

        auto* header = (Header * )((UIntPtr*) foundNode + bodyPadding);
        header->Size    = requiredSpace;
        header->Padding = bodyPadding;

        m_MemoryUsed += requiredSpace;

        return (void*) ((UIntPtr) header + sizeof(Header));
    }

    void FreeListAllocator::Free(void* block)
    {
        const UIntPtr headerAddress =
                          (UIntPtr) block - OTR_ALIGNED_OFFSET(sizeof(Header), OTR_PLATFORM_MEMORY_ALIGNMENT);
        const Header* const header = (Header*) headerAddress;

        Node* nodeToInsert = (Node * )(headerAddress);
        nodeToInsert->Size = header->Size + header->Padding;
        nodeToInsert->Next = nullptr;

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
            iteratorCurrent = iteratorCurrent->Next;
        }

        m_MemoryUsed -= nodeToInsert->Size;

        Merge(nodeToInsert, iteratorPrevious);
    }

    void FreeListAllocator::Clear()
    {
        m_MemoryUsed = 0;

        m_Head = (Node*) m_Memory;
        m_Head->Size = m_MemorySize;
        m_Head->Next = nullptr;
    }

    void FreeListAllocator::GetMemoryFootprint(const void* const block,
                                               UInt64* outSize,
                                               UInt64* outOffset,
                                               UInt16* outPadding,
                                               UInt16* outAlignment) const
    {
        OTR_INTERNAL_ASSERT_MSG(block != nullptr, "Block must not be null")

        const UIntPtr headerAddress =
                          (UIntPtr) block - OTR_ALIGNED_OFFSET(sizeof(Header), OTR_PLATFORM_MEMORY_ALIGNMENT);
        const Header* const header = (Header*) headerAddress;

        *outSize      = header->Size;
        *outOffset =
            headerAddress - (UIntPtr) m_Memory + OTR_ALIGNED_OFFSET(sizeof(Header), OTR_PLATFORM_MEMORY_ALIGNMENT);
        *outPadding   = header->Padding;
        *outAlignment = OTR_PLATFORM_MEMORY_ALIGNMENT;
    }

    FreeListAllocator::Node* FreeListAllocator::FindFirstFit(Node** previous,
                                                             UInt16* padding,
                                                             const UInt64 size,
                                                             const UInt16 alignment)
    {
        Node* itCurrent  = m_Head;
        Node* itPrevious = nullptr;
        UIntPtr itPadding = 0;

        while (itCurrent)
        {
            itPadding = GetAlignmentPadding((UIntPtr) itCurrent, alignment);
            UInt64 requiredSpace = size + itPadding;

            if (itCurrent->Size >= requiredSpace)
                break;

            itPrevious = itCurrent;
            itCurrent = itCurrent->Next;
        }

        if (padding)
            *padding = itPadding;

        if (previous)
            *previous = itPrevious;

        return itCurrent;
    }

    FreeListAllocator::Node* FreeListAllocator::FindBestFit(Node** previous,
                                                            UInt16* padding,
                                                            const UInt64 size,
                                                            const UInt16 alignment)
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
            UInt64 difference = itCurrent->Size - requiredSpace;

            if (difference == 0)
            {
                itBest = itCurrent;
                break;
            }
            else if (itCurrent->Size > requiredSpace && difference < smallestDifference)
            {
                smallestDifference = difference;
                itBest             = itCurrent;
            }

            itPrevious = itCurrent;
            itCurrent         = itCurrent->Next;
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
                toInsert->Next = m_Head;

            m_Head = toInsert;
        }
        else
        {
            if (!previous->Next)
            {
                previous->Next = toInsert;
                toInsert->Next = nullptr;
            }
            else
            {
                toInsert->Next = previous->Next;
                previous->Next = toInsert;
            }
        }
    }

    void FreeListAllocator::Remove(Node* toRemove, Node* previous)
    {
        OTR_INTERNAL_ASSERT_MSG(toRemove, "Node to remove must not be null")

        if (!previous)
            m_Head = toRemove->Next;
        else
            previous->Next = toRemove->Next;
    }

    void FreeListAllocator::Merge(Node* toMerge, Node* previousNode)
    {
        if (toMerge
            && toMerge->Next
            && ((UIntPtr) toMerge + toMerge->Size) == (UIntPtr) toMerge->Next)
        {
            toMerge->Size += toMerge->Next->Size;
            Remove(toMerge->Next, toMerge);
        }

        if (previousNode
            && previousNode->Next
            && ((UIntPtr) previousNode + previousNode->Size) == (UIntPtr) toMerge)
        {
            previousNode->Size += toMerge->Size;
            Remove(toMerge, previousNode);
        }
    }

    UIntPtr FreeListAllocator::GetAlignmentPadding(const UIntPtr& address, const UInt16 alignment)
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