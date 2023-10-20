#include "Otter.PCH.h"

#include "Core/GlobalActions.h"

namespace Otter
{
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCInconsistentNamingInspection"

    Action<const Internal::WindowCloseEvent&>     GlobalActions::OnWindowClose;
    Action<const Internal::WindowResizeEvent&>    GlobalActions::OnWindowResize;
    Action<const Internal::WindowMinimizedEvent&> GlobalActions::OnWindowMinimized;
    Action<const Internal::WindowMaximizedEvent&> GlobalActions::OnWindowMaximized;
    Action<const Internal::WindowRestoredEvent&>  GlobalActions::OnWindowRestored;
    Action<const Internal::WindowRefreshEvent&>   GlobalActions::OnWindowRefresh;

    Action<const Internal::KeyPressedEvent&>  GlobalActions::OnKeyPressed;
    Action<const Internal::KeyReleasedEvent&> GlobalActions::OnKeyReleased;
    Action<const Internal::KeyHoldEvent&>     GlobalActions::OnKeyHold;

    Action<const Internal::MouseButtonPressedEvent&>  GlobalActions::OnMouseButtonPressed;
    Action<const Internal::MouseButtonReleasedEvent&> GlobalActions::OnMouseButtonReleased;
    Action<const Internal::MouseScrollEvent&>         GlobalActions::OnMouseScroll;
    Action<const Internal::MouseMovedEvent&>          GlobalActions::OnMouseMoved;
    Action<const Internal::MouseDraggedEvent&>        GlobalActions::OnMouseDragged;

#pragma clang diagnostic pop
}
