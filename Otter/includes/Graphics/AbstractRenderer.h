#ifndef OTTERENGINE_ABSTRACTRENDERER_H
#define OTTERENGINE_ABSTRACTRENDERER_H

namespace Otter::Graphics
{
    class AbstractRenderer
    {
    public:
        virtual ~AbstractRenderer() = default;

        virtual void Initialise(const void* platformContext) = 0;
        virtual void Shutdown() = 0;

        virtual void RenderFrame() = 0;
    };
}

#endif //OTTERENGINE_ABSTRACTRENDERER_H
