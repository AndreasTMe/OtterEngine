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
        explicit Application(const ApplicationConfiguration& config);
        virtual ~Application();

        void Run();

    protected:
        template<typename TLayer, typename... Args>
        requires IsDerivedFrom<TLayer, Layer>
        void PushLayer(Args&& ... args)
        {
            m_Layers.Add(New<TLayer>(std::forward<Args>(args)...));
        }

    private:
        const ApplicationConfiguration k_Configuration;

        Time* m_Time = nullptr;
        List<Layer*> m_Layers{ };
    };

    [[nodiscard]] Application* CreateApplication();
}

#endif //OTTERENGINE_APPLICATION_H
