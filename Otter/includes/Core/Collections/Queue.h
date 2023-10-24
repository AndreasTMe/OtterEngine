#ifndef OTTERENGINE_QUEUE_H
#define OTTERENGINE_QUEUE_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Core/Memory.h"

#include "Core/Collections/Iterators/LinearIterator.h"

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

        OTR_INLINE void Push(const T& item)
        {
            if (base::m_Count >= base::m_Capacity)
            {
                base::m_Capacity = base::m_Capacity == 0 ? 2 : base::m_Capacity * 1.5;
                T* newData = Buffer::New<T>(base::m_Capacity);

                for (UInt64 i = 0; i < base::m_Count; i++)
                    newData[i] = base::m_Data[i];

                if (base::m_Count > 0)
                    Buffer::Delete(base::m_Data, base::m_Count);

                base::m_Data = newData;
            }

            base::m_Data[base::m_Count++] = item;
        }

        OTR_INLINE bool TryPop()
        {
            if (base::m_Count == 0)
                return false;

            for (UInt64 i = 0; i < base::m_Count - 1; i++)
                base::m_Data[i] = base::m_Data[i + 1];

            base::m_Count--;

            return true;
        }

        OTR_INLINE const T& Peek()
        {
            OTR_ASSERT_MSG(base::m_Count > 0, "Queue is empty")
            return base::m_Data[0];
        }
    };
}

#endif //OTTERENGINE_QUEUE_H
