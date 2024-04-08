#include <gtest/gtest.h>

#include "ECS/ComponentData.h"

using ComponentData = Otter::ComponentData;

class ComponentData_Fixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Otter::MemorySystem::Initialise(4_KiB);
    }

    void TearDown() override
    {
        EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
        Otter::MemorySystem::Shutdown();
    }
};

struct TestComponent1 final : public Otter::IComponent
{
    static constexpr Otter::ComponentId Id = 1;

    int A;
    int B;

    TestComponent1() = default;

    TestComponent1(int a, int b)
        : A(a), B(b)
    {
    }
};

struct TestComponent2 final : public Otter::IComponent
{
    static constexpr Otter::ComponentId Id = 2;

    int A;
    int B;

    TestComponent2() = default;

    TestComponent2(int a, int b)
        : A(a), B(b)
    {
    }
};

struct TestComponent3 final : public Otter::IComponent
{
    static constexpr Otter::ComponentId Id = 3;

    int A;
    int B;

    TestComponent3() = default;

    TestComponent3(int a, int b)
        : A(a), B(b)
    {
    }
};

TEST_F(ComponentData_Fixture, DefaultConstructor)
{
    EXPECT_NO_THROW(auto data = ComponentData{ });
}

TEST_F(ComponentData_Fixture, CopyConstructor)
{
    ComponentData data1;
    EXPECT_FALSE(data1.IsCreated());

    auto t1 = TestComponent1(1, 2);
    auto t2 = TestComponent2(3, 4);
    auto t3 = TestComponent3(5, 6);

    data1.Add(TestComponent1::Id, sizeof(TestComponent1), (Byte*) &t1);
    data1.Add(TestComponent2::Id, sizeof(TestComponent2), (Byte*) &t2);
    data1.Add(TestComponent3::Id, sizeof(TestComponent3), (Byte*) &t3);

    EXPECT_TRUE(data1.IsCreated());

    ComponentData data2{ data1 };

    EXPECT_EQ(data1, data2);
    EXPECT_TRUE(data1.IsCreated());
    EXPECT_TRUE(data2.IsCreated());
}

TEST_F(ComponentData_Fixture, MoveConstructor)
{
    ComponentData data1;
    EXPECT_FALSE(data1.IsCreated());

    auto t1 = TestComponent1(1, 2);
    auto t2 = TestComponent2(3, 4);
    auto t3 = TestComponent3(5, 6);

    data1.Add(TestComponent1::Id, sizeof(TestComponent1), (Byte*) &t1);
    data1.Add(TestComponent2::Id, sizeof(TestComponent2), (Byte*) &t2);
    data1.Add(TestComponent3::Id, sizeof(TestComponent3), (Byte*) &t3);

    EXPECT_TRUE(data1.IsCreated());

    ComponentData data2{ std::move(data1) };

    EXPECT_NE(data1, data2);
    EXPECT_FALSE(data1.IsCreated());
    EXPECT_TRUE(data2.IsCreated());
}

TEST_F(ComponentData_Fixture, AssignmentCopy)
{
    ComponentData data1;
    EXPECT_FALSE(data1.IsCreated());

    auto t1 = TestComponent1(1, 2);
    auto t2 = TestComponent2(3, 4);
    auto t3 = TestComponent3(5, 6);

    data1.Add(TestComponent1::Id, sizeof(TestComponent1), (Byte*) &t1);
    data1.Add(TestComponent2::Id, sizeof(TestComponent2), (Byte*) &t2);
    data1.Add(TestComponent3::Id, sizeof(TestComponent3), (Byte*) &t3);

    EXPECT_TRUE(data1.IsCreated());

    ComponentData data2;
    data2 = data1;

    EXPECT_EQ(data1, data2);
    EXPECT_TRUE(data1.IsCreated());
    EXPECT_TRUE(data2.IsCreated());
}

TEST_F(ComponentData_Fixture, AssignmentMove)
{
    ComponentData data1;
    EXPECT_FALSE(data1.IsCreated());

    auto t1 = TestComponent1(1, 2);
    auto t2 = TestComponent2(3, 4);
    auto t3 = TestComponent3(5, 6);

    data1.Add(TestComponent1::Id, sizeof(TestComponent1), (Byte*) &t1);
    data1.Add(TestComponent2::Id, sizeof(TestComponent2), (Byte*) &t2);
    data1.Add(TestComponent3::Id, sizeof(TestComponent3), (Byte*) &t3);

    EXPECT_TRUE(data1.IsCreated());

    ComponentData data2;
    data2 = std::move(data1);

    EXPECT_NE(data1, data2);
    EXPECT_FALSE(data1.IsCreated());
    EXPECT_TRUE(data2.IsCreated());
}

TEST_F(ComponentData_Fixture, Iterator)
{
    ComponentData componentData;
    EXPECT_FALSE(componentData.IsCreated());

    auto t1 = TestComponent1(1, 2);
    auto t2 = TestComponent2(3, 4);
    auto t3 = TestComponent3(5, 6);

    componentData.Add(TestComponent1::Id, sizeof(TestComponent1), (Byte*) &t1);
    componentData.Add(TestComponent2::Id, sizeof(TestComponent2), (Byte*) &t2);
    componentData.Add(TestComponent3::Id, sizeof(TestComponent3), (Byte*) &t3);

    EXPECT_TRUE(componentData.IsCreated());
    EXPECT_EQ(componentData.GetCount(), 3);

    UInt64 loopCount = 0;

    for (const auto& [id, size, data]: componentData)
    {
        switch (id)
        {
            case TestComponent1::Id:
            {
                EXPECT_EQ(size, sizeof(TestComponent1));

                auto* component = (TestComponent1*) data;
                EXPECT_EQ(component->A, 1);
                EXPECT_EQ(component->B, 2);

                loopCount++;

                break;
            }
            case TestComponent2::Id:
            {
                EXPECT_EQ(size, sizeof(TestComponent2));

                auto* component = (TestComponent2*) data;
                EXPECT_EQ(component->A, 3);
                EXPECT_EQ(component->B, 4);

                loopCount++;

                break;
            }
            case TestComponent3::Id:
            {
                EXPECT_EQ(size, sizeof(TestComponent3));

                auto* component = (TestComponent3*) data;
                EXPECT_EQ(component->A, 5);
                EXPECT_EQ(component->B, 6);

                loopCount++;

                break;
            }
            default:
                FAIL();
                break;
        }
    }

    EXPECT_EQ(loopCount, 3);

    loopCount = 0;
    componentData.Remove(TestComponent2::Id);
    bool comp2Found = false;

    EXPECT_EQ(componentData.GetCount(), 2);

    for (const auto& [id, size, data]: componentData)
    {
        switch (id)
        {
            case TestComponent1::Id:
            {
                EXPECT_EQ(size, sizeof(TestComponent1));

                auto* component = (TestComponent1*) data;
                EXPECT_EQ(component->A, 1);
                EXPECT_EQ(component->B, 2);

                loopCount++;

                break;
            }
            case TestComponent2::Id:
            {
                comp2Found = true;
                loopCount++;

                break;
            }
            case TestComponent3::Id:
            {
                EXPECT_EQ(size, sizeof(TestComponent3));

                auto* component = (TestComponent3*) data;
                EXPECT_EQ(component->A, 5);
                EXPECT_EQ(component->B, 6);

                loopCount++;

                break;
            }
            default:
                FAIL();
                break;
        }
    }

    EXPECT_EQ(loopCount, 2);
    EXPECT_FALSE(comp2Found);
}