#include "Otter.PCH.h"

#include "Platform/Linux/Platform.Linux.h"

#if OTR_PLATFORM_LINUX

namespace Otter::Internal
{
    bool LinuxPlatform::TryInitialise()
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
        return 0.0;
    }
}

#endif
