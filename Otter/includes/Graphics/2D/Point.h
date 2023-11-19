#ifndef OTTERENGINE_POINT_H
#define OTTERENGINE_POINT_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Collections/Collection.h"
#include "Math/Vector.h"
#include "Graphics/2D/Sprite.h"

namespace Otter::Graphics
{
    struct Point final
    {
    public:
        Vector<3, Float32> Position;
        Vector<4, Float32> Color;

        OTR_INLINE static void GetVertices(const Sprite& sprite, List<Point>& vertices)
        {
            const auto spriteVertices = sprite.GetVertices();
            const auto spriteCenter   = sprite.GetCenter();
            vertices.Reserve(spriteVertices.Length());

            for (UInt64 i = 0; i < spriteVertices.Length(); i++)
            {
                vertices.Add({
                                 { spriteVertices[i][0] - spriteCenter.GetX(),
                                   spriteVertices[i][1] - spriteCenter.GetY(),
                                   0.0f },
                                 sprite.GetColor()
                             });
            }
        }

        OTR_INLINE static void GetTriangles(const Sprite& sprite, List<UInt16>& triangles)
        {
            // TODO: This is a temporary solution, only for square sprites
            Collections::New<UInt16>({ 0, 1, 2, 1, 2, 3 }, triangles);
        }
    };
}

#endif //OTTERENGINE_POINT_H
