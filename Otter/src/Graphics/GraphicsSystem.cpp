#include "Otter.PCH.h"

#include "Graphics/GraphicsSystem.h"

#include "Graphics/Abstractions/Renderer.h"

namespace Otter::GraphicsSystem
{
    static Graphics::Renderer* gs_Renderer = nullptr;

    bool TryInitialise(const void* const platformContext)
    {
        OTR_INTERNAL_ASSERT_MSG(gs_Renderer == nullptr, "Graphics system already initialised")

        gs_Renderer = Graphics::Renderer::Create();

        List < Graphics::Shader * > shaders;
        shaders.Reserve(2);
        shaders.AddRange({
                             Graphics::Shader::Create("Assets/Shaders/default.glsl")
                         });

        gs_Renderer->Initialise(platformContext, shaders);
        // TODO: Initialise Global Uniform Buffer/Camera

        OTR_LOG_DEBUG("Graphics system initialised...")

        return true;
    }

    void Shutdown()
    {
        OTR_INTERNAL_ASSERT_MSG(gs_Renderer != nullptr, "Graphics system not initialised")

        OTR_LOG_DEBUG("Shutting down graphics system...")
        gs_Renderer->Shutdown();

        Graphics::Renderer::Destroy(gs_Renderer);
    }

    void RenderFrame()
    {
        if (!gs_Renderer->TryBeginFrame())
            return;

        // TODO: Step 1: Bind Pipeline
        // TODO: Step 2a: Update Uniform Buffer
        // TODO: Step 2b: Update Push Constants
        // TODO: Step 3a: Bind Vertex Buffer
        // TODO: Step 3b: Bind Index Buffer
        // TODO: Step 4: Draw Indexed
        gs_Renderer->DrawIndexed();

        gs_Renderer->EndFrame();
    }
}