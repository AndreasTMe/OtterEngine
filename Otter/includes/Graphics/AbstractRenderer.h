#ifndef OTTERENGINE_ABSTRACTRENDERER_H
#define OTTERENGINE_ABSTRACTRENDERER_H

namespace Otter::Graphics
{
    class AbstractRenderer
    {
    public:
        virtual ~AbstractRenderer() = default;

        virtual bool TryInitialise(const void* windowHandle) = 0;
        virtual void Shutdown() = 0;
    };
}

#endif //OTTERENGINE_ABSTRACTRENDERER_H
