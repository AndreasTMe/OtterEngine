#include <gtest/gtest.h>

#include "Core/Collections/Span.h"
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"

template<typename T, UInt64 Size>
using ReadOnlySpan = Otter::ReadOnlySpan<T, Size>;

TEST(ReadOnlySpan, Initialisation_Default)
{
    ReadOnlySpan<int, 5> span;

    EXPECT_NE(span.GetData(), nullptr);
    EXPECT_EQ(span.GetSize(), 5);

    for (UInt64 i = 0; i < span.GetSize(); ++i)
        EXPECT_EQ(span[i], 0);
}

TEST(ReadOnlySpan, Initialisation_FromList)
{
    ReadOnlySpan<int, 5> span = { 1, 2, 3, 4, 5 };

    EXPECT_NE(span.GetData(), nullptr);
    EXPECT_EQ(span.GetSize(), 5);

    for (UInt64 i = 0; i < span.GetSize(); ++i)
        EXPECT_EQ(span[i], i + 1);
}

TEST(ReadOnlySpan, Initialisation_CopySpan)
{
    Otter::Span<int, 5>  span = { 1, 2, 3, 4, 5 };
    ReadOnlySpan<int, 5> readOnlySpan(span);

    EXPECT_NE(readOnlySpan.GetData(), nullptr);
    EXPECT_EQ(readOnlySpan.GetSize(), 5);

    for (UInt64 i = 0; i < readOnlySpan.GetSize(); ++i)
        EXPECT_EQ(readOnlySpan[i], i + 1);
}

TEST(ReadOnlySpan, Initialisation_MoveSpan)
{
    Otter::Span<int, 5>  span = { 1, 2, 3, 4, 5 };
    ReadOnlySpan<int, 5> readOnlySpan(std::move(span));

    EXPECT_NE(readOnlySpan.GetData(), nullptr);
    EXPECT_EQ(readOnlySpan.GetSize(), 5);

    for (UInt64 i = 0; i < readOnlySpan.GetSize(); ++i)
    {
        EXPECT_EQ(readOnlySpan[i], i + 1);
        EXPECT_EQ(span[i], 0);
    }
}

TEST(ReadOnlySpan, Iterator)
{
    ReadOnlySpan<int, 5> span = { 1, 2, 3, 4, 5 };

    UInt64    i  = 0;
    for (auto it = span.cbegin(); it != span.cend(); ++it)
        EXPECT_EQ(*it, ++i);

    for (auto it = span.crbegin(); it != span.crend(); --it)
        EXPECT_EQ(*it, i--);
}