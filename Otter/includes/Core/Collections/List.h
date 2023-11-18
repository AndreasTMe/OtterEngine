#ifndef OTTERENGINE_LIST_H
#define OTTERENGINE_LIST_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Memory.h"

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

        [[nodiscard]] T& operator[](UInt64 index)
        {
            OTR_ASSERT_MSG(index < base::m_Count, "List index out of bounds")
            return base::m_Data[index];
        }

        [[nodiscard]] const T& operator[](UInt64 index) const
        {
            OTR_ASSERT_MSG(index < base::m_Count, "List index out of bounds")
            return base::m_Data[index];
        }

        void Add(const T& item)
        {
            if (base::m_Count >= base::m_Capacity)
                base::Expand();

            base::m_Data[base::m_Count++] = item;
        }

        void Add(T&& item) noexcept
        {
            if (base::m_Count >= base::m_Capacity)
                base::Expand();

            base::m_Data[base::m_Count++] = std::move(item);
        }

        bool TryRemove(const T& item)
        {
            for (UInt64 i = 0; i < base::m_Count; i++)
                if (base::m_Data[i] == item)
                    return TryRemoveAt(i);

            return false;
        }

        bool TryRemove(T&& item) noexcept
        {
            for (UInt64 i = 0; i < base::m_Count; i++)
                if (base::m_Data[i] == item)
                    return TryRemoveAt(i);

            return false;
        }

        bool TryRemoveAt(const UInt64 index)
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
