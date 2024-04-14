#include "ECS/World.h"

#include "Components/Core/TransformComponent.h"
#include "Components/Core/CameraComponent.h"
#include "Components/Core/SpriteComponent.h"

namespace Otter
{
    EntityManager World::s_EntityManager{ };

    bool World::TryInitialise()
    {
        if (s_EntityManager.IsLocked())
            return false;

        RegisterWorldComponents();

        return true;
    }

    void World::Destroy()
    {
        s_EntityManager.Destroy();
    }

    void World::RegisterWorldComponents()
    {
        OTR_VALIDATE(!s_EntityManager.IsLocked(), "The entity manager is already locked.")

        s_EntityManager
            .RegisterComponents<TransformComponent>()
            .RegisterComponents<CameraComponent>()
            .RegisterComponents<SpriteComponent>()
            .LockComponents();

        OTR_VALIDATE(s_EntityManager.IsLocked(), "The entity manager has not been locked.")
    }
}