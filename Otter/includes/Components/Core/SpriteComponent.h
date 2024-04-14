#ifndef OTTERENGINE_SPRITECOMPONENT_H
#define OTTERENGINE_SPRITECOMPONENT_H

#include "Components/IComponent.h"
#include "Math/Vector.h"

namespace Otter
{
    /**
     * @brief A component that represents a sprite.
     */
    struct SpriteComponent final : public IComponent
    {
    private:
    public:
        /// @brief The Id of the sprite component.
        static constexpr ComponentId Id = 3;

        /// @brief The color of the sprite.
        Vector4D<Float32> Color = Vector4D<Float32>::One();

        /**
         * @brief Constructor.
         */
        SpriteComponent() = default;

        /**
         * @brief Destructor.
         */
        ~SpriteComponent() = default;

        /**
         * @brief Constructor.
         *
         * @param color The color of the sprite.
         */
        SpriteComponent(const Vector4D<Float32>& color)
            : Color(color)
        {
        }

        /**
         * @brief Copy constructor.
         *
         * @param other Sprite to copy from.
         */
        SpriteComponent(const SpriteComponent& other)
        {
            Color = other.Color;
        }

        /**
         * @brief Move constructor.
         *
         * @param other Sprite to move from.
         */
        SpriteComponent(SpriteComponent&& other) noexcept
        {
            Color = std::move(other.Color);
        }

        /**
         * @brief Copy assignment operator.
         *
         * @param other Sprite to copy from.
         *
         * @return Reference to this sprite.
         */
        SpriteComponent& operator=(const SpriteComponent& other)
        {
            if (this == &other)
                return *this;

            Color = other.Color;

            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * @param other Sprite to move from.
         *
         * @return Reference to this sprite.
         */
        SpriteComponent& operator=(SpriteComponent&& other) noexcept
        {
            if (this == &other)
                return *this;

            Color = std::move(other.Color);

            return *this;
        }
    };
}

#endif //OTTERENGINE_SPRITECOMPONENT_H
