#ifndef OTTERENGINE_STACK_H
#define OTTERENGINE_STACK_H

#include "Core/Collections/Collection.h"
#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    template<typename T>
    class Stack final : public Collection<T>
    {
        OTR_USING_BASE(Collection<T>)

        using Iterator = LinearIterator<T>;
        using ConstIterator = LinearIterator<const T>;

    public:
        OTR_COLLECTION_CONSTRUCT(Stack)
        OTR_COLLECTION_COPY(Stack)
        OTR_COLLECTION_MOVE(Stack)

        void Push(const T& item)
        {
            if (base::m_Count >= base::m_Capacity)
                base::Expand();

            base::m_Data[base::m_Count++] = item;
        }

        void Push(T&& item)
        {
            if (base::m_Count >= base::m_Capacity)
                base::Expand();

            base::m_Data[base::m_Count++] = std::move(item);
        }

        bool TryPop(T* outItem)
        {
            if (base::m_Count == 0)
                return false;

            *outItem = base::m_Data[base::m_Count - 1];
            base::m_Count--;

            return true;
        }

        bool TryPeek(T* outItem)
        {
            if (base::m_Count == 0)
                return false;

            *outItem = base::m_Data[base::m_Count - 1];

            return true;
        }

        OTR_INLINE Iterator begin() noexcept { return Iterator(base::m_Data); }
        OTR_INLINE Iterator end() noexcept { return Iterator(base::m_Data + base::m_Count); }
        OTR_INLINE Iterator rbegin() noexcept { return Iterator(base::m_Data + base::m_Count - 1); }
        OTR_INLINE Iterator rend() noexcept { return Iterator(base::m_Data - 1); }

        OTR_INLINE ConstIterator begin() const noexcept { return ConstIterator(base::m_Data); }
        OTR_INLINE ConstIterator end() const noexcept { return ConstIterator(base::m_Data + base::m_Count); }
        OTR_INLINE ConstIterator rbegin() const noexcept { return ConstIterator(base::m_Data + base::m_Count - 1); }
        OTR_INLINE ConstIterator rend() const noexcept { return ConstIterator(base::m_Data - 1); }
    };
}

#endif //OTTERENGINE_STACK_H
