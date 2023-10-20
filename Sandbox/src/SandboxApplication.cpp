#include <Otter.h>

namespace Sandbox
{
    class SandboxApplication final : public Otter::Application
    {
    public:
        SandboxApplication() : Otter::Application()
        {
        }
    };
}

Otter::Application* Otter::CreateApplication()
{
    return new Sandbox::SandboxApplication();
}
