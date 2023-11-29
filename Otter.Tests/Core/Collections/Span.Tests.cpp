#include <gtest/gtest.h>

#include "Core/Collections/Span.h"

template<typename T, UInt64 Size>
using Span = Otter::Span<T, Size>;
