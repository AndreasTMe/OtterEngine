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
            : k_Head(head), m_Ptr(ptr), m_Capacity(capacity), m_SlotsInUse(slotsInUse)
        {
            if (k_Head == m_Ptr)
            {
                auto index = 0;
                while (index < m_Capacity && !m_SlotsInUse.Get(index))
                {
                    m_Ptr++;
                    index++;
                }
            }
            else
            {
                auto index = m_Capacity - 1;
                while (k_Head < m_Ptr && !m_SlotsInUse.Get(index))
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
            if (m_SlotsInUse.Get(index))
                return *this;

            while (index <= m_Capacity && !m_SlotsInUse.Get(index))
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
            if (index >= 0 && m_SlotsInUse.Get(index))
                return *this;

            while (k_Head < m_Ptr && !m_SlotsInUse.Get(index))
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

        Slot<T>* m_Ptr;
        UInt64 m_Capacity;
        BitSet m_SlotsInUse;
    };
}

#endif //OTTERENGINE_SLOTITERATOR_H
