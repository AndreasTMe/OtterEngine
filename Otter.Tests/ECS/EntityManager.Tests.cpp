#include <gtest/gtest.h>

#include "ECS/EntityManager.h"

using EntityManager = Otter::EntityManager;

class EntityManager_Fixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Otter::MemorySystem::Initialise(8_KiB);
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

    int C;
    int D;

    TestComponent2() = default;

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
    manager.RefreshManagerData();

    EXPECT_EQ(manager.GetArchetypeCount(), 1);

    EXPECT_EQ(manager.GetEntityCount(), 0);
    EXPECT_EQ(manager.GetComponentCount(), 2);

    EXPECT_EQ(archetype.GetEntityCount(), 0);
    EXPECT_EQ(archetype.GetComponentCount(), 2);
}

TEST_F(EntityManager_Fixture, CreateArchetype_InvalidComponentRegistration)
{
    EntityManager manager;

    EXPECT_DEATH(auto _ = manager.CreateArchetype(), "");

    manager.RegisterComponents<TestComponent1>()
        .LockComponents();

    EXPECT_DEATH(auto _ = manager.CreateArchetype()
        .With<TestComponent2>()
        .Build(), "");
}

TEST_F(EntityManager_Fixture, CreateEntity_Success)
{
    EntityManager manager;
    manager.RegisterComponents<TestComponent1, TestComponent2>()
        .LockComponents();

    auto entity = manager.CreateEntity()
        .SetComponentData<TestComponent1>(1, 2)
        .SetComponentData<TestComponent2>(3, 4)
        .Build();

    EXPECT_TRUE(entity.IsValid());

    EXPECT_EQ(manager.GetEntityCount(), 0) << "Entity is added on manager refresh";
    manager.RefreshManagerData();

    EXPECT_EQ(manager.GetEntityCount(), 1);
    EXPECT_EQ(manager.GetArchetypeCount(), 1);
    EXPECT_EQ(manager.GetComponentCount(), 2);
}

TEST_F(EntityManager_Fixture, CreateEntity_InvalidComponentRegistration)
{
    EntityManager manager;

    EXPECT_DEATH(auto _ = manager.CreateEntity(), "");

    manager.RegisterComponents<TestComponent1>()
        .LockComponents();

    EXPECT_DEATH(auto _ = manager.CreateEntity()
        .SetComponentData<TestComponent2>()
        .Build(), "");
}

TEST_F(EntityManager_Fixture, CreateEntityFromArchetype_Success)
{
    EntityManager manager;
    manager.RegisterComponents<TestComponent1, TestComponent2>()
        .LockComponents();

    auto archetype = manager.CreateArchetype()
        .With<TestComponent1>()
        .With<TestComponent2>()
        .Build();

    auto entity = manager.CreateEntityFromArchetype(archetype)
        .SetComponentData<TestComponent1>(1, 2)
        .SetComponentData<TestComponent2>(3, 4)
        .Build();

    EXPECT_TRUE(entity.IsValid());

    EXPECT_EQ(manager.GetEntityCount(), 0) << "Entity is added on manager refresh";
    manager.RefreshManagerData();

    EXPECT_EQ(manager.GetEntityCount(), 1);
    EXPECT_EQ(manager.GetArchetypeCount(), 1);
    EXPECT_EQ(manager.GetComponentCount(), 2);
}

TEST_F(EntityManager_Fixture, CreateEntityFromArchetype_InvalidComponentRegistration)
{
    EntityManager manager;

    EXPECT_DEATH(auto _ = manager.CreateEntity(), "");

    manager.RegisterComponents<TestComponent1>()
        .LockComponents();

    auto archetype = manager.CreateArchetype()
        .With<TestComponent1>()
        .Build();

    EXPECT_DEATH(auto _ = manager.CreateEntityFromArchetype(archetype)
        .SetComponentData<TestComponent2>()
        .Build(), "");
}

TEST_F(EntityManager_Fixture, DestroyEntity)
{
    EntityManager manager;
    manager.RegisterComponents<TestComponent1, TestComponent2>()
        .LockComponents();

    auto entity = manager.CreateEntity()
        .SetComponentData<TestComponent1>(1, 2)
        .SetComponentData<TestComponent2>(3, 4)
        .Build();

    EXPECT_TRUE(entity.IsValid());

    manager.RefreshManagerData();

    EXPECT_EQ(manager.GetEntityCount(), 1);
    EXPECT_EQ(manager.GetArchetypeCount(), 1);
    EXPECT_EQ(manager.GetComponentCount(), 2);

    manager.DestroyEntity(entity);

    EXPECT_EQ(manager.GetEntityCount(), 1) << "Entity is removed on manager refresh";

    manager.RefreshManagerData();

    EXPECT_EQ(manager.GetEntityCount(), 0);
    EXPECT_EQ(manager.GetArchetypeCount(), 1);
    EXPECT_EQ(manager.GetComponentCount(), 2);
}