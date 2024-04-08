#ifndef OTTERENGINE_WORLD_H
#define OTTERENGINE_WORLD_H

#include "ECS/EntityManager.h"

namespace Otter
{
    class World final
    {
    public:
        /**
         * @brief Deleted constructor.
         */
        World() = delete;

        /**
         * @brief Deleted destructor.
         */
        ~World() = delete;

        /**
         * @brief Deleted copy constructor.
         */
        World(const World& other) = delete;

        /**
         * @brief Deleted move constructor.
         */
        World(World&& other) noexcept = delete;

        /**
         * @brief Deleted copy assignment operator.
         */
        World& operator=(const World& other) = delete;

        /**
         * @brief Deleted move assignment operator.
         */
        World& operator=(World&& other) noexcept = delete;

        /**
         * @brief Gets the entity manager.
         *
         * @return A reference to the entity manager.
         */
        [[nodiscard]] OTR_INLINE static EntityManager& GetEntityManager() noexcept { return s_EntityManager; }

        /**
         * @brief Initialises the world.
         *
         * @return True if the world was initialised successfully, false otherwise.
         */
        [[nodiscard]] static bool TryInitialise();

        /**
         * @brief Destroys the world.
         */
        static void Destroy();

    private:
        static EntityManager s_EntityManager;
    };

    EntityManager World::s_EntityManager{ };
}

#endif //OTTERENGINE_WORLD_H
