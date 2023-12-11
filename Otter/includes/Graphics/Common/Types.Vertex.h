#ifndef OTTERENGINE_TYPES_VERTEX_H
#define OTTERENGINE_TYPES_VERTEX_H

#include "Math/Vector.h"

namespace Otter::Graphics
{
    struct Vertex final
    {
    public:
        Vector3D<Float32> Position;
        Vector4D<Float32> Color;
        Vector2D<Float32> TexCoord;
    };
}

#endif //OTTERENGINE_TYPES_VERTEX_H
