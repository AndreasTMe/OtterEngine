#include <gtest/gtest.h>

#include "ECS/EntityManager.h"

using EntityManager = Otter::EntityManager;

class EntityManager_Fixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Otter::MemorySystem::Initialise(2_KiB);
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

    TestComponent1(int a, int b)
        : A(a), B(b)
    {
    }
};

struct TestComponent2 final : public Otter::IComponent
{
    static constexpr Otter::ComponentId Id = 2;

    int C;
    int D;

    TestComponent2(int c, int d)
        : C(c), D(d)
    {
    }
};

TEST_F(EntityManager_Fixture, RegisterComponents)
{
    EntityManager manager;
    manager.RegisterComponents<TestComponent1, TestComponent2>()
        .LockComponents();

    EXPECT_EQ(manager.GetComponentCount(), 2);
}

TEST_F(EntityManager_Fixture, CreateArchetype_Success)
{
    EntityManager manager;
    manager.RegisterComponents<TestComponent1, TestComponent2>()
        .LockComponents();

    auto archetype = manager.CreateArchetype()
        .With<TestComponent1>()
        .With<TestComponent2>()
        .Build();

    EXPECT_EQ(manager.GetArchetypeCount(), 0) << "Archetype is added on manager refresh";
    EXPECT_EQ(manager.GetEntityCount(), 0);
    EXPECT_EQ(manager.GetComponentCount(), 2);

    EXPECT_EQ(archetype.GetEntityCount(), 0);
    EXPECT_EQ(archetype.GetComponentCount(), 2);
}

TEST_F(EntityManager_Fixture, CreateArchetype_InvalidComponentRegistration)
{
    EntityManager manager;

    EXPECT_DEATH(auto builder = manager.CreateArchetype(), "");

    manager.RegisterComponents<TestComponent1>()
        .LockComponents();

    EXPECT_DEATH(auto archetype = manager.CreateArchetype()
        .With<TestComponent2>()
        .Build(), "");
}
