#ifndef OTTERENGINE_OTTER_PCH_H
#define OTTERENGINE_OTTER_PCH_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Logger.h"
#include "Core/Memory.h"
#include "Core/Events.h"
#include "Core/GlobalActions.h"
#include "Core/Collections.h"

#include "Platform/Platform.h"

#if OTR_PLATFORM_WINDOWS
    #include <Windows.h>
#endif

#endif //OTTERENGINE_OTTER_PCH_H