#include <gtest/gtest.h>

#include "ECS/Archetype.h"

using Archetype = Otter::Archetype;

class Archetype_Fixture : public ::testing::Test
{
protected:
    void SetUp() override
    {
        Otter::MemorySystem::Initialise(1_KiB);
    }

    void TearDown() override
    {
        Otter::MemorySystem::Shutdown();
    }
};

TEST_F(Archetype_Fixture, DefaultConstructor)
{
    {
        Archetype archetype;

        EXPECT_EQ(archetype.GetEntityCount(), 0);
        EXPECT_EQ(archetype.GetComponentCount(), 0);
    }

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
}

TEST_F(Archetype_Fixture, Constructor)
{
    {
        Otter::BitSet componentMask;
        componentMask.Set(0, true);
        componentMask.Set(1, true);

        Otter::List<Otter::ComponentId> componentIds;
        componentIds.Add(0);
        componentIds.Add(1);

        Archetype archetype(componentMask, componentIds);

        EXPECT_EQ(archetype.GetEntityCount(), 0);
        EXPECT_EQ(archetype.GetComponentCount(), 2);
    }

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
}

TEST_F(Archetype_Fixture, CopyConstructor)
{
    {
        Otter::BitSet componentMask;
        componentMask.Set(0, true);
        componentMask.Set(1, true);

        Otter::List<Otter::ComponentId> componentIds;
        componentIds.Add(0);
        componentIds.Add(1);

        Archetype archetype1(componentMask, componentIds);
        Archetype archetype2(archetype1);

        EXPECT_EQ(archetype1.GetEntityCount(), 0);
        EXPECT_EQ(archetype1.GetComponentCount(), 2);

        EXPECT_EQ(archetype2.GetEntityCount(), 0);
        EXPECT_EQ(archetype2.GetComponentCount(), 2);
    }

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
}

TEST_F(Archetype_Fixture, MoveConstructor)
{
    {
        Otter::BitSet componentMask;
        componentMask.Set(0, true);
        componentMask.Set(1, true);

        Otter::List<Otter::ComponentId> componentIds;
        componentIds.Add(0);
        componentIds.Add(1);

        Archetype archetype1(componentMask, componentIds);
        Archetype archetype2(std::move(archetype1));

        EXPECT_EQ(archetype1.GetEntityCount(), 0);
        EXPECT_EQ(archetype1.GetComponentCount(), 0);

        EXPECT_EQ(archetype2.GetEntityCount(), 0);
        EXPECT_EQ(archetype2.GetComponentCount(), 2);
    }

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
}

TEST_F(Archetype_Fixture, Assignment_Copy)
{
    {
        Otter::BitSet componentMask;
        componentMask.Set(0, true);
        componentMask.Set(1, true);

        Otter::List<Otter::ComponentId> componentIds;
        componentIds.Add(0);
        componentIds.Add(1);

        Archetype archetype1(componentMask, componentIds);
        Archetype archetype2;

        archetype2 = archetype1;

        EXPECT_EQ(archetype1.GetEntityCount(), 0);
        EXPECT_EQ(archetype1.GetComponentCount(), 2);

        EXPECT_EQ(archetype2.GetEntityCount(), 0);
        EXPECT_EQ(archetype2.GetComponentCount(), 2);
    }

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
}

TEST_F(Archetype_Fixture, Assignment_Move)
{
    {
        Otter::BitSet componentMask;
        componentMask.Set(0, true);
        componentMask.Set(1, true);

        Otter::List<Otter::ComponentId> componentIds;
        componentIds.Add(0);
        componentIds.Add(1);

        Archetype archetype1(componentMask, componentIds);
        Archetype archetype2;

        archetype2 = std::move(archetype1);

        EXPECT_EQ(archetype1.GetEntityCount(), 0);
        EXPECT_EQ(archetype1.GetComponentCount(), 0);

        EXPECT_EQ(archetype2.GetEntityCount(), 0);
        EXPECT_EQ(archetype2.GetComponentCount(), 2);
    }

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
}

TEST_F(Archetype_Fixture, Equality)
{
    {
        Otter::BitSet componentMask;
        componentMask.Set(0, true);
        componentMask.Set(1, true);

        Otter::List<Otter::ComponentId> componentIds;
        componentIds.Add(0);
        componentIds.Add(1);

        Archetype archetype1(componentMask, componentIds);
        Archetype archetype2(componentMask, componentIds);
        Archetype archetype3;

        EXPECT_TRUE(archetype1 == archetype2);
        EXPECT_FALSE(archetype1 == archetype3);
    }

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
}

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

    int A;
    int B;

    TestComponent2(int a, int b)
        : A(a), B(b)
    {
    }
};

TEST_F(Archetype_Fixture, Adding_And_Removing_ComponentData)
{
    {
        Otter::BitSet componentMask;
        componentMask.Set(0, true);

        Otter::List<Otter::ComponentId> componentIds;
        componentIds.Add(TestComponent1::Id);

        Archetype archetype(componentMask, componentIds);

        TestComponent1                    component(1, 2);
        Otter::List<Otter::ComponentData> componentData;
        componentData.Add(Otter::ComponentData{ TestComponent1::Id, (Byte*) &component, sizeof(TestComponent1) });

        Otter::EntityId entityId = 1;
        EXPECT_TRUE(archetype.TryAddComponentData(entityId, componentData));

        EXPECT_EQ(archetype.GetEntityCount(), 1);
        EXPECT_EQ(archetype.GetComponentCount(), 1);

        EXPECT_TRUE(archetype.TryRemoveComponentData(entityId));

        EXPECT_EQ(archetype.GetEntityCount(), 0);
        EXPECT_EQ(archetype.GetComponentCount(), 1);
    }

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
}

TEST_F(Archetype_Fixture, HasComponent)
{
    {
        Otter::BitSet componentMask;
        componentMask.Set(0, true);

        Otter::ComponentId              id = 1;
        Otter::List<Otter::ComponentId> componentIds;
        componentIds.Add(id);

        Archetype archetype(componentMask, componentIds);

        EXPECT_TRUE(archetype.HasComponent<TestComponent1>());
    }

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
}

TEST_F(Archetype_Fixture, GetComponents_Single)
{
    {
        Otter::BitSet componentMask;
        componentMask.Set(0, true);

        Otter::List<Otter::ComponentId> componentIds;
        componentIds.Add(TestComponent1::Id);

        Archetype archetype(componentMask, componentIds);

        TestComponent1                    component(1, 2);
        Otter::List<Otter::ComponentData> componentData;
        componentData.Add(Otter::ComponentData{ TestComponent1::Id, (Byte*) &component, sizeof(TestComponent1) });

        Otter::EntityId entityId = 1;
        EXPECT_TRUE(archetype.TryAddComponentData(entityId, componentData));

        EXPECT_EQ(archetype.GetEntityCount(), 1);
        EXPECT_EQ(archetype.GetComponentCount(), 1);

        auto* testComponent = archetype.GetComponents<TestComponent1>();

        EXPECT_EQ(testComponent->A, 1);
        EXPECT_EQ(testComponent->B, 2);

        testComponent->A = 3;
        testComponent->B = 4;

        testComponent = archetype.GetComponents<TestComponent1>();

        EXPECT_EQ(testComponent->A, 3);
        EXPECT_EQ(testComponent->B, 4);
    }

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
}

TEST_F(Archetype_Fixture, GetComponents_Multiple)
{
    {
        Otter::BitSet componentMask;
        componentMask.Set(0, true);
        componentMask.Set(1, true);

        Otter::List<Otter::ComponentId> componentIds;
        componentIds.Add(TestComponent1::Id);

        EXPECT_DEATH(Archetype(componentMask, componentIds), "");

        componentIds.Add(TestComponent2::Id);

        Archetype archetype(componentMask, componentIds);

        TestComponent1                    component1(1, 2);
        Otter::List<Otter::ComponentData> componentData;
        componentData.Add(Otter::ComponentData{ TestComponent1::Id, (Byte*) &component1, sizeof(TestComponent1) });

        Otter::EntityId entityId = 1;
        EXPECT_DEATH(archetype.TryAddComponentData(entityId, componentData), "");

        TestComponent2 component2(3, 4);
        componentData.Add(Otter::ComponentData{ TestComponent2::Id, (Byte*) &component2, sizeof(TestComponent2) });

        EXPECT_TRUE(archetype.TryAddComponentData(entityId, componentData));

        EXPECT_EQ(archetype.GetEntityCount(), 1);
        EXPECT_EQ(archetype.GetComponentCount(), 2);

        auto [comp1, comp2] = archetype.GetComponents<TestComponent1, TestComponent2>();
        EXPECT_EQ(comp1->A, 1);
        EXPECT_EQ(comp1->B, 2);
        EXPECT_EQ(comp2->A, 3);
        EXPECT_EQ(comp2->B, 4);

        comp1->A = 5;
        comp1->B = 6;
        comp2->A = 7;
        comp2->B = 8;

        std::tie(comp1, comp2) = archetype.GetComponents<TestComponent1, TestComponent2>();

        EXPECT_EQ(comp1->A, 5);
        EXPECT_EQ(comp1->B, 6);
        EXPECT_EQ(comp2->A, 7);
        EXPECT_EQ(comp2->B, 8);
    }

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
}

TEST_F(Archetype_Fixture, GetComponentsForEntity_Single)
{
    {
        Otter::BitSet componentMask;
        componentMask.Set(0, true);

        Otter::List<Otter::ComponentId> componentIds;
        componentIds.Add(TestComponent1::Id);

        Archetype archetype(componentMask, componentIds);

        TestComponent1                    component1(1, 2);
        Otter::List<Otter::ComponentData> componentData1;
        componentData1.Add(Otter::ComponentData{ TestComponent1::Id, (Byte*) &component1, sizeof(TestComponent1) });

        Otter::EntityId entityId1 = 1;
        EXPECT_TRUE(archetype.TryAddComponentData(entityId1, componentData1));

        TestComponent1                    component2(3, 4);
        Otter::List<Otter::ComponentData> componentData2;
        componentData2.Add(Otter::ComponentData{ TestComponent1::Id, (Byte*) &component2, sizeof(TestComponent1) });

        Otter::EntityId entityId2 = 2;
        EXPECT_TRUE(archetype.TryAddComponentData(entityId2, componentData2));

        EXPECT_EQ(archetype.GetEntityCount(), 2);
        EXPECT_EQ(archetype.GetComponentCount(), 1);

        EXPECT_DEATH(auto result = archetype.GetComponentsForEntity<TestComponent1>(123), "");

        auto* found = archetype.GetComponentsForEntity<TestComponent1>(entityId2);

        EXPECT_EQ(found->A, 3);
        EXPECT_EQ(found->B, 4);

        found->A = 5;
        found->B = 6;

        found = archetype.GetComponentsForEntity<TestComponent1>(entityId1);

        EXPECT_EQ(found->A, 1);
        EXPECT_EQ(found->B, 2);

        found = archetype.GetComponentsForEntity<TestComponent1>(entityId2);

        EXPECT_EQ(found->A, 5);
        EXPECT_EQ(found->B, 6);
    }

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
}

TEST_F(Archetype_Fixture, GetComponentsForEntity_Multiple)
{
    {
        Otter::BitSet componentMask;
        componentMask.Set(0, true);
        componentMask.Set(1, true);

        Otter::List<Otter::ComponentId> componentIds;
        componentIds.Add(TestComponent1::Id);
        componentIds.Add(TestComponent2::Id);

        Archetype archetype(componentMask, componentIds);

        TestComponent1                    component1a(1, 2);
        TestComponent2                    component1b(3, 4);
        Otter::List<Otter::ComponentData> componentData1;
        componentData1.Add(Otter::ComponentData{ TestComponent1::Id, (Byte*) &component1a, sizeof(TestComponent1) });
        componentData1.Add(Otter::ComponentData{ TestComponent2::Id, (Byte*) &component1b, sizeof(TestComponent2) });

        Otter::EntityId entityId1 = 1;
        EXPECT_TRUE(archetype.TryAddComponentData(entityId1, componentData1));

        TestComponent1                    component2a(5, 6);
        TestComponent2                    component2b(7, 8);
        Otter::List<Otter::ComponentData> componentData2;
        componentData2.Add(Otter::ComponentData{ TestComponent1::Id, (Byte*) &component2a, sizeof(TestComponent1) });
        componentData2.Add(Otter::ComponentData{ TestComponent2::Id, (Byte*) &component2b, sizeof(TestComponent2) });

        Otter::EntityId entityId2 = 2;
        EXPECT_TRUE(archetype.TryAddComponentData(entityId2, componentData2));

        EXPECT_EQ(archetype.GetEntityCount(), 2);
        EXPECT_EQ(archetype.GetComponentCount(), 2);

        auto [comp1, comp2] = archetype.GetComponentsForEntity<TestComponent1, TestComponent2>(entityId2);

        EXPECT_EQ(comp1->A, 5);
        EXPECT_EQ(comp1->B, 6);
        EXPECT_EQ(comp2->A, 7);
        EXPECT_EQ(comp2->B, 8);

        comp1->A = 9;
        comp1->B = 10;
        comp2->A = 11;
        comp2->B = 12;

        std::tie(comp1, comp2) = archetype.GetComponentsForEntity<TestComponent1, TestComponent2>(entityId1);

        EXPECT_EQ(comp1->A, 1);
        EXPECT_EQ(comp1->B, 2);
        EXPECT_EQ(comp2->A, 3);
        EXPECT_EQ(comp2->B, 4);

        std::tie(comp1, comp2) = archetype.GetComponentsForEntity<TestComponent1, TestComponent2>(entityId2);

        EXPECT_EQ(comp1->A, 9);
        EXPECT_EQ(comp1->B, 10);
        EXPECT_EQ(comp2->A, 11);
        EXPECT_EQ(comp2->B, 12);
    }

    EXPECT_EQ(Otter::MemorySystem::GetUsedMemory(), 0);
}