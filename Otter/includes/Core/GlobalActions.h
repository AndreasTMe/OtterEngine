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
        OTR_DISABLE_OBJECT_COPIES(GlobalActions)
        OTR_DISABLE_OBJECT_MOVES(GlobalActions)

        OTR_INLINE static GlobalActions& GetInstance()
        {
            static GlobalActions instance;
            return instance;
        }

        Func<bool, const WindowCloseEvent&>     OnWindowClose;
        Func<bool, const WindowResizeEvent&>    OnWindowResize;
        Func<bool, const WindowMinimizedEvent&> OnWindowMinimized;
        Func<bool, const WindowMaximizedEvent&> OnWindowMaximized;
        Func<bool, const WindowRestoredEvent&>  OnWindowRestored;
        Func<bool, const WindowRefreshEvent&>   OnWindowRefresh;

        Func<bool, const KeyPressedEvent&>  OnKeyPressed;
        Func<bool, const KeyReleasedEvent&> OnKeyReleased;
        Func<bool, const KeyRepeatEvent&>   OnKeyRepeat;

        Func<bool, const MouseButtonPressedEvent&>  OnMouseButtonPressed;
        Func<bool, const MouseButtonReleasedEvent&> OnMouseButtonReleased;
        Func<bool, const MouseScrollEvent&>         OnMouseScroll;
        Func<bool, const MouseMovedEvent&>          OnMouseMoved;
        Func<bool, const MouseDraggedEvent&>        OnMouseDragged;

    private:
        OTR_WITH_DEFAULT_CONSTRUCTOR(GlobalActions)
    };
}

#define OTR_GLOBAL_ACTIONS Otter::GlobalActions::GetInstance()

#endif //OTTERENGINE_GLOBALACTIONS_H

