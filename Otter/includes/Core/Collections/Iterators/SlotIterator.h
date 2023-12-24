#ifndef OTTERENGINE_SLOTITERATOR_H
#define OTTERENGINE_SLOTITERATOR_H

#include "Core/Defines.h"
#include "Core/Collections/BitSet.h"
#include "Core/Collections/Utils/HashSlot.h"

namespace Otter
{
    /**
     * @brief Iterator for a collection of Slots.
     *
     * @tparam T Type of the data stored in the Slots.
     */
    template<typename T>
    struct SlotIterator
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param head The head of the Slot array.
         * @param ptr The current Slot.
         * @param capacity The capacity of the Slot array.
         * @param slotsInUse The BitSet of the Slots in use.
         */
        SlotIterator(Slot<T>* head,
                     Slot<T>* ptr,
                     const UInt64 capacity,
                     const BitSet& slotsInUse) // NOLINT(*-pass-by-value)
            : k_Head(head), k_Capacity(capacity), k_SlotsInUse(slotsInUse), m_Ptr(ptr)
        {
            if (k_Head == m_Ptr)
            {
                auto index = 0;
                while (index < k_Capacity && !k_SlotsInUse.Get(index))
                {
                    m_Ptr++;
                    index++;
                }
            }
            else
            {
                auto index = k_Capacity - 1;
                while (k_Head < m_Ptr && !k_SlotsInUse.Get(index))
                {
                    m_Ptr--;
                    index--;
                }
            }
        }

        /**
         * @brief Increments the iterator.
         *
         * @return The incremented iterator.
         */
        OTR_INLINE SlotIterator& operator++()
        {
            m_Ptr++;

            auto index = m_Ptr - k_Head;
            if (k_SlotsInUse.Get(index))
                return *this;

            while (index <= k_Capacity && !k_SlotsInUse.Get(index))
            {
                m_Ptr++;
                index++;
            }

            return *this;
        }

        /**
         * @brief Increments the iterator.
         *
         * @return The iterator before incrementation.
         */
        OTR_INLINE const SlotIterator operator++(int)
        {
            SlotIterator iterator = *this;
            ++(*this);
            return iterator;
        }

        /**
         * @brief Decrements the iterator.
         *
         * @return The decremented iterator.
         */
        OTR_INLINE SlotIterator& operator--()
        {
            m_Ptr--;

            auto index = m_Ptr - k_Head;
            if (index >= 0 && k_SlotsInUse.Get(index))
                return *this;

            while (k_Head < m_Ptr && !k_SlotsInUse.Get(index))
            {
                m_Ptr--;
                index--;
            }

            return *this;
        }

        /**
         * @brief Decrements the iterator.
         *
         * @return The iterator before decrementation.
         */
        OTR_INLINE const SlotIterator operator--(int)
        {
            SlotIterator iterator = *this;
            --(*this);
            return iterator;
        }

        /**
         * @brief Dereferences the iterator.
         *
         * @return The dereferenced object.
         */
        OTR_INLINE const T& operator*() const { return m_Ptr->Data; }

        /**
         * @brief Equality operator.
         *
         * @param other The other iterator to compare to.
         *
         * @return True if the iterators are equal, false otherwise.
         */
        OTR_INLINE bool operator==(const SlotIterator& other) const { return m_Ptr == other.m_Ptr; }

        /**
         * @brief Inequality operator.
         *
         * @param other The other iterator to compare to.
         *
         * @return True if the iterators are not equal, false otherwise.
         */
        OTR_INLINE bool operator!=(const SlotIterator& other) const { return !(*this == other); }

    private:
        const Slot<T>* const k_Head;
        const UInt64 k_Capacity;
        const BitSet k_SlotsInUse;

        Slot<T>* m_Ptr;
    };
}

#endif //OTTERENGINE_SLOTITERATOR_H
