#ifndef OTTERENGINE_GRAPHICSSYSTEM_H
#define OTTERENGINE_GRAPHICSSYSTEM_H

#include "Graphics/Abstractions/RendererAPI.h"

namespace Otter
{
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

        static bool TryInitialise(const void* platformContext);
        static void Shutdown();

        static void RenderFrame();

    private:
        GraphicsSystem() = default;
        ~GraphicsSystem() = default;

        static Graphics::RendererAPI* s_Renderer;
    };
}

#endif //OTTERENGINE_GRAPHICSSYSTEM_H
