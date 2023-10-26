#ifndef OTTERENGINE_QUEUE_H
#define OTTERENGINE_QUEUE_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Memory.h"

#include "Core/Collections/Collection.h"

namespace Otter
{
    template<typename T>
    class Queue final : public Collection<T>
    {
        OTR_USING_BASE(Collection<T>)

    public:
        OTR_COLLECTION_CONSTRUCT(Queue)
        OTR_COLLECTION_COPY(Queue)
        OTR_COLLECTION_MOVE(Queue)

        void Enqueue(const T& item)
        {
            if (base::m_Count >= base::m_Capacity)
                base::Expand();

            base::m_Data[base::m_Count++] = item;
        }

        void Enqueue(T&& item) noexcept
        {
            if (base::m_Count >= base::m_Capacity)
                base::Expand();

            base::m_Data[base::m_Count++] = std::move(item);
        }

        bool TryDequeue()
        {
            if (base::m_Count == 0)
                return false;

            for (UInt64 i = 0; i < base::m_Count - 1; i++)
                base::m_Data[i] = base::m_Data[i + 1];

            base::m_Count--;

            return true;
        }

        bool TryPeek(T& item)
        {
            if (base::m_Count == 0)
                return false;

            item = base::m_Data[0];

            return true;
        }
    };
}

#endif //OTTERENGINE_QUEUE_H
