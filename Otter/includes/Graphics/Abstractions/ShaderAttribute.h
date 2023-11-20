#ifndef OTTERENGINE_SHADERATTRIBUTE_H
#define OTTERENGINE_SHADERATTRIBUTE_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Graphics/Common/Types.ShaderAttributeType.h"
#include "Graphics/Common/Types.ShaderAttributeSize.h"

namespace Otter::Graphics
{
    struct ShaderAttribute final
    {
    public:
        ShaderAttributeType Type;
        ShaderAttributeSize Size;
        UInt16              Offset;

        [[nodiscard]] OTR_INLINE UInt32 GetBitOffset() const { return Offset * static_cast<UInt32>(Size); }
    };
}

#endif //OTTERENGINE_SHADERATTRIBUTE_H
