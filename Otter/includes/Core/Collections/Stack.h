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

        bool TryPop(T& item)
        {
            if (base::m_Count == 0)
                return false;

            item = base::m_Data[base::m_Count - 1];
            base::m_Count--;

            return true;
        }

        bool TryPeek(T& item)
        {
            if (base::m_Count == 0)
                return false;

            item = base::m_Data[base::m_Count - 1];

            return true;
        }
    };
}

#endif //OTTERENGINE_STACK_H
