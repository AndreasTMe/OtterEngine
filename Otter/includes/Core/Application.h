#ifndef OTTERENGINE_APPLICATION_H
#define OTTERENGINE_APPLICATION_H

#include "Core/Defines.h"
#include "Core/Collections/List.h"
#include "Core/Layers.h"
#include "Core/Time.h"

namespace Otter
{
    /**
     * @brief Configuration structure, used to pass information to the application, like window information,
     * memory requirements, etc.
     */
    struct ApplicationConfiguration final
    {
        const char* Title;
        UInt16 Width;
        UInt16 Height;
        UInt64 MemoryRequirements;
    };

    /**
     * @brief Application class.
     *
     * @note This class is the main class of the engine. It is responsible for the initialization of
     * the engine and the main loop.
     */
    class Application
    {
    public:
        /**
         * @brief Constructor.
         *
         * @param config The application configuration.
         */
        explicit Application(const ApplicationConfiguration& config);

        /**
         * @brief Destructor.
         */
        virtual ~Application();

        /**
         * @brief Runs the application.
         *
         * @note This function is responsible for the main loop of the engine.
         */
        void Run();

    protected:
        /**
         * @brief Used from child classes to push a layer to the application.
         *
         * @tparam TLayer An application layer.
         * @tparam Args The types of arguments to pass to the layer constructor.
         *
         * @param args The arguments to pass to the layer constructor.
         */
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

    /**
     * @brief Creates an application.
     *
     * @return A pointer to the application.
     *
     * @note This function is implemented by the client.
     */
    [[nodiscard]] Application* CreateApplication();
}

#endif //OTTERENGINE_APPLICATION_H
