#ifndef OTTERENGINE_TYPES_VERTEX_H
#define OTTERENGINE_TYPES_VERTEX_H

#include "Core/Types.h"
#include "Math/Vector.h"

namespace Otter::Graphics
{
    struct Vertex final
    {
    public:
        Vector<3, Float32> Position;
        Vector<4, Float32> Color;
        Vector<2, Float32> TexCoord;
    };
}

#endif //OTTERENGINE_TYPES_VERTEX_H
