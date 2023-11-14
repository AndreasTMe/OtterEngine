#ifndef OTTERENGINE_ABSTRACTRENDERER_H
#define OTTERENGINE_ABSTRACTRENDERER_H

#include "Core/Defines.h"

namespace Otter::Graphics
{
    class AbstractRenderer
    {
    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR_AND_VIRTUAL_DESTRUCTOR(AbstractRenderer)

        virtual void Initialise(const void* platformContext) = 0;
        virtual void Shutdown() = 0;

        virtual void RenderFrame() = 0;
    };
}

#endif //OTTERENGINE_ABSTRACTRENDERER_H
