#include <gtest/gtest.h>

#include "ECS/EntityManager.h"

using EntityManager = Otter::EntityManager;

class EntityManager_Fixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Otter::MemorySystem::Initialise(16_KiB);
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

struct TestComponent3 final : public Otter::IComponent
{
    static constexpr Otter::ComponentId Id = 3;

    int E;
    int F;

    TestComponent3() = default;

    TestComponent3(int e, int f)
        : E(e), F(f)
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

    auto archetype1 = manager.CreateArchetype()
        .With<TestComponent1>()
        .Build();

    auto archetype2 = manager.CreateArchetype()
        .With<TestComponent1>()
        .With<TestComponent2>()
        .Build();

    EXPECT_EQ(manager.GetArchetypeCount(), 0) << "Archetype is added on manager refresh";
    manager.RefreshManagerData();

    EXPECT_EQ(manager.GetArchetypeCount(), 2);

    EXPECT_EQ(manager.GetEntityCount(), 0);
    EXPECT_EQ(manager.GetComponentCount(), 2);

    EXPECT_EQ(archetype1.GetEntityCount(), 0);
    EXPECT_EQ(archetype1.GetComponentCount(), 1);
    EXPECT_EQ(archetype2.GetEntityCount(), 0);
    EXPECT_EQ(archetype2.GetComponentCount(), 2);
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

TEST_F(EntityManager_Fixture, AddGetRemoveSingleComponent_SingleEntity)
{
    EntityManager manager;
    manager.RegisterComponents<TestComponent1, TestComponent2, TestComponent3>()
        .LockComponents();

    auto entity = manager.CreateEntity()
        .SetComponentData<TestComponent1>(1, 2)
        .SetComponentData<TestComponent2>(3, 4)
        .Build();

    EXPECT_TRUE(entity.IsValid());

    manager.RefreshManagerData();

    EXPECT_TRUE(manager.HasComponent<TestComponent1>(entity));
    EXPECT_TRUE(manager.HasComponent<TestComponent2>(entity));
    EXPECT_FALSE(manager.HasComponent<TestComponent3>(entity));

    EXPECT_EQ(manager.GetEntityCount(), 1);
    EXPECT_EQ(manager.GetArchetypeCount(), 1);
    EXPECT_EQ(manager.GetComponentCount(), 3);

    auto* comp1 = manager.GetComponent<TestComponent1>(entity);

    EXPECT_EQ(comp1->A, 1);
    EXPECT_EQ(comp1->B, 2);

    EXPECT_TRUE(manager.TryAddComponent<TestComponent3>(entity, 5, 6));

    manager.RefreshManagerData();

    EXPECT_TRUE(manager.HasComponent<TestComponent1>(entity));
    EXPECT_TRUE(manager.HasComponent<TestComponent2>(entity));
    EXPECT_TRUE(manager.HasComponent<TestComponent3>(entity));

    EXPECT_EQ(manager.GetEntityCount(), 1);
    EXPECT_EQ(manager.GetArchetypeCount(), 2);
    EXPECT_EQ(manager.GetComponentCount(), 3);

    auto* comp3 = manager.GetComponent<TestComponent3>(entity);

    EXPECT_EQ(comp3->E, 5);
    EXPECT_EQ(comp3->F, 6);

    EXPECT_TRUE(manager.TryRemoveComponent<TestComponent2>(entity));

    manager.RefreshManagerData();

    EXPECT_TRUE(manager.HasComponent<TestComponent1>(entity));
    EXPECT_FALSE(manager.HasComponent<TestComponent2>(entity));
    EXPECT_TRUE(manager.HasComponent<TestComponent3>(entity));

    EXPECT_EQ(manager.GetEntityCount(), 1);
    EXPECT_EQ(manager.GetArchetypeCount(), 3);
    EXPECT_EQ(manager.GetComponentCount(), 3);
}

TEST_F(EntityManager_Fixture, AddGetRemoveMultipleComponents_SingleEntity)
{
    EntityManager manager;
    manager.RegisterComponents<TestComponent1, TestComponent2, TestComponent3>()
        .LockComponents();

    auto entity = manager.CreateEntity()
        .SetComponentData<TestComponent1>(1, 2)
        .Build();

    EXPECT_TRUE(entity.IsValid());

    manager.RefreshManagerData();

    EXPECT_TRUE(manager.HasComponent<TestComponent1>(entity));
    EXPECT_FALSE(manager.HasComponent<TestComponent2>(entity));
    EXPECT_FALSE(manager.HasComponent<TestComponent3>(entity));

    EXPECT_EQ(manager.GetEntityCount(), 1);
    EXPECT_EQ(manager.GetArchetypeCount(), 1);
    EXPECT_EQ(manager.GetComponentCount(), 3);

    auto* comp1 = manager.GetComponent<TestComponent1>(entity);

    EXPECT_EQ(comp1->A, 1);
    EXPECT_EQ(comp1->B, 2);

    EXPECT_TRUE(manager.TryAddComponent<TestComponent2>(entity, 3, 4));
    EXPECT_TRUE(manager.TryAddComponent<TestComponent3>(entity, 5, 6));

    manager.RefreshManagerData();

    EXPECT_TRUE(manager.HasComponent<TestComponent1>(entity));
    EXPECT_TRUE(manager.HasComponent<TestComponent2>(entity));
    EXPECT_TRUE(manager.HasComponent<TestComponent3>(entity));

    EXPECT_EQ(manager.GetEntityCount(), 1);
    EXPECT_EQ(manager.GetArchetypeCount(), 2);
    EXPECT_EQ(manager.GetComponentCount(), 3);

    auto* comp2 = manager.GetComponent<TestComponent2>(entity);

    EXPECT_EQ(comp2->C, 3);
    EXPECT_EQ(comp2->D, 4);

    auto* comp3 = manager.GetComponent<TestComponent3>(entity);

    EXPECT_EQ(comp3->E, 5);
    EXPECT_EQ(comp3->F, 6);

    EXPECT_TRUE(manager.TryRemoveComponent<TestComponent1>(entity));
    EXPECT_TRUE(manager.TryRemoveComponent<TestComponent2>(entity));

    manager.RefreshManagerData();

    EXPECT_FALSE(manager.HasComponent<TestComponent1>(entity));
    EXPECT_FALSE(manager.HasComponent<TestComponent2>(entity));
    EXPECT_TRUE(manager.HasComponent<TestComponent3>(entity));

    EXPECT_EQ(manager.GetEntityCount(), 1);
    EXPECT_EQ(manager.GetArchetypeCount(), 3);
    EXPECT_EQ(manager.GetComponentCount(), 3);

    comp3 = manager.GetComponent<TestComponent3>(entity);

    EXPECT_EQ(comp3->E, 5);
    EXPECT_EQ(comp3->F, 6);
}

TEST_F(EntityManager_Fixture, AddGetRemoveSingleAndMultipleComponents_MultipleEntities)
{
    EntityManager manager;
    manager.RegisterComponents<TestComponent1, TestComponent2, TestComponent3>()
        .LockComponents();

    auto entity1 = manager.CreateEntity()
        .SetComponentData<TestComponent1>(1, 2)
        .Build();

    auto entity2 = manager.CreateEntity()
        .SetComponentData<TestComponent1>(7, 8)
        .SetComponentData<TestComponent2>(9, 10)
        .Build();

    EXPECT_TRUE(entity1.IsValid());
    EXPECT_TRUE(entity2.IsValid());

    manager.RefreshManagerData();

    EXPECT_TRUE(manager.HasComponent<TestComponent1>(entity1));
    EXPECT_FALSE(manager.HasComponent<TestComponent2>(entity1));
    EXPECT_FALSE(manager.HasComponent<TestComponent3>(entity1));

    EXPECT_TRUE(manager.HasComponent<TestComponent1>(entity2));
    EXPECT_TRUE(manager.HasComponent<TestComponent2>(entity2));
    EXPECT_FALSE(manager.HasComponent<TestComponent3>(entity2));

    auto* comp1a = manager.GetComponent<TestComponent1>(entity1);

    EXPECT_EQ(comp1a->A, 1);
    EXPECT_EQ(comp1a->B, 2);

    auto* comp1b = manager.GetComponent<TestComponent1>(entity2);

    EXPECT_EQ(comp1b->A, 7);
    EXPECT_EQ(comp1b->B, 8);

    auto* comp2b = manager.GetComponent<TestComponent2>(entity2);

    EXPECT_EQ(comp2b->C, 9);
    EXPECT_EQ(comp2b->D, 10);

    EXPECT_EQ(manager.GetEntityCount(), 2);
    EXPECT_EQ(manager.GetArchetypeCount(), 2);
    EXPECT_EQ(manager.GetComponentCount(), 3);

    EXPECT_TRUE(manager.TryAddComponent<TestComponent2>(entity1, 3, 4));
    EXPECT_TRUE(manager.TryAddComponent<TestComponent3>(entity1, 5, 6));
    EXPECT_TRUE(manager.TryAddComponent<TestComponent3>(entity2, 11, 12));

    manager.RefreshManagerData();

    EXPECT_TRUE(manager.HasComponent<TestComponent1>(entity1));
    EXPECT_TRUE(manager.HasComponent<TestComponent2>(entity1));
    EXPECT_TRUE(manager.HasComponent<TestComponent3>(entity1));

    EXPECT_TRUE(manager.HasComponent<TestComponent1>(entity2));
    EXPECT_TRUE(manager.HasComponent<TestComponent2>(entity2));
    EXPECT_TRUE(manager.HasComponent<TestComponent3>(entity2));

    auto* comp2a = manager.GetComponent<TestComponent2>(entity1);

    EXPECT_EQ(comp2a->C, 3);
    EXPECT_EQ(comp2a->D, 4);

    auto* comp3a = manager.GetComponent<TestComponent3>(entity1);

    EXPECT_EQ(comp3a->E, 5);
    EXPECT_EQ(comp3a->F, 6);

    auto* comp3b = manager.GetComponent<TestComponent3>(entity2);

    EXPECT_EQ(comp3b->E, 11);
    EXPECT_EQ(comp3b->F, 12);

    EXPECT_EQ(manager.GetEntityCount(), 2);
    EXPECT_EQ(manager.GetArchetypeCount(), 3);
    EXPECT_EQ(manager.GetComponentCount(), 3);

    EXPECT_TRUE(manager.TryRemoveComponent<TestComponent1>(entity1));
    EXPECT_TRUE(manager.TryRemoveComponent<TestComponent2>(entity1));
    EXPECT_TRUE(manager.TryRemoveComponent<TestComponent2>(entity2));

    manager.RefreshManagerData();

    EXPECT_FALSE(manager.HasComponent<TestComponent1>(entity1));
    EXPECT_FALSE(manager.HasComponent<TestComponent2>(entity1));
    EXPECT_TRUE(manager.HasComponent<TestComponent3>(entity1));

    EXPECT_TRUE(manager.HasComponent<TestComponent1>(entity2));
    EXPECT_FALSE(manager.HasComponent<TestComponent2>(entity2));
    EXPECT_TRUE(manager.HasComponent<TestComponent3>(entity2));

    EXPECT_EQ(manager.GetEntityCount(), 2);
    EXPECT_EQ(manager.GetArchetypeCount(), 5);
    EXPECT_EQ(manager.GetComponentCount(), 3);
}

TEST_F(EntityManager_Fixture, ForEach_SingleEntity)
{
    EntityManager manager;
    manager.RegisterComponents<TestComponent1, TestComponent2, TestComponent3>()
        .LockComponents();

    auto entity = manager.CreateEntity()
        .SetComponentData<TestComponent1>(1, 2)
        .SetComponentData<TestComponent2>(3, 4)
        .SetComponentData<TestComponent3>(5, 6)
        .Build();

    EXPECT_TRUE(entity.IsValid());

    manager.RefreshManagerData();

    EXPECT_TRUE(manager.HasComponent<TestComponent1>(entity));
    EXPECT_TRUE(manager.HasComponent<TestComponent2>(entity));
    EXPECT_TRUE(manager.HasComponent<TestComponent3>(entity));

    EXPECT_EQ(manager.GetEntityCount(), 1);
    EXPECT_EQ(manager.GetArchetypeCount(), 1);
    EXPECT_EQ(manager.GetComponentCount(), 3);

    UInt64 runCount = 0;

    manager.ForEach<TestComponent1>(
        {
            [&](TestComponent1* c1)
            {
                EXPECT_EQ(c1->A, 1);
                EXPECT_EQ(c1->B, 2);

                c1->A = 10;
                c1->B = 20;

                ++runCount;
            }
        });

    EXPECT_EQ(runCount, 1);
    runCount = 0;

    manager.ForEach<TestComponent1, TestComponent2, TestComponent3>(
        {
            [&](TestComponent1* c1, TestComponent2* c2, TestComponent3* c3)
            {
                EXPECT_EQ(c1->A, 10);
                EXPECT_EQ(c1->B, 20);

                EXPECT_EQ(c2->C, 3);
                EXPECT_EQ(c2->D, 4);

                EXPECT_EQ(c3->E, 5);
                EXPECT_EQ(c3->F, 6);

                ++runCount;
            }
        });

    EXPECT_EQ(runCount, 1);
}

TEST_F(EntityManager_Fixture, ForEach_MultipleEntities)
{
    EntityManager manager;
    manager.RegisterComponents<TestComponent1, TestComponent2, TestComponent3>()
        .LockComponents();

    auto entity1 = manager.CreateEntity()
        .SetComponentData<TestComponent1>(1, 2)
        .SetComponentData<TestComponent2>(3, 4)
        .Build();

    auto entity2 = manager.CreateEntity()
        .SetComponentData<TestComponent1>(1, 2)
        .SetComponentData<TestComponent2>(3, 4)
        .SetComponentData<TestComponent3>(5, 6)
        .Build();

    EXPECT_TRUE(entity1.IsValid());
    EXPECT_TRUE(entity2.IsValid());

    manager.RefreshManagerData();

    EXPECT_TRUE(manager.HasComponent<TestComponent1>(entity1));
    EXPECT_TRUE(manager.HasComponent<TestComponent2>(entity1));
    EXPECT_FALSE(manager.HasComponent<TestComponent3>(entity1));

    EXPECT_TRUE(manager.HasComponent<TestComponent1>(entity2));
    EXPECT_TRUE(manager.HasComponent<TestComponent2>(entity2));
    EXPECT_TRUE(manager.HasComponent<TestComponent3>(entity2));

    EXPECT_EQ(manager.GetEntityCount(), 2);
    EXPECT_EQ(manager.GetArchetypeCount(), 2);
    EXPECT_EQ(manager.GetComponentCount(), 3);

    UInt64 runCount = 0;

    manager.ForEach<TestComponent1>(
        {
            [&](TestComponent1* c1)
            {
                ++runCount;
            }
        });

    EXPECT_EQ(runCount, 2);
    runCount = 0;

    manager.ForEach<TestComponent1, TestComponent2>(
        {
            [&](TestComponent1* c1, TestComponent2* c2)
            {
                ++runCount;
            }
        });

    EXPECT_EQ(runCount, 2);
    runCount = 0;

    manager.ForEach<TestComponent1, TestComponent2, TestComponent3>(
        {
            [&](TestComponent1* c1, TestComponent2* c2, TestComponent3* c3)
            {
                ++runCount;
            }
        });

    EXPECT_EQ(runCount, 1);
}

TEST_F(EntityManager_Fixture, ForEach_MemoryCheck)
{
    EntityManager manager;
    manager.RegisterComponents<TestComponent1, TestComponent2, TestComponent3>()
        .LockComponents();

    auto entity = manager.CreateEntity()
        .SetComponentData<TestComponent1>(1, 2)
        .SetComponentData<TestComponent2>(3, 4)
        .SetComponentData<TestComponent3>(5, 6)
        .Build();

    EXPECT_TRUE(entity.IsValid());

    manager.RefreshManagerData();

    EXPECT_TRUE(manager.HasComponent<TestComponent1>(entity));
    EXPECT_TRUE(manager.HasComponent<TestComponent2>(entity));
    EXPECT_TRUE(manager.HasComponent<TestComponent3>(entity));

    EXPECT_EQ(manager.GetEntityCount(), 1);
    EXPECT_EQ(manager.GetArchetypeCount(), 1);
    EXPECT_EQ(manager.GetComponentCount(), 3);

    UInt64 memoryUsed = Otter::MemorySystem::GetUsedMemory();

    for (UInt64 i = 0; i < 10; ++i)
    {
        manager.ForEach<TestComponent1>(
            {
                [&](TestComponent1* c1)
                {
                    // Do nothing
                }
            });

        EXPECT_EQ(memoryUsed, Otter::MemorySystem::GetUsedMemory());
    }

    for (UInt64 i = 0; i < 10; ++i)
    {
        manager.ForEach<TestComponent1, TestComponent2, TestComponent3>(
            {
                [&](TestComponent1* c1, TestComponent2* c2, TestComponent3* c3)
                {
                    // Do nothing
                }
            });

        EXPECT_EQ(memoryUsed, Otter::MemorySystem::GetUsedMemory());
    }

    EXPECT_EQ(memoryUsed, Otter::MemorySystem::GetUsedMemory());
}