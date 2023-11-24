#ifndef OTTERENGINE_EVENT_H
#define OTTERENGINE_EVENT_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Memory.h"

namespace Otter
{
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
    class Event
    {
    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR(Event)

        Event(const Event& other)
        {
            for (UInt64 i = 0; i < 16; i++)
                m_Data[i] = other.m_Data[i];
        }

        Event(Event&& other) noexcept
        {
            for (UInt64 i = 0; i < 16; i++)
                m_Data[i] = other.m_Data[i];
        }

        Event& operator=(const Event& other)
        {
            if (this != &other)
            {
                for (UInt64 i = 0; i < 16; i++)
                    m_Data[i] = other.m_Data[i];
            }

            return *this;
        }

        Event& operator=(Event&& other) noexcept
        {
            if (this != &other)
            {
                for (UInt64 i = 0; i < 16; i++)
                    m_Data[i] = other.m_Data[i];
            }

            return *this;
        }

        template<typename TEvent>
        requires IsBaseOf<Event, TEvent>
        [[nodiscard]] OTR_INLINE constexpr explicit operator TEvent() const
        {
            return *static_cast<const TEvent*>(this);
        }

        [[nodiscard]] OTR_INLINE constexpr EventCategory GetEventCategory() const { return Get<EventCategory>(0); }
        [[nodiscard]] OTR_INLINE constexpr EventType GetEventType() const { return Get<EventType>(4); }

        [[nodiscard]] OTR_INLINE constexpr bool IsOfCategory(const EventCategory& category) const
        {
            return (GetEventCategory() & category) == category;
        }

        [[nodiscard]] OTR_INLINE constexpr bool IsBlocking() const
        {
            return (GetEventCategory() & EventCategory::Blocking) == EventCategory::Blocking;
        }

    protected:
        Event(const EventCategory category, const EventType type)
            : Event()
        {
            Capture(category, 0);
            Capture(type, 4);
        }

        template<typename T>
        constexpr void Capture(const T& value, const UInt64 offset)
        {
            if constexpr (IsSame<T, bool> || IsSame<T, UInt8> || IsSame<T, Int8>)
            {
                m_Data[offset] = static_cast<Byte>(value);
            }
            else if constexpr (IsSame<T, UInt16> || IsSame<T, Int16>
                               || IsSame<T, UInt32> || IsSame<T, Int32>
                               || IsSame<T, UInt64> || IsSame<T, Int64>)
            {
                for (Size i = 0; i < sizeof(T); i++)
                    m_Data[i + offset] = ((value >> (i * 8)) & 0xFF);
            }
            else
            {
                MemorySystem::MemoryCopy(m_Data + offset, &value, sizeof(T));
            }
        }

        template<typename T>
        [[nodiscard]] constexpr T Get(const UInt64 offset) const
        {
            T value;

            if constexpr (IsSame<T, bool> || IsSame<T, UInt8> || IsSame<T, Int8>)
            {
                value = static_cast<T>(m_Data[offset]);
            }
            else if constexpr (IsSame<T, UInt16> || IsSame<T, Int16>
                               || IsSame<T, UInt32> || IsSame<T, Int32>
                               || IsSame<T, UInt64> || IsSame<T, Int64>)
            {
                value = 0;
                for (Size i = 0; i < sizeof(T); i++)
                    value |= (static_cast<T>(m_Data[i + offset]) << (i * 8));
            }
            else
            {
                MemorySystem::MemoryCopy(&value, m_Data + offset, sizeof(T));
            }

            return value;
        }

    private:
        Byte m_Data[16] = { 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0 };
    };
}
#endif //OTTERENGINE_EVENT_H
