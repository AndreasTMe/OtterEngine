#ifndef OTTERENGINE_GRAPHICSSYSTEM_H
#define OTTERENGINE_GRAPHICSSYSTEM_H

#include "Graphics/Abstractions/RendererAPI.h"

namespace Otter
{
    /**
     * @brief The application's graphics system.
     */
    class GraphicsSystem final
    {
    public:
        /**
         * @brief Deleted copy constructor.
         */
        GraphicsSystem(GraphicsSystem&) = delete;

        /**
         * @brief Deleted copy constructor.
         */
        GraphicsSystem(const GraphicsSystem&) = delete;

        /**
         * @brief Deleted copy assignment operator.
         */
        GraphicsSystem& operator=(const GraphicsSystem&) = delete;

        /**
         * @brief Deleted move constructor.
         */
        GraphicsSystem(GraphicsSystem&&) = delete;

        /**
         * @brief Deleted move assignment operator.
         */
        GraphicsSystem& operator=(GraphicsSystem&&) = delete;

        /**
         * @brief Initialises the graphics system.
         *
         * @param platformContext The platform-specific context.
         *
         * @return True if the graphics system was initialised successfully, false otherwise.
         */
        static bool TryInitialise(const void* platformContext);

        /**
         * @brief Shuts down the graphics system.
         */
        static void Shutdown();

        /**
         * @brief Renders a frame.
         */
        static void RenderFrame();

    private:
        /**
         * @brief Constructor.
         */
        GraphicsSystem() = default;

        /**
         * @brief Destructor.
         */
        ~GraphicsSystem() = default;

        static Graphics::RendererAPI* s_Renderer;
    };
}

#endif //OTTERENGINE_GRAPHICSSYSTEM_H
