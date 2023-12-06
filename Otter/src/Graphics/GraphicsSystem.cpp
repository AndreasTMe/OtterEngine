#include "Otter.PCH.h"

#include "Graphics/GraphicsSystem.h"
#include "Assets/Asset.h"

namespace Otter
{
    bool GraphicsSystem::TryInitialise(const void* const platformContext)
    {
        OTR_INTERNAL_ASSERT_MSG(m_Renderer == nullptr, "Graphics system already initialised")

        m_Renderer = Graphics::RendererAPI::Create();
        if (!m_Renderer)
            return false;

        List < Graphics::Shader * > shaders;
        shaders.Reserve(2);
        shaders.TryAddRange({
                                Asset::Create<AssetType::Shader>("Assets/Shaders/default.glsl")
                            });

        List < Graphics::Texture * > textures;
        textures.Reserve(2);
        textures.TryAddRange({
                                 Asset::Create<AssetType::Texture>("Assets/Textures/texture.jpg")
                             });

        m_Renderer->Initialise(platformContext, shaders, textures);
        // TODO: Initialise Global Uniform Buffer/Camera

        OTR_LOG_DEBUG("Graphics system initialised...")

        return true;
    }

    void GraphicsSystem::Shutdown()
    {
        OTR_INTERNAL_ASSERT_MSG(m_Renderer != nullptr, "Graphics system not initialised")

        OTR_LOG_DEBUG("Shutting down graphics system...")
        m_Renderer->Shutdown();

        Graphics::RendererAPI::Destroy(m_Renderer);
    }

    void GraphicsSystem::RenderFrame()
    {
        if (!m_Renderer->TryBeginFrame())
            return;

        // TODO: Step 1: Bind Pipelines
        // TODO: Step 2a: Update Uniform Buffer
        // TODO: Step 2b: Update Push Constants
        // TODO: Step 3a: Bind Vertex Buffer
        // TODO: Step 3b: Bind Index Buffer
        // TODO: Step 4: Draw Indexed
        m_Renderer->DrawIndexed();

        m_Renderer->EndFrame();
    }
}