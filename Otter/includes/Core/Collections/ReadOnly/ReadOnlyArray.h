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

        ReadOnlyArray() { m_Data = Buffer::New<T>(Size); }
        ~ReadOnlyArray() { Buffer::Delete(m_Data, Size); }

        OTR_WITH_CONST_ITERATOR(ConstIterator, m_Data, Size)
        OTR_DISABLE_OBJECT_COPIES(ReadOnlyArray)
        OTR_DISABLE_OBJECT_MOVES(ReadOnlyArray)

        ReadOnlyArray(InitialiserList<T> list)
        {
            OTR_ASSERT_MSG(list.size() == Size, "Initialiser list size does not match span size")

            UInt64 i = 0;
            for (const T& value: list)
                m_Data[i++] = value;
        }

        explicit ReadOnlyArray(const Array<T, Size>& other)
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = other.m_Data[i];
        }

        explicit ReadOnlyArray(Array<T, Size>&& other) noexcept
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);

            other.m_Data = nullptr;
        }

        ReadOnlyArray<T, Size>& operator=(const Array<T, Size>& other)
        {
            if (this == &other)
                return *this;

            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = other.m_Data[i];

            return *this;
        }

        ReadOnlyArray<T, Size>& operator=(Array<T, Size>&& other) noexcept
        {
            if (this == &other)
                return *this;

            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);

            other.m_Data = nullptr;

            return *this;
        }

        const T& operator[](UInt64 index) const
        {
            OTR_ASSERT_MSG(index < Size, "ReadOnlyArray index out of bounds")
            return m_Data[index];
        }

        [[nodiscard]] OTR_INLINE const T* GetData() const { return m_Data; }
        [[nodiscard]] OTR_INLINE constexpr UInt64 GetSize() const { return Size; }

    private:
        T m_Data[Size];
    };
}

template<typename OStream, typename T, UInt64 Size>
OStream& operator<<(OStream& os, const Otter::ReadOnlyArray<T, Size>& array)
{
    os << "ReadOnlyArray: [";

    for (UInt64 i = 0; i < Size; i++)
    {
        os << array[i];

        if (i >= 2)
        {
            os << ", ...";
            break;
        }

        if (i != Size - 1)
            os << ", ";
    }

    os << "]";

    return os;
}

#endif //OTTERENGINE_READONLYARRAY_H
