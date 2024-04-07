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

TEST_F(Archetype_Fixture, GetComponentDataForEntityUnsafe)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);
    fingerprint.Set(1, true);
    fingerprint.Set(2, true);

    Otter::List<Otter::ComponentId> componentIds = { TestComponent1::Id, TestComponent2::Id, TestComponent3::Id };

    Archetype archetype(fingerprint, componentIds);

    Otter::EntityId    entityId1        = 1;
    Otter::ComponentId componentIds1[3] = { TestComponent1::Id, TestComponent2::Id, TestComponent3::Id };
    TestComponent1     component1a(1, 2);
    TestComponent2     component2a(3, 4);
    TestComponent3     component3a(5, 6);
    Byte               componentData1[sizeof(TestComponent1) + sizeof(TestComponent2) + sizeof(TestComponent3)];
    Otter::MemorySystem::MemoryCopy(&componentData1[0], (Byte*) &component1a, sizeof(TestComponent1));
    Otter::MemorySystem::MemoryCopy(&componentData1[sizeof(TestComponent1)],
                                    (Byte*) &component2a,
                                    sizeof(TestComponent2));
    Otter::MemorySystem::MemoryCopy(&componentData1[sizeof(TestComponent1) + sizeof(TestComponent2)],
                                    (Byte*) &component3a,
                                    sizeof(TestComponent3));
    UInt64 componentSizes1[3] = { sizeof(TestComponent1), sizeof(TestComponent2), sizeof(TestComponent3) };

    EXPECT_TRUE(archetype.TryAddComponentDataUnsafe(entityId1, componentIds1, componentSizes1, componentData1));

    Otter::EntityId    entityId2        = 2;
    Otter::ComponentId componentIds2[3] = { TestComponent1::Id, TestComponent2::Id, TestComponent3::Id };
    TestComponent1     component1b(7, 8);
    TestComponent2     component2b(9, 10);
    TestComponent3     component3b(11, 12);
    Byte               componentData2[sizeof(TestComponent1) + sizeof(TestComponent2) + sizeof(TestComponent3)];
    Otter::MemorySystem::MemoryCopy(&componentData2[0], (Byte*) &component1b, sizeof(TestComponent1));
    Otter::MemorySystem::MemoryCopy(&componentData2[sizeof(TestComponent1)],
                                    (Byte*) &component2b,
                                    sizeof(TestComponent2));
    Otter::MemorySystem::MemoryCopy(&componentData2[sizeof(TestComponent1) + sizeof(TestComponent2)],
                                    (Byte*) &component3b,
                                    sizeof(TestComponent3));
    UInt64 componentSizes2[3] = { sizeof(TestComponent1), sizeof(TestComponent2), sizeof(TestComponent3) };

    EXPECT_TRUE(archetype.TryAddComponentDataUnsafe(entityId2, componentIds2, componentSizes2, componentData2));

    EXPECT_EQ(archetype.GetEntityCount(), 2);
    EXPECT_EQ(archetype.GetComponentCount(), 3);

    Otter::ComponentData componentData;
    archetype.GetComponentDataForEntityUnsafe(entityId2, &componentData);

    UInt64 loopCount = 0;

    for (const auto& [id, size, data]: componentData)
    {
        switch (id)
        {
            case TestComponent1::Id:
            {
                EXPECT_EQ(size, sizeof(TestComponent1));

                auto* found = (TestComponent1*) data;
                EXPECT_EQ(found->A, 7);
                EXPECT_EQ(found->B, 8);

                loopCount++;

                break;
            }
            case TestComponent2::Id:
            {
                EXPECT_EQ(size, sizeof(TestComponent1));

                auto* found = (TestComponent2*) data;
                EXPECT_EQ(found->A, 9);
                EXPECT_EQ(found->B, 10);

                loopCount++;

                break;
            }
            case TestComponent3::Id:
            {
                EXPECT_EQ(size, sizeof(TestComponent1));

                auto* found = (TestComponent3*) data;
                EXPECT_EQ(found->A, 11);
                EXPECT_EQ(found->B, 12);

                loopCount++;

                break;
            }
            default:
                FAIL();
                break;
        }
    }

    EXPECT_EQ(loopCount, 3);
}

TEST_F(Archetype_Fixture, AddGetComponentsForEntity_Single)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);

    Archetype archetype(fingerprint, { TestComponent1::Id });

    Otter::EntityId    entityId1          = 1;
    Otter::ComponentId componentIds1[1]   = { TestComponent1::Id };
    TestComponent1     component1(1, 2);
    UInt64             componentSizes1[1] = { sizeof(TestComponent1) };

    EXPECT_TRUE(archetype.TryAddComponentDataUnsafe(entityId1, componentIds1, componentSizes1, (Byte*) &component1));

    Otter::EntityId    entityId2          = 2;
    Otter::ComponentId componentIds2[1]   = { TestComponent1::Id };
    TestComponent1     component2(3, 4);
    UInt64             componentSizes2[1] = { sizeof(TestComponent1) };

    EXPECT_TRUE(archetype.TryAddComponentDataUnsafe(entityId2, componentIds2, componentSizes2, (Byte*) &component2));

    EXPECT_EQ(archetype.GetEntityCount(), 2);
    EXPECT_EQ(archetype.GetComponentCount(), 1);

    EXPECT_DEATH(auto result = archetype.GetComponentsForEntityUnsafe<TestComponent1>(123), "");

    auto* found = archetype.GetComponentsForEntityUnsafe<TestComponent1>(entityId2);

    EXPECT_EQ(found->A, 3);
    EXPECT_EQ(found->B, 4);

    found->A = 5;
    found->B = 6;

    found = archetype.GetComponentsForEntityUnsafe<TestComponent1>(entityId1);

    EXPECT_EQ(found->A, 1);
    EXPECT_EQ(found->B, 2);

    found = archetype.GetComponentsForEntityUnsafe<TestComponent1>(entityId2);

    EXPECT_EQ(found->A, 5);
    EXPECT_EQ(found->B, 6);
}

TEST_F(Archetype_Fixture, AddGetComponentsForEntity_Multiple)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);
    fingerprint.Set(1, true);
    fingerprint.Set(2, true);

    Otter::List<Otter::ComponentId> componentIds = { TestComponent1::Id, TestComponent2::Id, TestComponent3::Id };

    Archetype archetype(fingerprint, componentIds);

    Otter::EntityId    entityId1        = 1;
    Otter::ComponentId componentIds1[3] = { TestComponent1::Id, TestComponent2::Id, TestComponent3::Id };
    TestComponent1     component1a(1, 2);
    TestComponent2     component2a(3, 4);
    TestComponent3     component3a(5, 6);
    Byte               componentData1[sizeof(TestComponent1) + sizeof(TestComponent2) + sizeof(TestComponent3)];
    Otter::MemorySystem::MemoryCopy(&componentData1[0], (Byte*) &component1a, sizeof(TestComponent1));
    Otter::MemorySystem::MemoryCopy(&componentData1[sizeof(TestComponent1)],
                                    (Byte*) &component2a,
                                    sizeof(TestComponent2));
    Otter::MemorySystem::MemoryCopy(&componentData1[sizeof(TestComponent1) + sizeof(TestComponent2)],
                                    (Byte*) &component3a,
                                    sizeof(TestComponent3));
    UInt64 componentSizes1[3] = { sizeof(TestComponent1), sizeof(TestComponent2), sizeof(TestComponent3) };

    EXPECT_TRUE(archetype.TryAddComponentDataUnsafe(entityId1, componentIds1, componentSizes1, componentData1));

    Otter::EntityId    entityId2        = 2;
    Otter::ComponentId componentIds2[3] = { TestComponent1::Id, TestComponent2::Id, TestComponent3::Id };
    TestComponent1     component1b(7, 8);
    TestComponent2     component2b(9, 10);
    TestComponent3     component3b(11, 12);
    Byte               componentData2[sizeof(TestComponent1) + sizeof(TestComponent2) + sizeof(TestComponent3)];
    Otter::MemorySystem::MemoryCopy(&componentData2[0], (Byte*) &component1b, sizeof(TestComponent1));
    Otter::MemorySystem::MemoryCopy(&componentData2[sizeof(TestComponent1)],
                                    (Byte*) &component2b,
                                    sizeof(TestComponent2));
    Otter::MemorySystem::MemoryCopy(&componentData2[sizeof(TestComponent1) + sizeof(TestComponent2)],
                                    (Byte*) &component3b,
                                    sizeof(TestComponent3));
    UInt64 componentSizes2[3] = { sizeof(TestComponent1), sizeof(TestComponent2), sizeof(TestComponent3) };

    EXPECT_TRUE(archetype.TryAddComponentDataUnsafe(entityId2, componentIds2, componentSizes2, componentData2));

    EXPECT_EQ(archetype.GetEntityCount(), 2);
    EXPECT_EQ(archetype.GetComponentCount(), 3);

    auto [comp1, comp2, comp3] = archetype
        .GetComponentsForEntityUnsafe<TestComponent1, TestComponent2, TestComponent3>(entityId2);

    EXPECT_EQ(comp1->A, 7);
    EXPECT_EQ(comp1->B, 8);
    EXPECT_EQ(comp2->A, 9);
    EXPECT_EQ(comp2->B, 10);
    EXPECT_EQ(comp3->A, 11);
    EXPECT_EQ(comp3->B, 12);

    comp1->A = 70;
    comp1->B = 80;
    comp2->A = 90;
    comp2->B = 100;
    comp3->A = 110;
    comp3->B = 120;

    std::tie(comp1, comp2, comp3) = archetype
        .GetComponentsForEntityUnsafe<TestComponent1, TestComponent2, TestComponent3>(entityId1);

    EXPECT_EQ(comp1->A, 1);
    EXPECT_EQ(comp1->B, 2);
    EXPECT_EQ(comp2->A, 3);
    EXPECT_EQ(comp2->B, 4);
    EXPECT_EQ(comp3->A, 5);
    EXPECT_EQ(comp3->B, 6);

    std::tie(comp1, comp2, comp3) = archetype
        .GetComponentsForEntityUnsafe<TestComponent1, TestComponent2, TestComponent3>(entityId2);

    EXPECT_EQ(comp1->A, 70);
    EXPECT_EQ(comp1->B, 80);
    EXPECT_EQ(comp2->A, 90);
    EXPECT_EQ(comp2->B, 100);
    EXPECT_EQ(comp3->A, 110);
    EXPECT_EQ(comp3->B, 120);
}

TEST_F(Archetype_Fixture, Removing_ComponentData)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);

    Archetype archetype(fingerprint, { TestComponent1::Id });

    Otter::EntityId    entityId          = 1;
    Otter::ComponentId componentIds[1]   = { TestComponent1::Id };
    TestComponent1     component(1, 2);
    UInt64             componentSizes[1] = { sizeof(TestComponent1) };

    EXPECT_TRUE(archetype.TryAddComponentDataUnsafe(entityId, componentIds, componentSizes, (Byte*) &component));

    EXPECT_EQ(archetype.GetEntityCount(), 1);
    EXPECT_EQ(archetype.GetComponentCount(), 1);

    auto* found = archetype.GetComponentsForEntityUnsafe<TestComponent1>(entityId);

    EXPECT_EQ(found->A, 1);
    EXPECT_EQ(found->B, 2);

    EXPECT_TRUE(archetype.TryRemoveComponentData(entityId));

    EXPECT_EQ(archetype.GetEntityCount(), 0);
    EXPECT_EQ(archetype.GetComponentCount(), 1);

    EXPECT_DEATH(found = archetype.GetComponentsForEntityUnsafe<TestComponent1>(entityId), "");
}

TEST_F(Archetype_Fixture, HasComponent)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);

    Otter::List<Otter::ComponentId> componentIds = { TestComponent1::Id };

    Archetype archetype(fingerprint, componentIds);

    EXPECT_TRUE(archetype.HasComponent<TestComponent1>());
}

TEST_F(Archetype_Fixture, ForEach_SingleEntity)
{
    ArchetypeFingerprint fingerprint;
    fingerprint.Set(0, true);
    fingerprint.Set(1, true);
    fingerprint.Set(2, true);

    Archetype archetype(fingerprint, { TestComponent1::Id, TestComponent2::Id, TestComponent3::Id });

    Otter::EntityId    entityId        = 1;
    Otter::ComponentId componentIds[3] = { TestComponent1::Id, TestComponent2::Id, TestComponent3::Id };
    TestComponent1     component1(1, 2);
    TestComponent2     component2(3, 4);
    TestComponent3     component3(5, 6);
    Byte               componentData[sizeof(TestComponent1) + sizeof(TestComponent2) + sizeof(TestComponent3)];
    Otter::MemorySystem::MemoryCopy(&componentData[0], (Byte*) &component1, sizeof(TestComponent1));
    Otter::MemorySystem::MemoryCopy(&componentData[sizeof(TestComponent1)],
                                    (Byte*) &component2,
                                    sizeof(TestComponent2));
    Otter::MemorySystem::MemoryCopy(&componentData[sizeof(TestComponent1) + sizeof(TestComponent2)],
                                    (Byte*) &component3,
                                    sizeof(TestComponent3));
    UInt64 componentSizes1[3] = { sizeof(TestComponent1), sizeof(TestComponent2), sizeof(TestComponent3) };

    EXPECT_TRUE(archetype.TryAddComponentDataUnsafe(entityId, componentIds, componentSizes1, componentData));

    EXPECT_EQ(archetype.GetEntityCount(), 1);
    EXPECT_EQ(archetype.GetComponentCount(), 3);

    UInt64 runCount = 0;

    archetype.ForEach<TestComponent1>(
        {
            [&](auto* t1)
            {
                EXPECT_EQ(t1->A, 1);
                EXPECT_EQ(t1->B, 2);

                t1->A = 10;
                t1->B = 20;

                ++runCount;
            }
        });

    EXPECT_EQ(runCount, 1);
    runCount = 0;

    archetype.ForEach<TestComponent1, TestComponent2, TestComponent3>(
        {
            [&](auto* t1, auto* t2, auto* t3)
            {
                EXPECT_EQ(t1->A, 10);
                EXPECT_EQ(t1->B, 20);
                EXPECT_EQ(t2->A, 3);
                EXPECT_EQ(t2->B, 4);
                EXPECT_EQ(t3->A, 5);
                EXPECT_EQ(t3->B, 6);

                ++runCount;
            }
        });

    EXPECT_EQ(runCount, 1);
}