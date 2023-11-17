#ifndef OTTERENGINE_VULKANTYPES_UNIFORMBUFFERS_H
#define OTTERENGINE_VULKANTYPES_UNIFORMBUFFERS_H

#include "Math/Matrix.h"

namespace Otter::Graphics::Vulkan
{
    struct GlobalUniformBufferObject
    {
        alignas(16) Matrix<4, 4, Float32> Projection;
        alignas(16) Matrix<4, 4, Float32> View;

        // HELP: Max 256 bytes, reserve this for later
        alignas(16) Matrix<4, 4, Float32> Reserve0;
        alignas(16) Matrix<4, 4, Float32> Reserve1;
    };
}

#endif //OTTERENGINE_VULKANTYPES_UNIFORMBUFFERS_H
