#ifndef OTTERENGINE_LINEARITERATOR_H
#define OTTERENGINE_LINEARITERATOR_H

#include "Core/Defines.h"

namespace Otter
{
    /**
     * @brief A linear iterator.
     *
     * @tparam T The type of the iterator.
     */
    template<typename T>
    struct LinearIterator
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param ptr The pointer to the object.
         */
        explicit LinearIterator(T* ptr)
            : m_Ptr(ptr)
        {
        }

        /**
         * @brief Increments the iterator.
         *
         * @return The incremented iterator.
         */
        OTR_INLINE LinearIterator& operator++()
        {
            m_Ptr++;
            return *this;
        }

        /**
         * @brief Increments the iterator.
         *
         * @return The iterator before incrementation.
         */
        OTR_INLINE const LinearIterator operator++(int)
        {
            LinearIterator iterator = *this;
            ++(*this);
            return iterator;
        }

        /**
         * @brief Decrements the iterator.
         *
         * @return The decremented iterator.
         */
        OTR_INLINE LinearIterator& operator--()
        {
            m_Ptr--;
            return *this;
        }

        /**
         * @brief Decrements the iterator.
         *
         * @return The iterator before decrementation.
         */
        OTR_INLINE const LinearIterator operator--(int)
        {
            LinearIterator iterator = *this;
            --(*this);
            return iterator;
        }

        /**
         * @brief Finds the iterated object at the given index.
         *
         * @param index The index to get.
         *
         * @return The indexed object.
         */
        OTR_INLINE T& operator[](int index) { return *(m_Ptr + index); }

        /**
         * @brief Gets the pointer to the iterated object.
         *
         * @return The pointer to the iterated object.
         */
        OTR_INLINE T* operator->() { return m_Ptr; }

        /**
         * @brief Dereferences the iterator.
         *
         * @return The dereferenced object.
         */
        OTR_INLINE T& operator*() { return *m_Ptr; }

        /**
         * @brief Equality operator.
         *
         * @param other The other iterator to compare to.
         *
         * @return True if the iterators are equal, false otherwise.
         */
        OTR_INLINE bool operator==(const LinearIterator& other) const { return m_Ptr == other.m_Ptr; }

        /**
         * @brief Inequality operator.
         *
         * @param other The other iterator to compare to.
         *
         * @return True if the iterators are not equal, false otherwise.
         */
        OTR_INLINE bool operator!=(const LinearIterator& other) const { return !(*this == other); }

    private:
        T* m_Ptr;
    };
}

#endif //OTTERENGINE_LINEARITERATOR_H
