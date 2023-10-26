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
        OTR_DISABLE_HEAP_ALLOCATION

        explicit ReadOnlyArray(const T& value)
        {
            m_Data = Buffer::New<T>(Size);

            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = value;
        }

        template<typename... Args>
        explicit ReadOnlyArray(const T& value, const Args&& ... args)
        {
            OTR_ASSERT_MSG(VariadicArgs<Args...>::GetSize() < Size, "ReadOnlyArray size is too small")

            m_Data = Buffer::New<T>(Size);

            UInt64 i = 0;
            m_Data[i++] = value;
            ([&]()
            {
                m_Data[i++] = args;
            }(), ...);
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
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);

            return *this;
        }

        const T& operator[](UInt64 index) const
        {
            OTR_ASSERT_MSG(index < Size, "ReadOnlyArray index out of bounds")
            return m_Data[index];
        }

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
