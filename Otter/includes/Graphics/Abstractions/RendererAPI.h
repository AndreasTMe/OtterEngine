#ifndef OTTERENGINE_RENDERERAPI_H
#define OTTERENGINE_RENDERERAPI_H

#include "Core/Collections/Collection.h"
#include "Graphics/Abstractions/Shader.h"
#include "Graphics/Abstractions/Texture.h"

namespace Otter::Graphics
{
    /**
     * @brief An abstraction for the graphics API used by the engine.
     */
    class RendererAPI
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~RendererAPI() = default;

        /**
         * @brief Initialises the renderer API.
         *
         * @param platformContext The platform-specific context.
         * @param shaders The shaders to load.
         * @param textures The textures to load.
         */
        virtual void Initialise(const void* platformContext,
                                const Collection<Shader*>& shaders,
                                const Collection<Texture*>& textures) = 0;

        /**
         * @brief Shuts down the renderer API.
         */
        virtual void Shutdown() = 0;

        /**
         * @brief Begins a frame.
         *
         * @return True if the frame was successfully begun, false otherwise.
         */
        virtual bool TryBeginFrame() = 0;

        /**
         * @brief Ends a frame.
         */
        virtual void EndFrame() = 0;

        /**
         * @brief Draws a vertex array.
         */
        virtual void DrawIndexed() = 0;

        /**
         * @brief Creates a renderer API.
         *
         * @return The created renderer API.
         *
         * @note This function has to be implemented by each graphics API.
         */
        [[nodiscard]] static RendererAPI* Create();

        /**
         * @brief Destroys a renderer API.
         *
         * @param outRenderer The renderer API to destroy.
         *
         * @note This function has to be implemented by each graphics API.
         */
        static void Destroy(RendererAPI* outRenderer);

    protected:
        /**
         * @brief Constructor.
         */
        RendererAPI() = default;
    };
}

#endif //OTTERENGINE_RENDERERAPI_H
