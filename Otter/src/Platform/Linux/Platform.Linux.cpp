#include "Otter.PCH.h"

#include "Platform/Linux/Platform.Linux.h"

#if OTR_PLATFORM_LINUX

namespace Otter::Internal
{
    struct LinuxPlatformWindowData
    {
    };

    bool LinuxPlatform::Startup(const char* title, UInt16 width, UInt16 height)
    {
        return false;
    }

    void LinuxPlatform::Shutdown()
    {
    }

    void LinuxPlatform::CaptureWindowEvents()
    {
    }

    Double64 LinuxPlatform::GetAbsoluteTime() const
    {
        return 0;
    }
}

#endif
