#include "Otter.PCH.h"

#include "Core/GlobalActions.h"

namespace Otter
{
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

    Func<bool, const Internal::WindowCloseEvent&>     GlobalActions::OnWindowClose;
    Func<bool, const Internal::WindowResizeEvent&>    GlobalActions::OnWindowResize;
    Func<bool, const Internal::WindowMinimizedEvent&> GlobalActions::OnWindowMinimized;
    Func<bool, const Internal::WindowMaximizedEvent&> GlobalActions::OnWindowMaximized;
    Func<bool, const Internal::WindowRestoredEvent&>  GlobalActions::OnWindowRestored;
    Func<bool, const Internal::WindowRefreshEvent&>   GlobalActions::OnWindowRefresh;

    Func<bool, const Internal::KeyPressedEvent&>  GlobalActions::OnKeyPressed;
    Func<bool, const Internal::KeyReleasedEvent&> GlobalActions::OnKeyReleased;
    Func<bool, const Internal::KeyHoldEvent&>     GlobalActions::OnKeyHold;

    Func<bool, const Internal::MouseButtonPressedEvent&>  GlobalActions::OnMouseButtonPressed;
    Func<bool, const Internal::MouseButtonReleasedEvent&> GlobalActions::OnMouseButtonReleased;
    Func<bool, const Internal::MouseScrollEvent&>         GlobalActions::OnMouseScroll;
    Func<bool, const Internal::MouseMovedEvent&>          GlobalActions::OnMouseMoved;
    Func<bool, const Internal::MouseDraggedEvent&>        GlobalActions::OnMouseDragged;

#pragma clang diagnostic pop
}
