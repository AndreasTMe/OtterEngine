#ifndef OTTERENGINE_COMPONENT_H
#define OTTERENGINE_COMPONENT_H

#include "Core/BaseTypes.h"

#include "Math/Vector.h"
#include "Math/Quaternion.h"
#include "Math/Geometry/Rectangle.h"

namespace Otter::ECS
{
    /**
     * @brief Marker interface for all components to inherit from.
     *
     * @note IComponent has a protected constructor, deleted copy/move constructors and assignment operators, and no
     * virtual methods.
     * @note Should not be used for any other reason than marking a class as a component.
     */
    class IComponent
    {
    public:
        /**
         * @brief Destructor.
         */
        ~IComponent() = default;

    protected:
        /**
         * @brief Constructor.
         */
        IComponent() = default;
    };

    struct TransformComponent final : public IComponent
    {
    public:
        TransformComponent() = default;
        ~TransformComponent() = default;

        Vector3D<Float32>   Position = Vector3D<Float32>::Zero();
        Quaternion<Float32> Rotation = Quaternion<Float32>::Identity();
        Vector3D<Float32>   Scale    = Vector3D<Float32>::One();
    };

    struct CameraComponent final : public IComponent
    {
    public:
        CameraComponent() = default;
        ~CameraComponent() = default;

        Float32 FieldOfView = 45.0f;
        Float32 NearPlane   = 0.1f;
        Float32 FarPlane    = 1000.0f;
    };

    struct SpriteComponent final : public IComponent
    {
    private:
        template<AnyNumber TNumber>
        using Rectangle = Math::Rectangle<TNumber>;

    public:
        SpriteComponent() = default;
        ~SpriteComponent() = default;

        Vector4D<Float32>  Color  = Vector4D<Float32>::One();
        Rectangle<Float32> Bounds = { Vector2D<Float32>::Zero(), Vector2D<Float32>::One() };
    };
}

#endif //OTTERENGINE_COMPONENT_H
