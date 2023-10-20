#ifndef OTTERENGINE_ENTRYPOINT_H
#define OTTERENGINE_ENTRYPOINT_H

#include "Core/Defines.h"
#include "Core/Application.h"

int OTR_MAIN()
{
    auto app = Otter::CreateApplication();

    app->Run();

    delete app;

    return 0;
}

#endif //OTTERENGINE_ENTRYPOINT_H