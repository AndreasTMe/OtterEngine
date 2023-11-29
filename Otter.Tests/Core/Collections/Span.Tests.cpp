#include <gtest/gtest.h>

#include "Core/Collections/Span.h"
#include "Core/Collections/ReadOnly/ReadOnlySpan.h"

template<typename T, UInt64 Size>
using Span = Otter::Span<T, Size>;

TEST(Span, Initialisation_Default)
{
    Span<int, 5> span;

    EXPECT_NE(span.GetData(), nullptr);
    EXPECT_EQ(span.GetSize(), 5);

    for (UInt64 i = 0; i < span.GetSize(); ++i)
        EXPECT_EQ(span[i], 0);
}

TEST(Span, Initialisation_FromList)
{
    Span<int, 5> span = { 1, 2, 3, 4, 5 };

    EXPECT_NE(span.GetData(), nullptr);
    EXPECT_EQ(span.GetSize(), 5);

    for (UInt64 i = 0; i < span.GetSize(); ++i)
        EXPECT_EQ(span[i], i + 1);
}

TEST(Span, Initialisation_Copy)
{
    Span<int, 5> span = { 1, 2, 3, 4, 5 };
    Span<int, 5> copy = span;

    EXPECT_EQ(copy.GetSize(), span.GetSize());

    for (UInt64 i = 0; i < span.GetSize(); ++i)
        EXPECT_EQ(copy[i], span[i]);
}

TEST(Span, Initialisation_Move)
{
    Span<int, 5> span = { 1, 2, 3, 4, 5 };
    Span<int, 5> move = std::move(span);

    EXPECT_NE(move.GetData(), nullptr);
    EXPECT_EQ(move.GetSize(), 5);

    for (UInt64 i = 0; i < move.GetSize(); ++i)
    {
        EXPECT_EQ(move[i], i + 1);
        EXPECT_EQ(span[i], 0);
    }
}

TEST(Span, Initialisation_CopyAssign)
{
    Span<int, 5> span = { 1, 2, 3, 4, 5 };
    Span<int, 5> copy;
    copy = span;

    EXPECT_EQ(copy.GetSize(), span.GetSize());

    for (UInt64 i = 0; i < span.GetSize(); ++i)
        EXPECT_EQ(copy[i], span[i]);
}

TEST(Span, Initialisation_MoveAssign)
{
    Span<int, 5> span = { 1, 2, 3, 4, 5 };
    Span<int, 5> move;
    move = std::move(span);

    EXPECT_NE(move.GetData(), nullptr);
    EXPECT_EQ(move.GetSize(), 5);

    for (UInt64 i = 0; i < move.GetSize(); ++i)
        EXPECT_EQ(move[i], i + 1);
}

TEST(Span, AsReadOnly)
{
    Span<int, 5> span     = { 1, 2, 3, 4, 5 };
    auto         readOnly = span.AsReadOnly();

    EXPECT_EQ(readOnly.GetSize(), span.GetSize());

    for (UInt64 i = 0; i < span.GetSize(); ++i)
        EXPECT_EQ(readOnly[i], span[i]);
}