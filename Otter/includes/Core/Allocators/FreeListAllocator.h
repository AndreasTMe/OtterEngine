#ifndef OTTERENGINE_FREELISTALLOCATOR_H
#define OTTERENGINE_FREELISTALLOCATOR_H

#include "Core/Defines.h"
#include "Core/Types.h"

#include "Core/Allocators/AbstractAllocator.h"

namespace Otter
{
    /**
     * @brief A free list allocator used for allocating and freeing memory blocks of any size and alignment.
     * When allocating a memory block, the allocator will also allocate a header that holds information about
     * the memory block, and is also used when freeing the memory block. Any free blocks are stored in a linked list.
     *
     * @note The allocator can be used to allocate memory blocks in either a first fit or best fit manner.
     * @note When allocating a memory block, the allocator will iterate through the linked list and find the
     * appropriate block that fits the size and alignment. When freeing a memory block, the allocator will iterate
     * through the linked list and find the correct position to insert the free block. If the free block is adjacent
     * to another free block, the allocator will merge the two blocks together.
     */
    class FreeListAllocator final : public AbstractAllocator
    {
        struct Iterator;

    public:
        /**
         * @brief The allocation policy of the allocator.
         */
        enum class Policy : UInt8
        {
            /// @brief Allocates the first free block that fits the size and alignment.
            FirstFit = 0,
            /// @brief Allocates the smallest free block that fits the size and alignment.
            BestFit  = 1
        };

        /**
         * @brief Default constructor.
         */
        FreeListAllocator()
            : AbstractAllocator(), m_Head(nullptr), m_Policy(Policy::FirstFit)
        {
        }

        /**
         * @brief Constructor.
         *
         * @param memory The memory to use.
         * @param memorySize The size of the memory.
         * @param policy The allocation policy.
         */
        explicit FreeListAllocator(void* memory, UInt64 memorySize, Policy policy = Policy::FirstFit)
            : AbstractAllocator(memory, memorySize), m_Head(nullptr), m_Policy(policy)
        {
            Clear();
        }

        /**
         * @brief Destructor.
         */
        ~FreeListAllocator() final { m_Memory = nullptr; }

        /**
         * @brief Allocates a memory block
         *
         * @param size Size of the memory to allocate
         * @param alignment Alignment of the memory to allocate
         *
         * @return Pointer to the allocated memory block
         */
        void* Allocate(UInt64 size, UInt16 alignment) final;

        /**
         * @brief Frees a memory block
         *
         * @param block Pointer to the memory block to free
         */
        void Free(void* block) final;

        /**
         * @brief Retrieves the memory footprint of a memory block in the allocator
         *
         * @param block Pointer to the memory block
         * @param outSize The size of the memory block inside the allocator
         * @param outOffset The offset of the memory block from the start of the allocator
         * @param outPadding The padding of the memory block
         * @param outAlignment The alignment of the memory block
         */
        void GetMemoryFootprint(const void* block,
                                UInt64* outSize,
                                UInt64* outOffset,
                                UInt16* outPadding,
                                UInt16* outAlignment) const final;

        /**
         * @brief Clears the allocator.
         */
        void Clear();

        /**
         * @brief Retrieves the size of the allocator header.
         *
         * @return The size of the allocator header.
         */
        [[nodiscard]] OTR_INLINE static constexpr UInt64 GetAllocatorHeaderSize() noexcept { return sizeof(Header); }

        /**
         * @brief Retrieves the allocator's allocation policy.
         *
         * @return The allocator's allocation policy.
         */
        [[nodiscard]] OTR_INLINE constexpr Policy GetAllocationPolicy() const { return m_Policy; }

        /**
         * @brief The iterator starting point for the free list allocator.
         *
         * @return The iterator starting point.
         */
        [[nodiscard]] OTR_INLINE Iterator begin() const noexcept { return Iterator(m_Head); }

        /**
         * @brief The iterator ending point for the free list allocator.
         *
         * @return The iterator ending point.
         */
        [[nodiscard]] OTR_INLINE Iterator end() const noexcept { return Iterator(nullptr); }

    private:
        /**
         * @brief The header that holds information about an allocated memory block.
         */
        struct Header
        {
            UInt64 Size;
            UInt16 Padding;
        };

        /**
         * @brief A node of the allocator. Holds information about a free memory block.
         */
        struct Node
        {
            UInt64 Size;
            Node* Next;
        };

        /**
         * @brief The iterator for the free list allocator.
         */
        struct Iterator
        {
        public:
            /**
             * @brief Constructor.
             *
             * @param node The node to iterate from.
             */
            explicit Iterator(Node* node)
                : m_Node(node)
            {
            }

            /**
             * @brief Increments the iterator.
             *
             * @return The incremented iterator.
             */
            OTR_INLINE Iterator& operator++()
            {
                m_Node = m_Node->Next;
                return *this;
            }

            /**
             * @brief Increments the iterator.
             *
             * @return The iterator before incrementation.
             */
            OTR_INLINE const Iterator operator++(int)
            {
                Iterator iterator = *this;
                ++(*this);
                return iterator;
            }

            /**
             * @brief Dereferences the iterator.
             *
             * @return The dereferenced iterator.
             */
            OTR_INLINE Node& operator*() { return *m_Node; }

            /**
             * @brief Equality operator.
             *
             * @return True if the iterators are equal, false otherwise.
             */
            OTR_INLINE bool operator==(const Iterator& other) const { return m_Node == other.m_Node; }

            /**
             * @brief Inequality operator.
             *
             * @return True if the iterators are not equal, false otherwise.
             */
            OTR_INLINE bool operator!=(const Iterator& other) const { return !(*this == other); }

        private:
            Node* m_Node;
        };

        Node* m_Head;
        Policy m_Policy;

        /**
         * @brief Finds the first fit memory block.
         *
         * @param previous The previous node.
         * @param padding The padding of the memory block.
         * @param size The size of the memory block.
         * @param alignment The alignment of the memory block.
         *
         * @return The first fit memory block.
         */
        Node* FindFirstFit(Node** previous, UInt16* padding, UInt64 size, UInt16 alignment);

        /**
         * @brief Finds the best fit memory block.
         *
         * @param previous The previous node.
         * @param padding The padding of the memory block.
         * @param size The size of the memory block.
         * @param alignment The alignment of the memory block.
         *
         * @return The best fit memory block.
         */
        Node* FindBestFit(Node** previous, UInt16* padding, UInt64 size, UInt16 alignment);

        /**
         * @brief Inserts a memory block into the allocator.
         *
         * @param node The node to insert.
         * @param previous The previous node.
         */
        void Insert(Node* node, Node* previous);

        /**
         * @brief Removes a memory block from the allocator.
         *
         * @param node The node to remove.
         * @param previous The previous node.
         */
        void Remove(Node* node, Node* previous);

        /**
         * @brief Merges two memory blocks together.
         *
         * @param toMerge The memory block to merge.
         * @param previousNode The previous node.
         */
        void Merge(Node* toMerge, Node* previousNode);

        /**
         * @brief Retrieves the alignment padding of a memory block.
         *
         * @param address The address of the memory block.
         * @param alignment The alignment of the memory block.
         *
         * @return The address of the memory block with the alignment padding.
         */
        static UIntPtr GetAlignmentPadding(const UIntPtr& address, UInt16 alignment);
    };
}

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::FreeListAllocator::Policy& policy)
{
    switch (policy)
    {
        case Otter::FreeListAllocator::Policy::FirstFit:
            os << OTR_NAME_OF(FreeListAllocator::Policy::FirstFit);
            break;
        case Otter::FreeListAllocator::Policy::BestFit:
            os << OTR_NAME_OF(FreeListAllocator::Policy::BestFit);
            break;
        default:
            os << "Unknown FreeListAllocator::Policy";
    }

    return os;
}

#endif //OTTERENGINE_FREELISTALLOCATOR_H
