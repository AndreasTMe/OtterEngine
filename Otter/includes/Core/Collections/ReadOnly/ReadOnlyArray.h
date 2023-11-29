#ifndef OTTERENGINE_READONLYARRAY_H
#define OTTERENGINE_READONLYARRAY_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Core/Memory.h"

#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    template<typename T, UInt64 Size>
    struct Array;

    template<typename T, UInt64 Size>
    struct ReadOnlyArray final
    {
    public:
        using ConstIterator = LinearIterator<const T>;

        ReadOnlyArray()
        {
            if constexpr (Size > 0)
                m_Data = Buffer::New<T>(Size);
        }
        ~ReadOnlyArray()
        {
            if (m_Data != nullptr && Size > 0)
                Buffer::Delete(m_Data, Size);
        }

        OTR_WITH_CONST_ITERATOR(ConstIterator, m_Data, Size)
        OTR_DISABLE_OBJECT_COPIES(ReadOnlyArray)
        OTR_DISABLE_OBJECT_MOVES(ReadOnlyArray)

        ReadOnlyArray(InitialiserList<T> list)
        {
            OTR_ASSERT_MSG(list.size() == Size, "Initialiser list size does not match span size")

            m_Data = Buffer::New<T>(Size);

            UInt64 i = 0;
            for (const T& value: list)
                m_Data[i++] = value;
        }

        explicit ReadOnlyArray(const Array<T, Size>& other)
        {
            m_Data = Buffer::New<T>(Size);

            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = other.m_Data[i];
        }

        explicit ReadOnlyArray(Array<T, Size>&& other) noexcept
        {
            m_Data = Buffer::New<T>(Size);

            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);

            other.m_Data = nullptr;
        }

        const T& operator[](UInt64 index) const
        {
            OTR_ASSERT_MSG(index < Size, "ReadOnlyArray index out of bounds")
            return m_Data[index];
        }

        [[nodiscard]] OTR_INLINE const T* GetData() const { return m_Data; }
        [[nodiscard]] OTR_INLINE constexpr UInt64 GetSize() const { return Size; }

    private:
        T* m_Data;
    };
}

#endif //OTTERENGINE_READONLYARRAY_H
