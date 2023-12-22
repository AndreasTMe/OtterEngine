#ifndef OTTERENGINE_SLOTITERATOR_H
#define OTTERENGINE_SLOTITERATOR_H

#include "Core/Defines.h"
#include "Core/Collections/Utils/HashSlot.h"

namespace Otter
{
    template<typename T>
    struct SlotIterator
    {
    public:
        SlotIterator(Slot<T>* head, Slot<T>* ptr, UInt64 capacity, const BitSet& slotsInUse) // NOLINT(*-pass-by-value)
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

        OTR_INLINE const SlotIterator operator++(int)
        {
            SlotIterator iterator = *this;
            ++(*this);
            return iterator;
        }

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

        OTR_INLINE const SlotIterator operator--(int)
        {
            SlotIterator iterator = *this;
            --(*this);
            return iterator;
        }

        OTR_INLINE const T& operator*() const { return m_Ptr->Data; }

        OTR_INLINE bool operator==(const SlotIterator& other) const { return m_Ptr == other.m_Ptr; }

        OTR_INLINE bool operator!=(const SlotIterator& other) const { return !(*this == other); }

    private:
        const Slot<T>* const k_Head;
        const UInt64 k_Capacity;
        const BitSet k_SlotsInUse;

        Slot<T>* m_Ptr;
    };
}

#endif //OTTERENGINE_SLOTITERATOR_H
