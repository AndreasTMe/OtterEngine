#ifndef OTTERENGINE_CAMERACOMPONENT_H
#define OTTERENGINE_CAMERACOMPONENT_H

#include "Components/IComponent.h"
#include "Math/Vector.h"

namespace Otter
{
    /**
     * @brief A structure that represents the clipping planes of a camera.
     */
    struct ClippingPlanes
    {
    public:
        /// @brief The near plane of the camera.
        Float32 Near;
        /// @brief The far plane of the camera.
        Float32 Far;
    };

    /**
     * @brief A structure that represents the viewport rectangle of a camera.
     */
    struct ViewportRect
    {
    public:
        /// @brief The x position of the viewport.
        Float32 X;
        /// @brief The y position of the viewport.
        Float32 Y;
        /// @brief The width of the viewport.
        Float32 Width;
        /// @brief The height of the viewport.
        Float32 Height;
    };

    /**
     * @brief A component that represents a camera in 3D space.
     */
    struct CameraComponent final : public IComponent
    {
    public:
        /// @brief The Id of the camera component.
        static constexpr ComponentId Id = 2;

        /// @brief The background color of the camera.
        Vector4D<Float32> BackgroundColor = Vector4D<Float32>::Zero();
        /// @brief The clipping planes of the camera.
        ClippingPlanes    Planes          = { 0.1f, 1000.0f };
        /// @brief The viewport rectangle of the camera.
        ViewportRect      Viewport        = { 0, 0, 1, 1 };
        /// @brief The aspect ratio of the camera.
        Float32           AspectRatio     = 16.0f / 9.0f;
        /// @brief The field of view of the camera.
        Float32           FieldOfView     = 45.0f;
        /// @brief The priority of the camera in the draw order.
        Byte              Priority        = 0;
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
         * @param planes The clipping planes of the camera.
         * @param viewport The viewport rectangle of the camera.
         * @param aspectRatio The aspect ratio of the camera.
         * @param fieldOfView The field of view of the camera.
         * @param priority The priority of the camera in the draw order.
         * @param isOrthographic Whether or not the camera is orthographic.
         */
        CameraComponent(const Vector4D<Float32>& backgroundColor,
                        const ClippingPlanes& planes,
                        const ViewportRect& viewport,
                        Float32 aspectRatio,
                        Float32 fieldOfView,
                        Byte priority,
                        bool isOrthographic)
            : BackgroundColor(backgroundColor),
              Planes(planes),
              Viewport(viewport),
              AspectRatio(aspectRatio),
              FieldOfView(fieldOfView),
              Priority(priority),
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
            Planes          = other.Planes;
            Viewport        = other.Viewport;
            AspectRatio     = other.AspectRatio;
            FieldOfView     = other.FieldOfView;
            Priority        = other.Priority;
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
            Planes          = other.Planes;
            Viewport        = other.Viewport;
            AspectRatio     = other.AspectRatio;
            FieldOfView     = other.FieldOfView;
            Priority        = other.Priority;
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
            Planes          = other.Planes;
            Viewport        = other.Viewport;
            AspectRatio     = other.AspectRatio;
            FieldOfView     = other.FieldOfView;
            Priority        = other.Priority;
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
            Planes          = other.Planes;
            Viewport        = other.Viewport;
            AspectRatio     = other.AspectRatio;
            FieldOfView     = other.FieldOfView;
            Priority        = other.Priority;
            IsOrthographic  = other.IsOrthographic;

            return *this;
        }
    };
}

#endif //OTTERENGINE_CAMERACOMPONENT_H
