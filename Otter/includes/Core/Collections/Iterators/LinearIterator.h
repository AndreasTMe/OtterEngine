#ifndef OTTERENGINE_LINEARITERATOR_H
#define OTTERENGINE_LINEARITERATOR_H

#include "Core/Defines.h"

namespace Otter
{
    template<typename T>
    struct LinearIterator
    {
    public:
        OTR_INLINE explicit LinearIterator(T* ptr) : m_Ptr(ptr) {}

        OTR_INLINE LinearIterator& operator++()
        {
            m_Ptr++;
            return *this;
        }

        OTR_INLINE const LinearIterator operator++(int)
        {
            LinearIterator iterator = *this;
            ++(*this);
            return iterator;
        }

        OTR_INLINE LinearIterator& operator--()
        {
            m_Ptr--;
            return *this;
        }

        OTR_INLINE const LinearIterator operator--(int)
        {
            LinearIterator iterator = *this;
            --(*this);
            return iterator;
        }

        OTR_INLINE T& operator[](int index) { return *(m_Ptr + index); }
        OTR_INLINE T* operator->() { return m_Ptr; }
        OTR_INLINE T& operator*() { return *m_Ptr; }
        OTR_INLINE bool operator==(const LinearIterator& other) const { return m_Ptr == other.m_Ptr; }
        OTR_INLINE bool operator!=(const LinearIterator& other) const { return !(*this == other); }

    private:
        T* m_Ptr;
    };
}

#endif //OTTERENGINE_LINEARITERATOR_H
