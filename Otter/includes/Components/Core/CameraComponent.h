#ifndef OTTERENGINE_CAMERACOMPONENT_H
#define OTTERENGINE_CAMERACOMPONENT_H

#include "Components/IComponent.h"
#include "Math/Vector.h"

namespace Otter
{
    /**
     * @brief A component that represents a camera in 3D space.
     */
    struct CameraComponent final : public IComponent
    {
    public:
        /// @brief The background color of the camera.
        Vector4D<Float32> BackgroundColor = Vector4D<Float32>::Zero();
        /// @brief The near plane of the camera.
        Float32           NearPlane       = 0.1f;
        /// @brief The far plane of the camera.
        Float32           FarPlane        = 1000.0f;
        /// @brief The aspect ratio of the camera.
        Float32           AspectRatio     = 16.0f / 9.0f;
        /// @brief The field of view of the camera.
        Float32           FieldOfView     = 45.0f;
        /// @brief Whether or not the camera is orthographic.
        bool              IsOrthographic  = false;

        /**
         * @brief Constructor.
         */
        CameraComponent() = default;

        /**
         * @brief Destructor.
         */
        ~CameraComponent() = default;

        /**
         * @brief Constructor.
         *
         * @param backgroundColor The background color of the camera.
         * @param nearPlane The near plane of the camera.
         * @param farPlane The far plane of the camera.
         * @param aspectRatio The aspect ratio of the camera.
         * @param fieldOfView The field of view of the camera.
         * @param isOrthographic Whether or not the camera is orthographic.
         */
        CameraComponent(const Vector4D<Float32>& backgroundColor,
                        Float32 nearPlane,
                        Float32 farPlane,
                        Float32 aspectRatio,
                        Float32 fieldOfView,
                        bool isOrthographic)
            : BackgroundColor(backgroundColor),
              NearPlane(nearPlane),
              FarPlane(farPlane),
              AspectRatio(aspectRatio),
              FieldOfView(fieldOfView),
              IsOrthographic(isOrthographic)
        {
        }

        /**
         * @brief Copy constructor.
         *
         * @param other Camera to copy from.
         */
        CameraComponent(const CameraComponent& other)
        {
            BackgroundColor = other.BackgroundColor;
            NearPlane       = other.NearPlane;
            FarPlane        = other.FarPlane;
            AspectRatio     = other.AspectRatio;
            FieldOfView     = other.FieldOfView;
            IsOrthographic  = other.IsOrthographic;
        }

        /**
         * @brief Move constructor.
         *
         * @param other Camera to move from.
         */
        CameraComponent(CameraComponent&& other) noexcept
        {
            BackgroundColor = std::move(other.BackgroundColor);
            NearPlane       = other.NearPlane;
            FarPlane        = other.FarPlane;
            AspectRatio     = other.AspectRatio;
            FieldOfView     = other.FieldOfView;
            IsOrthographic  = other.IsOrthographic;
        }

        /**
         * @brief Copy assignment operator.
         *
         * @param other Camera to copy from.
         *
         * @return Reference to this camera.
         */
        CameraComponent& operator=(const CameraComponent& other)
        {
            if (this == &other)
                return *this;

            BackgroundColor = other.BackgroundColor;
            NearPlane       = other.NearPlane;
            FarPlane        = other.FarPlane;
            AspectRatio     = other.AspectRatio;
            FieldOfView     = other.FieldOfView;
            IsOrthographic  = other.IsOrthographic;

            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * @param other Camera to move from.
         *
         * @return Reference to this camera.
         */
        CameraComponent& operator=(CameraComponent&& other) noexcept
        {
            if (this == &other)
                return *this;

            BackgroundColor = std::move(other.BackgroundColor);
            NearPlane       = other.NearPlane;
            FarPlane        = other.FarPlane;
            AspectRatio     = other.AspectRatio;
            FieldOfView     = other.FieldOfView;
            IsOrthographic  = other.IsOrthographic;

            return *this;
        }
    };
}

#endif //OTTERENGINE_CAMERACOMPONENT_H
