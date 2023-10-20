#ifndef OTTERENGINE_GLOBALACTIONS_H
#define OTTERENGINE_GLOBALACTIONS_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Events/WindowEvents.h"
#include "Core/Events/KeyboardEvents.h"
#include "Core/Events/MouseEvents.h"

namespace Otter
{
    class GlobalActions final
    {
    public:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

        static Action<const Internal::WindowCloseEvent&>     OnWindowClose;
        static Action<const Internal::WindowResizeEvent&>    OnWindowResize;
        static Action<const Internal::WindowMinimizedEvent&> OnWindowMinimized;
        static Action<const Internal::WindowMaximizedEvent&> OnWindowMaximized;
        static Action<const Internal::WindowRestoredEvent&>  OnWindowRestored;
        static Action<const Internal::WindowRefreshEvent&>   OnWindowRefresh;

        static Action<const Internal::KeyPressedEvent&>  OnKeyPressed;
        static Action<const Internal::KeyReleasedEvent&> OnKeyReleased;
        static Action<const Internal::KeyHoldEvent&>     OnKeyHold;

        static Action<const Internal::MouseButtonPressedEvent&>  OnMouseButtonPressed;
        static Action<const Internal::MouseButtonReleasedEvent&> OnMouseButtonReleased;
        static Action<const Internal::MouseScrollEvent&>         OnMouseScroll;
        static Action<const Internal::MouseMovedEvent&>          OnMouseMoved;
        static Action<const Internal::MouseDraggedEvent&>        OnMouseDragged;

#pragma clang diagnostic pop

    private:
        OTR_WITH_DEFAULT_CONSTRUCTOR(GlobalActions)
    };
}

#endif //OTTERENGINE_GLOBALACTIONS_H

