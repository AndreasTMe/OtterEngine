#ifndef OTTERENGINE_VERTEX_H
#define OTTERENGINE_VERTEX_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Collections/Collection.h"
#include "Math/Vector.h"
#include "Graphics/2D/Sprite.h"

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

#endif //OTTERENGINE_VERTEX_H
