#ifndef OTTERENGINE_ENUMERABLE_H
#define OTTERENGINE_ENUMERABLE_H

#include "Core/Memory.h"
#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    template<typename T>
    class Enumerable final
    {
        using Iterator = LinearIterator<T>;
        using ConstIterator = LinearIterator<const T>;

    public:
        ~Enumerable()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Count);

            m_Data  = nullptr;
            m_Count = 0;
        }

        static Enumerable Of(InitialiserList<T> list)
        {
            Enumerable enumerable;
            enumerable.m_Data = Buffer::New<T>(list.size());

            enumerable.m_Count = 0;
            for (const T& item: list)
                enumerable.m_Data[enumerable.m_Count++] = item;

            return enumerable;
        }

        void ClearDestructive()
        {
            if (IsCreated())
                Buffer::Delete<T>(m_Data, m_Count);

            m_Data  = nullptr;
            m_Count = 0;
        }

        OTR_INLINE static Enumerable Empty() { return { }; }

        [[nodiscard]] OTR_INLINE const T* GetData() const noexcept { return m_Data; }
        [[nodiscard]] OTR_INLINE UInt64 GetCount() const noexcept { return m_Count; }
        [[nodiscard]] OTR_INLINE bool IsCreated() const noexcept { return m_Data && m_Count > 0; }
        [[nodiscard]] OTR_INLINE bool IsEmpty() const noexcept { return m_Count == 0; }

        OTR_INLINE Iterator begin() noexcept { return Iterator(m_Data); }
        OTR_INLINE Iterator end() noexcept { return Iterator(m_Data + m_Count); }
        OTR_INLINE Iterator rbegin() noexcept { return Iterator(m_Data + m_Count - 1); }
        OTR_INLINE Iterator rend() noexcept { return Iterator(m_Data - 1); }

        OTR_INLINE ConstIterator begin() const noexcept { return ConstIterator(m_Data); }
        OTR_INLINE ConstIterator end() const noexcept { return ConstIterator(m_Data + m_Count); }
        OTR_INLINE ConstIterator rbegin() const noexcept { return ConstIterator(m_Data + m_Count - 1); }
        OTR_INLINE ConstIterator rend() const noexcept { return ConstIterator(m_Data - 1); }

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
