#ifndef OTTERENGINE_APPLICATION_H
#define OTTERENGINE_APPLICATION_H

#include <vector>

#include "Core/Defines.h"
#include "Core/Layers.h"

namespace Otter
{
    class Application
    {
    public:
        virtual ~Application()
        {
            for (auto* layer: m_Layers)
                Layer::Delete(layer);
        }

        void Run();

    protected:
        Application() = default;

        // TODO: Add functionality for adding/removing layers.

    private:
        // TODO: Replace with a custom list.
        std::vector<Layer*> m_Layers;
    };

    Application* CreateApplication();
}

#endif //OTTERENGINE_APPLICATION_H
