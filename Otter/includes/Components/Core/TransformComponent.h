#ifndef OTTERENGINE_TRANSFORMCOMPONENT_H
#define OTTERENGINE_TRANSFORMCOMPONENT_H

#include "Components/IComponent.h"
#include "Math/Vector.h"
#include "Math/Quaternion.h"

namespace Otter
{
    /**
     * @brief A component that represents a transform in 3D space.
     */
    struct TransformComponent final : public IComponent
    {
    public:
        /// @brief The Id of the transform component.
        static constexpr ComponentId Id = 1;

        /// @brief The position of the transform.
        Vector3D<Float32>   Position = Vector3D<Float32>::Zero();
        /// @brief The rotation of the transform.
        Quaternion<Float32> Rotation = Quaternion<Float32>::Identity();
        /// @brief The scale of the transform.
        Vector3D<Float32>   Scale    = Vector3D<Float32>::One();

        /**
         * @brief Constructor.
         */
        TransformComponent() = default;

        /**
         * @brief Destructor.
         */
        ~TransformComponent() = default;

        /**
         * @brief Constructor.
         *
         * @param position The position of the transform.
         * @param rotation The rotation of the transform.
         * @param scale The scale of the transform.
         */
        TransformComponent(const Vector3D<Float32>& position,
                           const Quaternion<Float32>& rotation,
                           const Vector3D<Float32>& scale)
            : Position(position), Rotation(rotation), Scale(scale)
        {
        }

        /**
         * @brief Constructor.
         *
         * @param position The position of the transform.
         * @param rotation The rotation of the transform.
         * @param scale The scale of the transform.
         */
        TransformComponent(Vector3D<Float32>&& position,
                           Quaternion<Float32>&& rotation,
                           Vector3D<Float32>&& scale)
            : Position(std::move(position)), Rotation(std::move(rotation)), Scale(std::move(scale))
        {
        }

        /**
         * @brief Copy constructor.
         *
         * @param other Transform to copy from.
         */
        TransformComponent(const TransformComponent& other)
        {
            Position = other.Position;
            Rotation = other.Rotation;
            Scale    = other.Scale;
        }

        /**
         * @brief Move constructor.
         *
         * @param other Transform to move from.
         */
        TransformComponent(TransformComponent&& other) noexcept
        {
            Position = std::move(other.Position);
            Rotation = std::move(other.Rotation);
            Scale    = std::move(other.Scale);
        }

        /**
         * @brief Copy assignment operator.
         *
         * @param other Transform to copy from.
         *
         * @return Reference to this transform.
         */
        TransformComponent& operator=(const TransformComponent& other)
        {
            if (this == &other)
                return *this;

            Position = other.Position;
            Rotation = other.Rotation;
            Scale    = other.Scale;

            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * @param other Transform to move from.
         *
         * @return Reference to this transform.
         */
        TransformComponent& operator=(TransformComponent&& other) noexcept
        {
            if (this == &other)
                return *this;

            Position = std::move(other.Position);
            Rotation = std::move(other.Rotation);
            Scale    = std::move(other.Scale);

            return *this;
        }
    };
}

#endif //OTTERENGINE_TRANSFORMCOMPONENT_H
