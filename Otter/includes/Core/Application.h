#ifndef OTTERENGINE_APPLICATION_H
#define OTTERENGINE_APPLICATION_H

#include "Core/Defines.h"
#include "Core/Layers.h"

namespace Otter
{
    class Application
    {
    public:
        virtual ~Application() = default;

        void Run();

    protected:
        Application() = default;

        template<typename... TLayers>
        void CaptureLayersInOrder(Layer* first, TLayers&& ... layers)
        {
            static bool hasCapturedLayers = false;
            OTR_INTERNAL_ASSERT_MSG(!hasCapturedLayers, "Layers have already been captured!")

            // Capture layers

            hasCapturedLayers = true;
        }
    };

    Application* CreateApplication();
}

#endif //OTTERENGINE_APPLICATION_H
