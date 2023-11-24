#ifndef OTTERENGINE_GLOBALACTIONS_H
#define OTTERENGINE_GLOBALACTIONS_H

#include "Core/Defines.h"

#include "Core/Events/WindowEvents.h"
#include "Core/Events/KeyboardEvents.h"
#include "Core/Events/MouseEvents.h"

namespace Otter
{
    class GlobalActions final
    {
    public:
        static Func<bool, const WindowCloseEvent&>     OnWindowClose;
        static Func<bool, const WindowResizeEvent&>    OnWindowResize;
        static Func<bool, const WindowMinimizedEvent&> OnWindowMinimized;
        static Func<bool, const WindowMaximizedEvent&> OnWindowMaximized;
        static Func<bool, const WindowRestoredEvent&>  OnWindowRestored;
        static Func<bool, const WindowRefreshEvent&>   OnWindowRefresh;

        static Func<bool, const KeyPressedEvent&>  OnKeyPressed;
        static Func<bool, const KeyReleasedEvent&> OnKeyReleased;
        static Func<bool, const KeyHoldEvent&>     OnKeyHold;

        static Func<bool, const MouseButtonPressedEvent&>  OnMouseButtonPressed;
        static Func<bool, const MouseButtonReleasedEvent&> OnMouseButtonReleased;
        static Func<bool, const MouseScrollEvent&>         OnMouseScroll;
        static Func<bool, const MouseMovedEvent&>          OnMouseMoved;
        static Func<bool, const MouseDraggedEvent&>        OnMouseDragged;

    private:
        OTR_WITH_DEFAULT_CONSTRUCTOR(GlobalActions)
    };
}

#endif //OTTERENGINE_GLOBALACTIONS_H

