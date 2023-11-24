#ifndef OTTERENGINE_EVENT_H
#define OTTERENGINE_EVENT_H

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Otter::Internal
{
    class Event
    {
    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR_AND_VIRTUAL_DESTRUCTOR(Event)

        [[nodiscard]] OTR_INLINE constexpr EventType GetEventType() const { return m_EventType; }
        [[nodiscard]] OTR_INLINE constexpr EventCategory GetEventCategory() const { return m_EventCategory; }
        [[nodiscard]] OTR_INLINE constexpr bool IsBlocking() const
        {
            return (m_EventCategory & EventCategory::Blocking) == EventCategory::Blocking;
        }

    protected:
        Event(const EventType& type, const EventCategory& category)
            : m_EventType(type), m_EventCategory(category)
        {
        }

        EventType     m_EventType     = EventType::None;
        EventCategory m_EventCategory = EventCategory::None;
    };

    template<EventType TEventType>
    concept IsKeyPressEvent = TEventType == EventType::KeyPressed;

    template<EventType TEventType>
    concept IsKeyReleaseEvent = TEventType == EventType::KeyReleased;

    template<EventType TEventType>
    concept IsKeyHoldEvent = TEventType == EventType::KeyHold;

    template<EventType TEventType>
    concept IsKeyEvent = IsKeyPressEvent<TEventType> || IsKeyReleaseEvent<TEventType> || IsKeyHoldEvent<TEventType>;

    template<EventType TEventType>
    concept IsMouseButtonPressEvent = TEventType == EventType::MouseButtonPressed;

    template<EventType TEventType>
    concept IsMouseButtonReleaseEvent = TEventType == EventType::MouseButtonReleased;

    template<EventType TEventType>
    concept IsMouseButtonEvent = IsMouseButtonPressEvent<TEventType> || IsMouseButtonReleaseEvent<TEventType>;

    template<EventType TEventType>
    concept IsMouseScrollEvent = TEventType == EventType::MouseScroll;

    template<EventType TEventType>
    concept IsMouseMovedEvent = TEventType == EventType::MouseMoved;

    template<EventType TEventType>
    concept IsMouseDraggedEvent = TEventType == EventType::MouseDragged
                                  || TEventType == EventType::MouseDragStarted
                                  || TEventType == EventType::MouseDragEnded;

    template<EventType TEventType>
    concept IsMouseEvent = IsMouseButtonEvent<TEventType>
                           || IsMouseScrollEvent<TEventType>
                           || IsMouseMovedEvent<TEventType>
                           || IsMouseDraggedEvent<TEventType>;

    template<EventType TEventType>
    concept IsWindowCloseEvent = TEventType == EventType::WindowClose;

    template<EventType TEventType>
    concept IsWindowResizeEvent = TEventType == EventType::WindowResize;

    template<EventType TEventType>
    concept IsWindowMinimizedEvent = TEventType == EventType::WindowMinimized;

    template<EventType TEventType>
    concept IsWindowMaximizedEvent = TEventType == EventType::WindowMaximized;

    template<EventType TEventType>
    concept IsWindowRestoredEvent = TEventType == EventType::WindowRestored;

    template<EventType TEventType>
    concept IsWindowSizeEvent = IsWindowResizeEvent<TEventType>
                                || IsWindowMinimizedEvent<TEventType>
                                || IsWindowMaximizedEvent<TEventType>
                                || IsWindowRestoredEvent<TEventType>;

    template<EventType TEventType>
    concept IsWindowRefreshEvent = TEventType == EventType::WindowRefresh;

    template<EventType TEventType>
    concept IsWindowEvent = IsWindowCloseEvent<TEventType>
                            || IsWindowSizeEvent<TEventType>
                            || IsWindowRefreshEvent<TEventType>;

    template<EventCategory TLeft, EventCategory TRight>
    concept IncludesCategory = (TLeft & TRight) == TRight;

    template<EventCategory TCategory, EventType TType>
    concept IsValidEvent = (IsKeyEvent<TType> && IncludesCategory<TCategory, EventCategory::Keyboard>)
                           || (IsMouseEvent<TType> && IncludesCategory<TCategory, EventCategory::Mouse>)
                           || (IsWindowEvent<TType> && IncludesCategory<TCategory, EventCategory::Window>);

    /*
     * Event Memory Layout:
     *    | Keyboard Event  | Mouse Event           | Window Event
     * ------------------------------------------------------------------------
     * [0]: EventCategory   (Reserved - I32)
     * [1]: -
     * [2]: -
     * [3]: -
     * ------------------------------------------------------------------------
     * [4]: EventType       (Reserved - U8)
     * ------------------------------------------------------------------------
     * [5]: KeyCode (U8)    | MouseButton (U8)      | IsInitiatedByUser (Bool)
     * [6]:                 | Count (U8)            |
     * [7]:                 | IsPositive (Bool)     |
     * ------------------------------------------------------------------------
     * [8]:                 | X (U16)               | Width (U16)
     * [9]:                 | -                     | -
     * [A]:                 | Y (U16)               | Height (U16)
     * [B]:                 | -                     | -
     * ------------------------------------------------------------------------
     * [C]: Counter (F32)   | Counter (F32)         |
     * [D]: -               | -                     |
     * [E]: -               | -                     |
     * [F]: -               | -                     |
     */
    class EventData
    {
    public:
        template<typename TEvent>
        requires IsBaseOf<EventData, TEvent>
        [[nodiscard]] OTR_INLINE constexpr explicit operator TEvent() const
        {
            return *static_cast<const TEvent*>(this);
        }

        template<EventCategory TCategory, EventType TType, typename... TArgs>
        requires IsValidEvent<TCategory, TType>
        [[nodiscard]] OTR_INLINE static EventData Create(TArgs&& ... args);

        [[nodiscard]] OTR_INLINE constexpr EventCategory GetEventCategory() const
        {
            return static_cast<EventCategory>(m_Int32[0]);
        }

        [[nodiscard]] OTR_INLINE constexpr bool IsOfCategory(const EventCategory& category) const
        {
            return (static_cast<EventCategory>(m_Int32[0]) & category) == category;
        }

        [[nodiscard]] OTR_INLINE constexpr bool IsBlocking() const
        {
            return (static_cast<EventCategory>(m_Int32[0]) & EventCategory::Blocking) == EventCategory::Blocking;
        }

        [[nodiscard]] OTR_INLINE constexpr EventType GetEventType() const
        {
            return static_cast<EventType>(m_Byte[4]);
        }

        template<EventType TEventType>
        struct As
        {
            template<EventType T = TEventType, typename = EnableIf<IsKeyEvent < T>>>
            OTR_INLINE static KeyCode GetKeyCode(const EventData& data)
            {
                return static_cast<KeyCode>(data.m_UInt8[5]);
            }

            template<EventType T = TEventType, typename = EnableIf<IsKeyHoldEvent<T>>>
            OTR_INLINE static Float32 GetHoldTime(const EventData& data) noexcept { return data.m_Float32[3]; }

            template<EventType T = TEventType,
                typename = EnableIf<IsMouseButtonEvent<T> || IsMouseDraggedEvent<T>>>
            OTR_INLINE static MouseButton GetMouseButton(const EventData& data)
            {
                return static_cast<MouseButton>(data.m_UInt8[5]);
            }

            template<EventType T = TEventType, typename = EnableIf<IsMouseButtonPressEvent<T>>>
            OTR_INLINE static UInt8 GetCount(const EventData& data) noexcept { return data.m_UInt8[6]; }

            template<EventType T = TEventType, typename = EnableIf<IsMouseScrollEvent<T>>>
            OTR_INLINE static bool IsPositive(const EventData& data) noexcept { return data.m_Bool[7]; }

            template<EventType T = TEventType,
                typename = EnableIf<IsMouseEvent<T> && !IsMouseScrollEvent<T>>>
            OTR_INLINE static UInt16 GetX(const EventData& data) noexcept { return data.m_UInt16[4]; }

            template<EventType T = TEventType,
                typename = EnableIf<IsMouseEvent<T> && !IsMouseScrollEvent<T>>>
            OTR_INLINE static UInt16 GetY(const EventData& data) noexcept { return data.m_UInt16[5]; }

            template<EventType T = TEventType, typename = EnableIf<IsWindowSizeEvent<T>>>
            OTR_INLINE static UInt16 GetWidth(const EventData& data) noexcept { return data.m_UInt16[4]; }

            template<EventType T = TEventType, typename = EnableIf<IsWindowSizeEvent<T>>>
            OTR_INLINE static UInt16 GetHeight(const EventData& data) noexcept { return data.m_UInt16[5]; }
        };

    protected:
        EventData(const EventCategory category, const EventType type)
        {
            m_Int32[0] = static_cast<Int32>(category);
            m_UInt8[4] = static_cast<UInt8>(type);
        }

        EventData(const EventCategory category,
                  const EventType type,
                  const KeyCode keyCode)
            : EventData(category, type)
        {
            OTR_INTERNAL_ASSERT_MSG((category & EventCategory::Keyboard) == EventCategory::Keyboard,
                                    "EventCategory must be a keyboard event.")
            OTR_INTERNAL_ASSERT_MSG(type == EventType::KeyPressed
                                    || type == EventType::KeyReleased
                                    || type == EventType::KeyHold,
                                    "EventType must be a key event.")

            m_UInt8[5] = static_cast<UInt8>(keyCode);
        }

        EventData(const EventCategory category,
                  const EventType type,
                  const KeyCode keyCode,
                  const Float32 time)
            : EventData(category, type, keyCode)
        {
            OTR_INTERNAL_ASSERT_MSG(type == EventType::KeyHold, "EventType must be a key hold event.")

            m_Float32[3] = time;
        }

        EventData(const EventCategory category,
                  const EventType type,
                  const MouseButton button)
            : EventData(category, type)
        {
            OTR_INTERNAL_ASSERT_MSG((category & EventCategory::Mouse) == EventCategory::Mouse,
                                    "EventCategory must be a mouse event.")
            OTR_INTERNAL_ASSERT_MSG(type == EventType::MouseButtonPressed
                                    || type == EventType::MouseButtonReleased,
                                    "EventType must be a mouse button event.")

            m_UInt8[5] = static_cast<UInt8>(button);
        }

        EventData(const EventCategory category,
                  const EventType type,
                  const MouseButton button,
                  const UInt8 count)
            : EventData(category, type, button)
        {
            OTR_INTERNAL_ASSERT_MSG(type == EventType::MouseButtonPressed,
                                    "EventType must be a mouse button pressed event.")

            m_UInt8[6] = count;
        }

        EventData(const EventCategory category,
                  const EventType type,
                  const bool isPositive)
            : EventData(category, type)
        {
            OTR_INTERNAL_ASSERT_MSG((category & EventCategory::Mouse) == EventCategory::Mouse,
                                    "EventCategory must be a mouse event.")
            OTR_INTERNAL_ASSERT_MSG(type == EventType::MouseScroll,
                                    "EventType must be a mouse scroll event.")

            m_Bool[7] = isPositive;
        }

        EventData(const EventCategory category,
                  const EventType type,
                  const UInt16 x,
                  const UInt16 y)
            : EventData(category, type)
        {
            OTR_INTERNAL_ASSERT_MSG((category & EventCategory::Mouse) == EventCategory::Mouse,
                                    "EventCategory must be a mouse event.")
            OTR_INTERNAL_ASSERT_MSG(type == EventType::MouseMoved,
                                    "EventType must be a mouse move event.")

            m_UInt16[4] = x;
            m_UInt16[5] = y;
        }

        EventData(const EventCategory category,
                  const EventType type,
                  const UInt16 width,
                  const UInt16 height,
                  const Float32 dragTime)
            : EventData(category, type)
        {
            OTR_INTERNAL_ASSERT_MSG((category & EventCategory::Mouse) == EventCategory::Mouse,
                                    "EventCategory must be a mouse event.")
            OTR_INTERNAL_ASSERT_MSG(type == EventType::MouseDragged
                                    || type == EventType::MouseDragStarted
                                    || type == EventType::MouseDragEnded,
                                    "EventType must be a mouse drag event.")

            m_UInt16[4]  = width;
            m_UInt16[5]  = height;
            m_Float32[3] = dragTime;
        }

        EventData(const EventCategory category,
                  const EventType type,
                  const UInt16 width,
                  const UInt16 height,
                  const bool isInitiatedByUser)
            : EventData(category, type)
        {
            m_UInt16[4] = width;
            m_UInt16[5] = height;
            m_Bool[5]   = isInitiatedByUser;
        }

        union
        {
            // 8 bytes
            bool      m_Bool[16];
            Byte      m_Byte[16];
            UInt8     m_UInt8[16];
            Int8      m_Int8[16];
            // 16 bytes
            Int16     m_Int16[8];
            UInt16    m_UInt16[8];
            Float16   m_Float16[8];
            // 32 bytes
            Int32     m_Int32[4];
            UInt32    m_UInt32[4];
            Float32   m_Float32[4];
            // 64 bytes
            Int64     m_Int64[2];
            UInt64    m_UInt64[2];
            Double64  m_Double64[2];
            // 128 bytes
            Double128 m_Double128[1] = { 0.0 };
        };
    };

#define OTR_WINDOW_EVENT_CLASS(Type)                                                                            \
    class Window##Type##EventData final : public EventData                                                      \
    {                                                                                                           \
    public:                                                                                                     \
        Window##Type##EventData()                                                                               \
            : EventData((EventCategory) (EventCategory::Window | EventCategory::Blocking),                      \
                        EventType::Window##Type)                                                                \
            {                                                                                                   \
            }                                                                                                   \
    };

    OTR_WINDOW_EVENT_CLASS(Close)

    OTR_WINDOW_EVENT_CLASS(Minimized)

    OTR_WINDOW_EVENT_CLASS(Maximized)

    OTR_WINDOW_EVENT_CLASS(Restored)

    OTR_WINDOW_EVENT_CLASS(Refresh)

#undef OTR_WINDOW_EVENT_CLASS

    template<EventCategory TCategory, EventType TType, typename... TArgs>
    requires IsValidEvent<TCategory, TType>
    EventData EventData::Create(TArgs&& ... args)
    {
        WindowCloseEventData eventData;
        auto                 eventData1 = (WindowMinimizedEventData) eventData;

        return { TCategory, TType, std::forward<TArgs>(args)... };
    }
}
#endif //OTTERENGINE_EVENT_H
