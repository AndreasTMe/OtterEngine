#ifndef OTTERENGINE_STACK_H
#define OTTERENGINE_STACK_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Memory.h"

#include "Core/Collections/Collection.h"

namespace Otter
{
    template<typename T>
    class Stack final : public Collection<T>
    {
        OTR_USING_BASE(Collection<T>)

    public:
        OTR_COLLECTION_CONSTRUCT(Stack)
        OTR_COLLECTION_COPY(Stack)
        OTR_COLLECTION_MOVE(Stack)

        OTR_INLINE void Push(const T& item)
        {
            if (base::m_Count >= base::m_Capacity)
                base::Expand();

            base::m_Data[base::m_Count++] = item;
        }

        OTR_INLINE void Push(T&& item)
        {
            if (base::m_Count >= base::m_Capacity)
                base::Expand();

            base::m_Data[base::m_Count++] = std::move(item);
        }

        OTR_INLINE bool TryPop()
        {
            if (base::m_Count == 0)
                return false;

            base::m_Count--;

            return true;
        }

        OTR_INLINE bool TryPeek(const T& item)
        {
            if (base::m_Count == 0)
                return false;

            item = base::m_Data[base::m_Count - 1];

            return true;
        }
    };
}

#endif //OTTERENGINE_STACK_H
