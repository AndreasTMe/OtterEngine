#ifndef OTTERENGINE_ENUMERABLE_H
#define OTTERENGINE_ENUMERABLE_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Memory.h"

#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    template<typename T>
    class Enumerable final
    {
    public:
        using Iterator = LinearIterator<T>;
        using ConstIterator = LinearIterator<const T>;

        ~Enumerable()
        {
            if (m_Data != nullptr && m_Count > 0)
                Buffer::Delete(m_Data, m_Count);

            m_Data  = nullptr;
            m_Count = 0;
        }

        OTR_WITH_ITERATOR(Iterator, m_Data, m_Count)
        OTR_WITH_CONST_ITERATOR(ConstIterator, m_Data, m_Count)
        OTR_DISABLE_HEAP_ALLOCATION

        static Enumerable Of(InitialiserList<T> list)
        {
            Enumerable enumerable;
            enumerable.m_Data = Buffer::New<T>(list.size());

            enumerable.m_Count = 0;
            for (const T& item: list)
                enumerable.m_Data[enumerable.m_Count++] = item;

            return enumerable;
        }

        OTR_INLINE static Enumerable Empty() { return { }; }

        [[nodiscard]] OTR_INLINE constexpr UInt64 GetCount() const { return m_Count; }
        [[nodiscard]] OTR_INLINE constexpr bool IsEmpty() const { return m_Count == 0; }

        [[nodiscard]] OTR_INLINE constexpr T* GetData() const { return m_Data; }

    private:
        Enumerable()
            : m_Data(nullptr), m_Count(0)
        {
        }

        T* m_Data = nullptr;
        UInt64 m_Count = 0;
    };
}

#endif //OTTERENGINE_ENUMERABLE_H