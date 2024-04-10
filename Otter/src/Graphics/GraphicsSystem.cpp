#include "Graphics/GraphicsSystem.h"

#include "Core/Collections/List.h"
#include "Assets/Asset.h"

namespace Otter
{
    Graphics::RendererAPI* GraphicsSystem::s_Renderer = nullptr;

    bool GraphicsSystem::TryInitialise(const void* const platformContext)
    {
        OTR_INTERNAL_ASSERT_MSG(s_Renderer == nullptr, "Graphics system already initialised")

        s_Renderer = Graphics::RendererAPI::Create();
        if (!s_Renderer)
            return false;

        List<Graphics::Shader*> shaders;
        shaders.Reserve(2);
        shaders.TryAddRange({
                                Asset::Create<AssetType::Shader>("Assets/Shaders/default.glsl")
                            });

        List<Graphics::Texture*> textures;
        textures.Reserve(2);
        textures.TryAddRange({
                                 Asset::Create<AssetType::Texture>("Assets/Textures/texture.jpg")
                             });

        s_Renderer->Initialise(platformContext, shaders, textures);

        OTR_LOG_DEBUG("Graphics system initialised...")

        return true;
    }

    void GraphicsSystem::Shutdown()
    {
        OTR_INTERNAL_ASSERT_MSG(s_Renderer != nullptr, "Graphics system not initialised")

        OTR_LOG_DEBUG("Shutting down graphics system...")
        s_Renderer->Shutdown();

        Graphics::RendererAPI::Destroy(s_Renderer);
    }

    void GraphicsSystem::RenderFrame()
    {
        if (!s_Renderer->TryBeginFrame())
            return;

        // TODO: Step 1: Bind Pipelines
        // TODO: Step 2a: Update Uniform Buffer
        // TODO: Step 2b: Update Push Constants
        // TODO: Step 3a: Bind Vertex Buffer
        // TODO: Step 3b: Bind Index Buffer
        // TODO: Step 4: Draw Indexed
        s_Renderer->DrawIndexed();

        s_Renderer->EndFrame();
    }
}