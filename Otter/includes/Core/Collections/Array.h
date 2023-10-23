#ifndef OTTERENGINE_ARRAY_H
#define OTTERENGINE_ARRAY_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Core/Memory.h"

#include "Core/Collections/Iterators/LinearIterator.h"

namespace Otter
{
    template<typename T, UInt64 Size>
    struct ReadOnlyArray;

    template<typename T, UInt64 Size>
    struct Array final
    {
    public:
        using Iterator = LinearIterator<T>;
        using ConstIterator = LinearIterator<const T>;

        Array() { m_Data = Buffer::New<T>(Size); }
        ~Array() { Buffer::Delete(m_Data, Size); }

        OTR_WITH_ITERATOR(Iterator, m_Data, Size)
        OTR_WITH_CONST_ITERATOR(ConstIterator, m_Data, Size)
        OTR_DISABLE_HEAP_ALLOCATION

        OTR_INLINE explicit Array(const T& value)
        {
            m_Data = Buffer::New<T>(Size);

            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = value;
        }

        template<typename... Args>
        OTR_INLINE explicit Array(const T& value, const Args&& ... args)
        {
            OTR_ASSERT_MSG(sizeof...(Args) < Size, "Array size is too small")

            m_Data = Buffer::New<T>(Size);

            UInt64 i = 0;
            m_Data[i++] = value;
            ([&]()
            {
                m_Data[i++] = args;
            }(), ...);
        }

        OTR_INLINE Array(const Array<T, Size>& other)
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = other.m_Data[i];
        }

        OTR_INLINE Array(Array<T, Size>&& other) noexcept
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);
        }

        OTR_INLINE Array<T, Size>& operator=(const Array<T, Size>& other)
        {
            if (this == &other)
                return *this;

            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = other.m_Data[i];

            return *this;
        }

        OTR_INLINE Array<T, Size>& operator=(Array<T, Size>&& other) noexcept
        {
            for (UInt64 i = 0; i < Size; i++)
                m_Data[i] = std::move(other.m_Data[i]);

            return *this;
        }

        OTR_INLINE T& operator[](UInt64 index)
        {
            OTR_ASSERT_MSG(index < Size, "Array index out of bounds")
            return m_Data[index];
        }

        OTR_INLINE const T& operator[](UInt64 index) const
        {
            OTR_ASSERT_MSG(index < Size, "Array index out of bounds")
            return m_Data[index];
        }

        [[nodiscard]] OTR_INLINE T* GetData() { return m_Data; }
        [[nodiscard]] OTR_INLINE const T* GetData() const { return m_Data; }

        [[nodiscard]] OTR_INLINE const ReadOnlyArray<T, Size> AsReadOnly() const
        {
            return ReadOnlyArray<T, Size>(*this);
        }

        [[nodiscard]] OTR_INLINE constexpr UInt64 Length() const { return Size; }

    private:
        T* m_Data;

        friend class ReadOnlyArray<T, Size>;
    };
}

template<typename OStream, typename T, UInt64 Size>
OTR_INLINE OStream& operator<<(OStream& os, const Otter::Array<T, Size>& array)
{
    os << "Array: [";

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

#endif //OTTERENGINE_ARRAY_H
