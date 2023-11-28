#include <Otter.h>

namespace Sandbox
{
    class SandboxApplication final : public Otter::Application
    {
    public:
        explicit SandboxApplication(const Otter::ApplicationConfiguration& config)
            : Otter::Application(config)
        {
        }
    };
}

Otter::Application* Otter::CreateApplication()
{
    return new Sandbox::SandboxApplication({ "Sandbox", 1280, 720, 10_KiB });
}
