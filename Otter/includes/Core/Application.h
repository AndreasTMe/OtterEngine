#ifndef OTTERENGINE_APPLICATION_H
#define OTTERENGINE_APPLICATION_H

#include "Core/Defines.h"
#include "Core/Collections/List.h"
#include "Core/Layers.h"
#include "Core/Time.h"

namespace Otter
{
    struct ApplicationConfiguration final
    {
        const char* Title;
        UInt16 Width;
        UInt16 Height;
        UInt64 MemoryRequirements;
    };

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
        explicit Application(const ApplicationConfiguration& config)
            : k_Configuration(config)
        {
        }

        // TODO: Add functionality for adding/removing layers.

    private:
        const ApplicationConfiguration k_Configuration;

        Time* m_Time = nullptr;
        List<Layer*> m_Layers{ };
    };

    [[nodiscard]] Application* CreateApplication();
}

#endif //OTTERENGINE_APPLICATION_H
