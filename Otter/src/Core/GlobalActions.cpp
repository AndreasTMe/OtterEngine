#include "Otter.PCH.h"

#include "Core/GlobalActions.h"

namespace Otter
{
    Func<bool, const WindowCloseEvent&>     GlobalActions::OnWindowClose;
    Func<bool, const WindowResizeEvent&>    GlobalActions::OnWindowResize;
    Func<bool, const WindowMinimizedEvent&> GlobalActions::OnWindowMinimized;
    Func<bool, const WindowMaximizedEvent&> GlobalActions::OnWindowMaximized;
    Func<bool, const WindowRestoredEvent&>  GlobalActions::OnWindowRestored;
    Func<bool, const WindowRefreshEvent&>   GlobalActions::OnWindowRefresh;

    Func<bool, const KeyPressedEvent&>  GlobalActions::OnKeyPressed;
    Func<bool, const KeyReleasedEvent&> GlobalActions::OnKeyReleased;
    Func<bool, const KeyHoldEvent&>     GlobalActions::OnKeyHold;

    Func<bool, const MouseButtonPressedEvent&>  GlobalActions::OnMouseButtonPressed;
    Func<bool, const MouseButtonReleasedEvent&> GlobalActions::OnMouseButtonReleased;
    Func<bool, const MouseScrollEvent&>         GlobalActions::OnMouseScroll;
    Func<bool, const MouseMovedEvent&>          GlobalActions::OnMouseMoved;
    Func<bool, const MouseDraggedEvent&>        GlobalActions::OnMouseDragged;
}
