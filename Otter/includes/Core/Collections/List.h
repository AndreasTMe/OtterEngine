#ifndef OTTERENGINE_LIST_H
#define OTTERENGINE_LIST_H

#include "Core/Collections/Collection.h"
#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    template<typename T>
    class List final : public Collection<T>
    {
        OTR_USING_BASE(Collection<T>)

        using Iterator = LinearIterator<T>;
        using ConstIterator = LinearIterator<const T>;

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

        bool TryAddAt(const UInt64 index, const T& item)
        {
            if (index >= base::m_Capacity || base::m_Count >= base::m_Capacity)
                return false;

            for (UInt64 i = base::m_Count; i > index; i--)
                base::m_Data[i] = base::m_Data[i - 1];

            base::m_Data[index] = item;
            base::m_Count++;

            return true;
        }

        bool TryAddAt(const UInt64 index, T&& item)
        {
            if (index >= base::m_Capacity || base::m_Count >= base::m_Capacity)
                return false;

            for (UInt64 i = base::m_Count; i > index; i--)
                base::m_Data[i] = base::m_Data[i - 1];

            base::m_Data[index] = std::move(item);
            base::m_Count++;

            return true;
        }

        bool TryAddRange(InitialiserList<T> items, bool allOrNothing = false)
        {
            if (items.size() == 0)
                return false;

            if (items.size() > base::m_Capacity - base::m_Count)
            {
                if (allOrNothing)
                    return false;

                base::Expand(items.size() - (base::m_Capacity - base::m_Count));
            }

            for (const T& item: items)
                Add(item);

            return true;
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

#endif //OTTERENGINE_LIST_H
