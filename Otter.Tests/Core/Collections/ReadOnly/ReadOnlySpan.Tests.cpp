#include <gtest/gtest.h>

#include "Core/Collections/ReadOnly/ReadOnlySpan.h"

template<typename T, UInt64 Size>
using ReadOnlySpan = Otter::ReadOnlySpan<T, Size>;
