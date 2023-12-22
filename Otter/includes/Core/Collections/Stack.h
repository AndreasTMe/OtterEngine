#ifndef OTTERENGINE_STACK_H
#define OTTERENGINE_STACK_H

#include "Core/Collections/Collection.h"
#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    /**
     * @brief A LIFO (Last In First Out) collection. The items are stored in a contiguous memory block on the heap.
     * It does not have a fixed capacity and will expand as needed.
     *
     * @tparam T The type of the items in the stack.
     */
    template<typename T>
    class Stack final : public Collection<T>
    {
        OTR_USING_BASE(Collection<T>)

        /// @brief An alias for an iterator for the stack.
        using Iterator = LinearIterator<T>;

        /// @brief An alias for a const iterator for the stack.
        using ConstIterator = LinearIterator<const T>;

    public:
        OTR_COLLECTION_CONSTRUCT(Stack)
        OTR_COLLECTION_COPY(Stack)
        OTR_COLLECTION_MOVE(Stack)

        /**
         * @brief Pushes an item onto the stack.
         *
         * @param item The item to push onto the stack.
         */
        void Push(const T& item)
        {
            if (base::m_Count >= base::m_Capacity)
                base::Expand();

            base::m_Data[base::m_Count++] = item;
        }

        /**
         * @brief Pushes an item onto the stack.
         *
         * @param item The item to push onto the stack.
         */
        void Push(T&& item)
        {
            if (base::m_Count >= base::m_Capacity)
                base::Expand();

            base::m_Data[base::m_Count++] = std::move(item);
        }

        /**
         * @brief Tries to pop an item from the stack.
         *
         * @param outItem The item that was popped from the stack.
         *
         * @return True if an item was popped from the stack, false otherwise.
         */
        bool TryPop(T* outItem)
        {
            if (base::m_Count == 0)
                return false;

            *outItem = base::m_Data[base::m_Count - 1];
            base::m_Count--;

            return true;
        }

        /**
         * @brief Tries to peek at the top item of the stack.
         *
         * @param outItem The item at the top of the stack.
         *
         * @return True if an item was peeked at, false otherwise.
         */
        bool TryPeek(T* outItem)
        {
            if (base::m_Count == 0)
                return false;

            *outItem = base::m_Data[base::m_Count - 1];

            return true;
        }

        /**
         * @brief Gets an iterator to the first element of the stack.
         *
         * @return An iterator to the first element of the stack.
         */
        OTR_INLINE Iterator begin() noexcept { return Iterator(base::m_Data); }

        /**
         * @brief Gets an iterator to the last element of the stack.
         *
         * @return An iterator to the last element of the stack.
         */
        OTR_INLINE Iterator end() noexcept { return Iterator(base::m_Data + base::m_Count); }

        /**
         * @brief Gets a reverse iterator to the last element of the stack.
         *
         * @return A reverse iterator to the last element of the stack.
         */
        OTR_INLINE Iterator rbegin() noexcept { return Iterator(base::m_Data + base::m_Count - 1); }

        /**
         * @brief Gets a reverse iterator to the first element of the stack.
         *
         * @return A reverse iterator to the first element of the stack.
         */
        OTR_INLINE Iterator rend() noexcept { return Iterator(base::m_Data - 1); }

        /**
         * @brief Gets a const iterator to the first element of the stack.
         *
         * @return A const iterator to the first element of the stack.
         */
        OTR_INLINE ConstIterator cbegin() const noexcept { return ConstIterator(base::m_Data); }

        /**
         * @brief Gets a const iterator to the last element of the stack.
         *
         * @return A const iterator to the last element of the stack.
         */
        OTR_INLINE ConstIterator cend() const noexcept { return ConstIterator(base::m_Data + base::m_Count); }

        /**
         * @brief Gets a reverse const iterator to the last element of the stack.
         *
         * @return A reverse const iterator to the last element of the stack.
         */
        OTR_INLINE ConstIterator crbegin() const noexcept { return ConstIterator(base::m_Data + base::m_Count - 1); }

        /**
         * @brief Gets a reverse const iterator to the first element of the stack.
         *
         * @return A reverse const iterator to the first element of the stack.
         */
        OTR_INLINE ConstIterator crend() const noexcept { return ConstIterator(base::m_Data - 1); }
    };
}

#endif //OTTERENGINE_STACK_H
