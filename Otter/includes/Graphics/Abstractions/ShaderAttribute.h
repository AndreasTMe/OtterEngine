#ifndef OTTERENGINE_SHADERATTRIBUTE_H
#define OTTERENGINE_SHADERATTRIBUTE_H

#include "Core/Defines.h"
#include "Core/BaseTypes.h"
#include "Graphics/Common/Types.ShaderAttributeType.h"
#include "Graphics/Common/Types.ShaderAttributeSize.h"

namespace Otter::Graphics
{
    /**
     * @brief Shader attribute descriptor.
     */
    struct ShaderAttribute final
    {
    public:
        ShaderAttributeType Type;
        ShaderAttributeSize Size;
        UInt16              Offset;

        /**
         * @brief Gets the bit offset of the attribute.
         *
         * @return The bit offset.
         */
        [[nodiscard]] OTR_INLINE UInt32 GetBitOffset() const { return Offset * static_cast<UInt32>(Size); }
    };
}

#endif //OTTERENGINE_SHADERATTRIBUTE_H
