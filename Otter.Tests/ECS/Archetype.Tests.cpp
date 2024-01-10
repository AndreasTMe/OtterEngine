#include <gtest/gtest.h>

#include "ECS/Archetype.h"

using ArchetypeFingerprint = Otter::ArchetypeFingerprint;

using Archetype = Otter::Archetype;

class Archetype_Fixture : public ::testing::Test
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

TEST_F(Archetype_Fixture, DefaultConstructor)
{
    Archetype archetype;

    EXPECT_EQ(archetype.GetEntityCount(), 0);
    EXPECT_EQ(archetype.GetComponentCount(), 0);
}

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

TEST_F(Archetype_Fixture, Constructor)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);
    fingerprint.Set(1, true);

    Otter::List<Otter::ComponentId> componentIds = { TestComponent1::Id, TestComponent2::Id };

    Archetype archetype(fingerprint, componentIds);

    EXPECT_EQ(archetype.GetEntityCount(), 0);
    EXPECT_EQ(archetype.GetComponentCount(), 2);
}

TEST_F(Archetype_Fixture, CopyConstructor)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);
    fingerprint.Set(1, true);

    Otter::List<Otter::ComponentId> componentIds = { TestComponent1::Id, TestComponent2::Id };

    Archetype archetype1(fingerprint, componentIds);
    Archetype archetype2(archetype1);

    EXPECT_EQ(archetype1.GetEntityCount(), 0);
    EXPECT_EQ(archetype1.GetComponentCount(), 2);

    EXPECT_EQ(archetype2.GetEntityCount(), 0);
    EXPECT_EQ(archetype2.GetComponentCount(), 2);
}

TEST_F(Archetype_Fixture, MoveConstructor)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);
    fingerprint.Set(1, true);

    Otter::List<Otter::ComponentId> componentIds = { TestComponent1::Id, TestComponent2::Id };

    Archetype archetype1(fingerprint, componentIds);
    Archetype archetype2(std::move(archetype1));

    EXPECT_EQ(archetype1.GetEntityCount(), 0);
    EXPECT_EQ(archetype1.GetComponentCount(), 0);

    EXPECT_EQ(archetype2.GetEntityCount(), 0);
    EXPECT_EQ(archetype2.GetComponentCount(), 2);
}

TEST_F(Archetype_Fixture, Assignment_Copy)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);
    fingerprint.Set(1, true);

    Otter::List<Otter::ComponentId> componentIds = { TestComponent1::Id, TestComponent2::Id };

    Archetype archetype1(fingerprint, componentIds);
    Archetype archetype2;

    archetype2 = archetype1;

    EXPECT_EQ(archetype1.GetEntityCount(), 0);
    EXPECT_EQ(archetype1.GetComponentCount(), 2);

    EXPECT_EQ(archetype2.GetEntityCount(), 0);
    EXPECT_EQ(archetype2.GetComponentCount(), 2);
}

TEST_F(Archetype_Fixture, Assignment_Move)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);
    fingerprint.Set(1, true);

    Otter::List<Otter::ComponentId> componentIds = { TestComponent1::Id, TestComponent2::Id };

    Archetype archetype1(fingerprint, componentIds);
    Archetype archetype2;

    archetype2 = std::move(archetype1);

    EXPECT_EQ(archetype1.GetEntityCount(), 0);
    EXPECT_EQ(archetype1.GetComponentCount(), 0);

    EXPECT_EQ(archetype2.GetEntityCount(), 0);
    EXPECT_EQ(archetype2.GetComponentCount(), 2);
}

TEST_F(Archetype_Fixture, Equality)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);
    fingerprint.Set(1, true);

    Otter::List<Otter::ComponentId> componentIds = { TestComponent1::Id, TestComponent2::Id };

    Archetype archetype1(fingerprint, componentIds);
    Archetype archetype2(fingerprint, componentIds);
    Archetype archetype3;

    EXPECT_TRUE(archetype1 == archetype2);
    EXPECT_FALSE(archetype1 == archetype3);
}

TEST_F(Archetype_Fixture, AddingGettingAndRemoving_ComponentData)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);

    Otter::List<Otter::ComponentId> componentIds = { TestComponent1::Id };

    Archetype archetype(fingerprint, componentIds);

    TestComponent1                    component(1, 2);
    Otter::List<Otter::ComponentData> componentData;
    componentData.Add(Otter::ComponentData{ TestComponent1::Id, (Byte*) &component, sizeof(TestComponent1) });

    Otter::EntityId entityId = 1;
    EXPECT_TRUE(archetype.TryAddComponentData(entityId, componentData));

    EXPECT_EQ(archetype.GetEntityCount(), 1);
    EXPECT_EQ(archetype.GetComponentCount(), 1);

    Otter::List<Otter::ComponentData> storedComponentData;
    EXPECT_TRUE(archetype.TryGetAllComponentData(entityId, storedComponentData));

    EXPECT_EQ(storedComponentData.GetCount(), 1);
    for (auto& data: storedComponentData)
    {
        EXPECT_EQ(data.Id, TestComponent1::Id);
        EXPECT_EQ((*(TestComponent1*) data.Data).A, component.A);
        EXPECT_EQ((*(TestComponent1*) data.Data).B, component.B);
        EXPECT_EQ(data.Size, sizeof(TestComponent1));
    }

    EXPECT_TRUE(archetype.TryRemoveComponentData(entityId));

    EXPECT_EQ(archetype.GetEntityCount(), 0);
    EXPECT_EQ(archetype.GetComponentCount(), 1);
}

TEST_F(Archetype_Fixture, HasComponent)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);

    Otter::List<Otter::ComponentId> componentIds = { TestComponent1::Id };

    Archetype archetype(fingerprint, componentIds);

    EXPECT_TRUE(archetype.HasComponent<TestComponent1>());
}

TEST_F(Archetype_Fixture, GetComponents_Single)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);

    Otter::List<Otter::ComponentId> componentIds = { TestComponent1::Id };

    Archetype archetype(fingerprint, componentIds);

    TestComponent1                    component(1, 2);
    Otter::List<Otter::ComponentData> componentData;
    componentData.Add(Otter::ComponentData{ TestComponent1::Id, (Byte*) &component, sizeof(TestComponent1) });

    Otter::EntityId entityId = 1;
    EXPECT_TRUE(archetype.TryAddComponentData(entityId, componentData));

    EXPECT_EQ(archetype.GetEntityCount(), 1);
    EXPECT_EQ(archetype.GetComponentCount(), 1);

    auto comp = archetype.GetComponents<TestComponent1>();

    EXPECT_EQ(comp->A, 1);
    EXPECT_EQ(comp->B, 2);

    comp->A = 3;
    comp->B = 4;

    comp = archetype.GetComponents<TestComponent1>();

    EXPECT_EQ(comp->A, 3);
    EXPECT_EQ(comp->B, 4);
}

TEST_F(Archetype_Fixture, GetComponents_Multiple)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);
    fingerprint.Set(1, true);

    Otter::List<Otter::ComponentId> componentIds = { TestComponent1::Id };

    EXPECT_DEATH(Archetype(fingerprint, componentIds), "");

    componentIds.Add(TestComponent2::Id);

    Archetype archetype(fingerprint, componentIds);

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

TEST_F(Archetype_Fixture, GetComponentsForEntity_Single)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);

    Otter::List<Otter::ComponentId> componentIds = { TestComponent1::Id };

    Archetype archetype(fingerprint, componentIds);

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

TEST_F(Archetype_Fixture, GetComponentsForEntity_Multiple)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);
    fingerprint.Set(1, true);

    Otter::List<Otter::ComponentId> componentIds = { TestComponent1::Id, TestComponent2::Id };

    Archetype archetype(fingerprint, componentIds);

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