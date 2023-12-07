#include <gtest/gtest.h>

#include "Core/Function.h"

template<typename TSignature>
using Function = Otter::Function<TSignature>;

TEST(Function, Initialisation_Default)
{
    Function<void()>          func1;
    EXPECT_FALSE(func1);

    Function<void(int, int)>  func2;
    EXPECT_FALSE(func2);

    Function<float(int, int)> func3;
    EXPECT_FALSE(func3);

    Function<void()> func4 = nullptr;
    EXPECT_FALSE(func4);
    func4 = nullptr;
    EXPECT_FALSE(func4);
}

TEST(Function, Initialisation_Copy)
{
    Function<void()> func1 = []() { };
    Function<void()> func2 = func1;

    EXPECT_TRUE(func1);
    EXPECT_TRUE(func2);
}

TEST(Function, Initialisation_Move)
{
    Function<void()> func1 = []() { };
    Function<void()> func2 = std::move(func1);

    EXPECT_FALSE(func1);
    EXPECT_TRUE(func2);
}

int Add(int a, int b)
{
    return a + b;
}

class TempClass final
{
public:
    [[nodiscard]] int Add(int a, int b) const { return a + b + k_InnerValue; }
    [[nodiscard]] int GetInnerValue() const { return k_InnerValue; }

    [[nodiscard]]static int AddStatic(int a, int b) { return a + b; }

private:
    const int k_InnerValue = 5;
};

TEST(Function, Global_Add)
{
    Function<int(int, int)> addFunction = Add;
    EXPECT_TRUE(addFunction);

    EXPECT_EQ(addFunction(1, 2), 3);
}

TEST(Function, Class_Add)
{
    TempClass               tempClass;
    Function<int(int, int)> addFunction = [ThisPtr = &tempClass](auto&& a, auto&& b)
    {
        return ThisPtr->Add(std::forward<decltype(a)>(a), std::forward<decltype(b)>(b));
    };
    EXPECT_TRUE(addFunction);

    EXPECT_EQ(addFunction(1, 2), 3 + tempClass.GetInnerValue());
}

TEST(Function, Class_Static_Add)
{
    Function<int(int, int)> addFunction = TempClass::AddStatic;
    EXPECT_TRUE(addFunction);

    EXPECT_EQ(addFunction(1, 2), 3);
}

TEST(Function, Lambda_Add)
{
    Function<int(int, int)> addFunction = [](int a, int b) { return a + b; };
    EXPECT_TRUE(addFunction);

    EXPECT_EQ(addFunction(1, 2), 3);
}

TEST(Function, Lambda_Add_Capture_By_Ref)
{
    int                     m_Thing     = 5;
    Function<int(int, int)> addFunction = [&](int a, int b)
    {
        m_Thing = 5;

        return a + b + m_Thing;
    };
    EXPECT_TRUE(addFunction);

    EXPECT_EQ(addFunction(1, 2), 3 + m_Thing);
}

TEST(Function, Lambda_Add_Capture_By_Copy)
{
    int                     m_Thing     = 5;
    Function<int(int, int)> addFunction = [=](int a, int b)
    {
        return a + b + m_Thing;
    };
    EXPECT_TRUE(addFunction);

    EXPECT_EQ(addFunction(1, 2), 3 + m_Thing);
}

TEST(Function, Lambda_Add_Capture_By_Copy_Mutable)
{
    int                     m_Thing     = 5;
    Function<int(int, int)> addFunction = [=](int a, int b) mutable
    {
        m_Thing = 5;

        return a + b + m_Thing;
    };
    EXPECT_TRUE(addFunction);

    EXPECT_EQ(addFunction(1, 2), 3 + m_Thing);
}

TEST(Function, Lambda_Add_Capture_By_Ref_ThisPtr)
{
    int                     m_Thing     = 5;
    Function<int(int, int)> addFunction = [ThisPtr = &m_Thing](int a, int b)
    {
        *ThisPtr = 5;

        return a + b + *ThisPtr;
    };
    EXPECT_TRUE(addFunction);

    EXPECT_EQ(addFunction(1, 2), 3 + m_Thing);
}