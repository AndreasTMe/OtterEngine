#ifndef OTTERENGINE_APPLICATION_H
#define OTTERENGINE_APPLICATION_H

#include "Core/Defines.h"
#include "Core/Collections/List.h"
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
        List<Layer*> m_Layers{ };
    };

    [[nodiscard]] Application* CreateApplication();
}

#endif //OTTERENGINE_APPLICATION_H
