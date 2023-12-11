#include <Otter.h>

namespace Sandbox
{
    class TestLayer final : public Otter::Layer
    {
    public:
        explicit TestLayer(bool isEnabled = true)
            : Otter::Layer(isEnabled)
        {
            OTR_LOG_INFO("TestLayer was created")
        }
        ~TestLayer() override
        {
            OTR_LOG_INFO("TestLayer was destroyed")
        }

        void OnUpdate(const Otter::TimeStep& step) override
        {
        }
    };

    class SandboxApplication final : public Otter::Application
    {
    public:
        explicit SandboxApplication(const Otter::ApplicationConfiguration& config)
            : Otter::Application(config)
        {
            PushLayer<TestLayer>();
        }
    };
}

Otter::Application* Otter::CreateApplication()
{
    return new Sandbox::SandboxApplication({ "Sandbox", 1280, 720, 20_KiB });
}
