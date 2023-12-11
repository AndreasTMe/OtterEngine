#ifndef OTTERENGINE_EVENT_H
#define OTTERENGINE_EVENT_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Memory.h"

namespace Otter
{
    /**
     * @brief Concept for checking if an event type is KeyPressed.
     *
     * @tparam TEventType The event type to check.
     */
    template<EventType TEventType>
    concept IsKeyPressEvent = TEventType == EventType::KeyPressed;

    /**
     * @brief Concept for checking if an event type is KeyReleased.
     *
     * @tparam TEventType The event type to check.
     */
    template<EventType TEventType>
    concept IsKeyReleaseEvent = TEventType == EventType::KeyReleased;

    /**
     * @brief Concept for checking if an event type is KeyRepeat.
     *
     * @tparam TEventType The event type to check.
     */
    template<EventType TEventType>
    concept IsKeyRepeatEvent = TEventType == EventType::KeyRepeat;

    /**
     * @brief Concept for checking if an event type is a Key event.
     *
     * @tparam TEventType The event type to check.
     */
    template<EventType TEventType>
    concept IsKeyEvent = IsKeyPressEvent<TEventType> || IsKeyReleaseEvent<TEventType> || IsKeyRepeatEvent<TEventType>;

    /**
     * @brief Concept for checking if an event type is MouseButtonPressed.
     *
     * @tparam TEventType The event type to check.
     */
    template<EventType TEventType>
    concept IsMouseButtonPressEvent = TEventType == EventType::MouseButtonPressed;

    /**
     * @brief Concept for checking if an event type is MouseButtonReleased.
     *
     * @tparam TEventType The event type to check.
     */
    template<EventType TEventType>
    concept IsMouseButtonReleaseEvent = TEventType == EventType::MouseButtonReleased;

    /**
     * @brief Concept for checking if an event type is a MouseButton event.
     *
     * @tparam TEventType The event type to check.
     */
    template<EventType TEventType>
    concept IsMouseButtonEvent = IsMouseButtonPressEvent<TEventType> || IsMouseButtonReleaseEvent<TEventType>;

    /**
     * @brief Concept for checking if an event type is MouseScroll.
     *
     * @tparam TEventType The event type to check.
     */
    template<EventType TEventType>
    concept IsMouseScrollEvent = TEventType == EventType::MouseScroll;

    /**
     * @brief Concept for checking if an event type is MouseMoved.
     *
     * @tparam TEventType The event type to check.
     */
    template<EventType TEventType>
    concept IsMouseMovedEvent = TEventType == EventType::MouseMoved;

    /**
     * @brief Concept for checking if an event type is a Mouse event.
     *
     * @tparam TEventType The event type to check.
     */
    template<EventType TEventType>
    concept IsMouseEvent = IsMouseButtonEvent<TEventType>
                           || IsMouseScrollEvent<TEventType>
                           || IsMouseMovedEvent<TEventType>;

    /**
     * @brief Concept for checking if an event type is WindowClose.
     *
     * @tparam TEventType The event type to check.
     */
    template<EventType TEventType>
    concept IsWindowCloseEvent = TEventType == EventType::WindowClose;

    /**
     * @brief Concept for checking if an event type is WindowResize.
     *
     * @tparam TEventType The event type to check.
     */
    template<EventType TEventType>
    concept IsWindowResizeEvent = TEventType == EventType::WindowResize;

    /**
     * @brief Concept for checking if an event type is WindowMinimized.
     *
     * @tparam TEventType The event type to check.
     */
    template<EventType TEventType>
    concept IsWindowMinimizedEvent = TEventType == EventType::WindowMinimized;

    /**
     * @brief Concept for checking if an event type is WindowMaximized.
     *
     * @tparam TEventType The event type to check.
     */
    template<EventType TEventType>
    concept IsWindowMaximizedEvent = TEventType == EventType::WindowMaximized;

    /**
     * @brief Concept for checking if an event type is WindowRestored.
     *
     * @tparam TEventType The event type to check.
     */
    template<EventType TEventType>
    concept IsWindowRestoredEvent = TEventType == EventType::WindowRestored;

    /**
     * @brief Concept for checking if an event type is a Window size event.
     *
     * @tparam TEventType The event type to check.
     */
    template<EventType TEventType>
    concept IsWindowSizeEvent = IsWindowResizeEvent<TEventType>
                                || IsWindowMinimizedEvent<TEventType>
                                || IsWindowMaximizedEvent<TEventType>
                                || IsWindowRestoredEvent<TEventType>;

    /**
     * @brief Concept for checking if an event type is a Window event.
     *
     * @tparam TEventType The event type to check.
     */
    template<EventType TEventType>
    concept IsWindowEvent = IsWindowCloseEvent<TEventType>
                            || IsWindowSizeEvent<TEventType>;

    /**
     * @brief Concept for checking if an event category is included in another.
     *
     * @tparam TLeft The event category with all the flags.
     * @tparam TRight The event category to check.
     */
    template<EventCategory TLeft, EventCategory TRight>
    concept IncludesCategory = (TLeft & TRight) == TRight;

    /**
     * @brief Concept for checking if an event is valid.
     *
     * @tparam TCategory The event category to check.
     * @tparam TType The event type to check.
     */
    template<EventCategory TCategory, EventType TType>
    concept IsValidEvent = (IsKeyEvent<TType> && IncludesCategory<TCategory, EventCategory::Keyboard>)
                           || (IsMouseEvent<TType> && IncludesCategory<TCategory, EventCategory::Mouse>)
                           || (IsWindowEvent<TType> && IncludesCategory<TCategory, EventCategory::Window>);

    /**
     * @brief Event class that is used as a base for all events.
     *
     * @note The current Event System uses this class internally. Using this class directly won't do much, since
     * the constructor is protected, and produced event will be of category and type "None".
     * @note The event stores data in a 16 byte array and each slot is used when it makes sense. The memory layout
     * for the derived classes is as follows:
     * @code{.cpp}
     *  i | Keyboard Event    | Mouse Event       | Window Event
     * ----------------------------------------------------------------------
     * [0]: EventCategory   (Reserved - I32)
     * [1]: -
     * [2]: -
     * [3]: -
     * ----------------------------------------------------------------------
     * [4]: EventType       (Reserved - U8)
     * ----------------------------------------------------------------------
     * [5]: KeyCode (U8)      | MouseButton (U8)  | IsInitiatedByUser (Bool)
     * [6]: (free)            | Count (U8)        | (free)
     * [7]: (free)            | IsPositive (Bool) | (free)
     * ----------------------------------------------------------------------
     * [8]: (free)            | X (U16)           | Width (U16)
     * [9]: (free)            | -                 | -
     * [A]: (free)            | Y (U16)           | Height (U16)
     * [B]: (free)            | -                 | -
     * ----------------------------------------------------------------------
     * [C]: Counter (U16)     | Counter (U16)     | (free)
     * [D]: -                 | -                 | (free)
     * [E]: (free)            | (free)            | (free)
     * [F]: (free)            | (free)            | (free)
     * @endcode
     */
    class Event
    {
    public:
        OTR_WITH_DEFAULT_CONSTRUCTOR(Event)

        /**
         * @brief Copy constructor.
         *
         * @param other The event to copy.
         */
        Event(const Event& other)
        {
            MemorySystem::MemoryCopy(m_Data, other.m_Data, sizeof(m_Data));
        }

        /**
         * @brief Move constructor.
         *
         * @param other The event to move.
         */
        Event(Event&& other) noexcept
        {
            MemorySystem::MemoryCopy(m_Data, other.m_Data, sizeof(m_Data));
            MemorySystem::MemoryClear(other.m_Data, sizeof(m_Data));
        }

        /**
         * @brief Copy assignment operator.
         *
         * @param other The event to copy.
         * @return The event that was copied.
         */
        Event& operator=(const Event& other)
        {
            if (this == &other)
                return *this;

            MemorySystem::MemoryCopy(m_Data, other.m_Data, sizeof(m_Data));

            return *this;
        }

        /**
         * @brief Move assignment operator.
         *
         * @param other The event to move.
         * @return The event that was moved.
         */
        Event& operator=(Event&& other) noexcept
        {
            if (this == &other)
                return *this;

            MemorySystem::MemoryCopy(m_Data, other.m_Data, sizeof(m_Data));
            MemorySystem::MemoryClear(other.m_Data, sizeof(m_Data));

            return *this;
        }

        /**
         * @brief Explicit conversion operator to a specific event type.
         *
         * @tparam TEvent The event to convert to.
         *
         * @return The event converted to the specified type.
         */
        template<typename TEvent>
        requires IsBaseOf<Event, TEvent>
        [[nodiscard]] OTR_INLINE constexpr explicit operator TEvent() const
        {
            return *static_cast<const TEvent*>(this);
        }

        /**
         * @brief Returns the event category of the event.
         *
         * @return The event category of the event.
         */
        [[nodiscard]] OTR_INLINE constexpr EventCategory GetEventCategory() const { return Get<EventCategory>(0); }

        /**
         * @brief Returns the event type of the event.
         *
         * @return The event type of the event.
         */
        [[nodiscard]] OTR_INLINE constexpr EventType GetEventType() const { return Get<EventType>(4); }

        /**
         * @brief Checks if the event is of the specified category.
         *
         * @param category The category to check.
         *
         * @return True if the event is of the specified category, false otherwise.
         */
        [[nodiscard]] OTR_INLINE constexpr bool IsOfCategory(const EventCategory& category) const
        {
            return (GetEventCategory() & category) == category;
        }

        /**
         * @brief Checks if the event is blocking.
         *
         * @return True if the event is blocking, false otherwise.
         */
        [[nodiscard]] OTR_INLINE constexpr bool IsBlocking() const
        {
            return (GetEventCategory() & EventCategory::Blocking) == EventCategory::Blocking;
        }

    protected:
        /**
         * @brief Constructor.
         *
         * @param category The category of the event.
         * @param type The type of the event.
         */
        Event(const EventCategory category, const EventType type)
            : Event()
        {
            Capture(category, 0);
            Capture(type, 4);
        }

        /**
         * @brief Captures a value in the event data.
         *
         * @tparam T The type of the value to capture.
         *
         * @param value The value to capture.
         * @param offset The offset in the event data to capture the value at.
         */
        template<typename T>
        void Capture(const T& value, const UInt64 offset)
        {
            if constexpr (IsSame<T, bool>
                          || IsSame<T, UInt8> || IsSame<T, Int8>
                          || WithUnderlyingType<T, UInt8> || WithUnderlyingType<T, Int8>)
            {
                m_Data[offset] = static_cast<Byte>(value);
            }
            else if constexpr (IsSame<T, UInt16> || IsSame<T, Int16>
                               || WithUnderlyingType<T, UInt16> || WithUnderlyingType<T, Int16>
                               || IsSame<T, UInt32> || IsSame<T, Int32>
                               || WithUnderlyingType<T, UInt32> || WithUnderlyingType<T, Int32>
                               || IsSame<T, UInt64> || IsSame<T, Int64>
                               || WithUnderlyingType<T, UInt64> || WithUnderlyingType<T, Int64>)
            {
                for (Size i = 0; i < sizeof(T); i++)
                    m_Data[i + offset] = ((value >> (i * 8)) & 0xFF);
            }
            else
            {
                MemorySystem::MemoryCopy(&m_Data[offset], &value, sizeof(T));
            }
        }

        /**
         * @brief Gets a value from the event data.
         *
         * @tparam T The type of the value to get.
         *
         * @param offset The offset in the event data to get the value from.
         *
         * @return The value at the specified offset.
         */
        template<typename T>
        [[nodiscard]] T Get(const UInt64 offset) const
        {
            if constexpr (IsSame<T, bool>
                          || IsSame<T, UInt8> || IsSame<T, Int8>
                          || WithUnderlyingType<T, UInt8> || WithUnderlyingType<T, Int8>)
            {
                return static_cast<T>(m_Data[offset]);
            }
            else if constexpr (WithUnderlyingType<T, UInt16> || WithUnderlyingType<T, Int16>
                               || WithUnderlyingType<T, UInt32> || WithUnderlyingType<T, Int32>
                               || WithUnderlyingType<T, UInt64> || WithUnderlyingType<T, Int64>)
            {
                UnderlyingType<T> value = 0;

                for (Size i = 0; i < sizeof(T); i++)
                    value |= (static_cast<T>(m_Data[i + offset]) << (i * 8));

                return (T) value;
            }
            else if constexpr (IsSame<T, UInt16> || IsSame<T, Int16>
                               || IsSame<T, UInt32> || IsSame<T, Int32>
                               || IsSame<T, UInt64> || IsSame<T, Int64>)
            {
                T value = 0;

                for (Size i = 0; i < sizeof(T); i++)
                    value |= (static_cast<T>(m_Data[i + offset]) << (i * 8));

                return value;
            }
            else
            {
                T value = (T) 0.0;
                MemorySystem::MemoryCopy(&value, &m_Data[offset], sizeof(T));

                return value;
            }
        }

    private:
        Byte m_Data[16] = { 0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0 };
    };
}
#endif //OTTERENGINE_EVENT_H
