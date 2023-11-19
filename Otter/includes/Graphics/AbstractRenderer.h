#ifndef OTTERENGINE_ABSTRACTRENDERER_H
#define OTTERENGINE_ABSTRACTRENDERER_H

#include "Core/Defines.h"
#include "Core/Collections/Collection.h"
#include "Graphics/AbstractShader.h"

namespace Otter::Graphics
{
    class AbstractRenderer
    {
    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR_AND_VIRTUAL_DESTRUCTOR(AbstractRenderer)

        virtual void Initialise(const void* platformContext, const Collection<AbstractShader*>& shaders) = 0;
        virtual void Shutdown() = 0;

        virtual bool TryBeginFrame() = 0;
        virtual void DrawFrame() = 0;
        virtual void EndFrame() = 0;
    };
}

#endif //OTTERENGINE_ABSTRACTRENDERER_H
