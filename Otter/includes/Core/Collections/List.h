#ifndef OTTERENGINE_LIST_H
#define OTTERENGINE_LIST_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Core/Memory.h"

#include "Core/Collections/Iterators/LinearIterator.h"
#include "Core/Collections/Collection.h"

namespace Otter
{
    template<typename T>
    class List final : public Collection<T>
    {
        OTR_USING_BASE(Collection<T>)

    public:
        OTR_COLLECTION_CONSTRUCT(List)
        OTR_COLLECTION_COPY(List)
        OTR_COLLECTION_MOVE(List)

        [[nodiscard]] OTR_INLINE T& operator[](UInt64 index)
        {
            OTR_ASSERT_MSG(index < base::m_Count, "List index out of bounds")
            return base::m_Data[index];
        }

        [[nodiscard]] OTR_INLINE const T& operator[](UInt64 index) const
        {
            OTR_ASSERT_MSG(index < base::m_Count, "List index out of bounds")
            return base::m_Data[index];
        }

        OTR_INLINE void Add(T item)
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

        OTR_INLINE bool TryRemove(T item)
        {
            for (UInt64 i = 0; i < base::m_Count; i++)
                if (base::m_Data[i] == item)
                    return TryRemoveAt(i);

            return false;
        }

        OTR_INLINE bool TryRemoveAt(UInt64 index)
        {
            if (index >= base::m_Count)
                return false;

            for (UInt64 i = index; i < base::m_Count - 1; i++)
                base::m_Data[i] = base::m_Data[i + 1];

            base::m_Count--;
            return true;
        }
    };
}

#endif //OTTERENGINE_LIST_H
