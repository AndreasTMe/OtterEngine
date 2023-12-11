#ifndef OTTERENGINE_PLATFORM_H
#define OTTERENGINE_PLATFORM_H

#include "Core/Defines.h"
#include "Core/BaseTypes.h"

namespace Otter
{
    /**
     * @brief The state of the current window. Its size is 1 byte.
     */
    enum class WindowState : UInt8
    {
        /// @brief No state.
        None = 0x00,

        /// @brief Default state (windowed).
        Default = 0x01,

        /// @brief Window is maximised.
        Maximised = 0x02,

        /// @brief Window is minimised.
        Minimised = 0x03,

        /// @brief Maximum value of the enum.
        Max = 0xFF
    };

    /**
     * @brief Configuration structure. Contains all the information needed to create a window for a platform, such as
     * the window title, size, etc.
     */
    struct PlatformConfiguration final
    {
        const char* Title = nullptr;
        UInt16      Width  = 0;
        UInt16      Height = 0;
        WindowState State  = WindowState::Default;
    };

    /**
     * @brief The context of the platform. It is a void pointer to platform-specific data.
     */
    struct PlatformContext
    {
        void* Data;
    };

    /**
     * @brief The platform class. It is the base class for all the platforms.
     *
     * @note Contains all the platform-specific functions and is used to initialise a window, capture events, handle
     * memory and time, etc.
     */
    class Platform
    {
    public:
        /**
         * @brief Constructor.
         */
        Platform() = default;

        /**
         * @brief Destructor.
         */
        virtual ~Platform() = default;

        /**
         * @brief Initialises the platform and creates a window.
         *
         * @return True if the platform was initialised successfully, false otherwise.
         */
        virtual bool TryInitialise() = 0;

        /**
         * @brief Shuts down the platform and destroys the window.
         */
        virtual void Shutdown() = 0;

        /**
         * @brief Captures all window events.
         */
        virtual void CaptureWindowEvents() = 0;

        /**
         * @brief Checks if the platform is currently running.
         *
         * @return True if the platform is running, false otherwise.
         *
         * @note This function is used from the main loop to check if the platform is still running. While it is running,
         * the main loop will continue to run. When it returns false, the main loop will stop and the application will
         * eventually exit.
         */
        [[nodiscard]] virtual bool IsRunning() const = 0;

        /**
         * @brief Getter for the window width.
         *
         * @return The width of the window.
         */
        [[nodiscard]] virtual UInt16 GetWidth() const = 0;

        /**
         * @brief Getter for the window height.
         *
         * @return The height of the window.
         */
        [[nodiscard]] virtual UInt16 GetHeight() const = 0;

        /**
         * @brief Getter for the platform absolute time.
         *
         * @return The absolute time of the platform.
         *
         * @note This function is used to calculate the delta time between frames.
         */
        [[nodiscard]] virtual Double64 GetAbsoluteTime() const = 0;

        /**
         * @brief Getter for the platform context.
         *
         * @return The context of the platform.
         *
         * @note This function is used to initialise other systems that require a window pointer,
         * like the graphics system.
         */
        [[nodiscard]] const void* GetUnsafeContext() const { return m_Context; }

        /**
         * @brief Creates a platform.
         *
         * @param configuration The configuration of the platform.
         *
         * @return A pointer to the platform.
         *
         * @note This function is implemented for each platform separately.
         */
        [[nodiscard]] static Platform* CreatePlatform(const PlatformConfiguration& configuration);

        /**
         * @brief Destroys a platform.
         *
         * @param platform The platform to destroy.
         *
         * @note This function is implemented for each platform separately.
         */
        static void DestroyPlatform(Platform* platform);

        /**
         * @brief Allocates memory.
         *
         * @param size The size of the memory to allocate.
         *
         * @return A pointer to the allocated memory.
         *
         * @note This function is implemented for each platform separately.
         */
        [[nodiscard]] static void* Allocate(UInt64 size);

        /**
         * @brief Reallocates memory.
         *
         * @param block The block of memory to reallocate.
         * @param size The size of the memory to reallocate.
         *
         * @return A pointer to the reallocated memory.
         *
         * @note This function is implemented for each platform separately.
         */
        [[nodiscard]] static void* Reallocate(void* block, UInt64 size);

        /**
         * @brief Frees memory.
         *
         * @param block The block of memory to free.
         *
         * @note This function is implemented for each platform separately.
         */
        static void Free(void* block);

        /**
         * @brief Copies memory.
         *
         * @param destination The destination of the memory copy.
         * @param source The source of the memory copy.
         * @param size The size of the memory to copy.
         *
         * @note This function is implemented for each platform separately.
         */
        static void MemoryCopy(void* destination, const void* source, UInt64 size);

        /**
         * @brief Clears memory.
         *
         * @param block The block of memory to clear.
         * @param size The size of the memory to clear.
         *
         * @note This function is implemented for each platform separately.
         */
        static void MemoryClear(void* block, UInt64 size);

        /**
         * @brief Used to sleep for a certain amount of milliseconds.
         *
         * @param value The amount of milliseconds to sleep for.
         *
         * @note This function is implemented for each platform separately.
         */
        static void SleepForMilliseconds(UInt64 value);

        /**
         * @brief Logs a message.
         *
         * @param message The message to log.
         * @param level The level of importance of the message.
         *
         * @note This function is implemented for each platform separately.
         */
        static void Log(const char* message, UInt8 level);

    protected:
        PlatformContext* m_Context{ };
    };
}

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::WindowState& windowState)
{
    switch (windowState)
    {
        case Otter::WindowState::None:
            os << OTR_NAME_OF(Otter::WindowState::None);
            break;
        case Otter::WindowState::Default:
            os << OTR_NAME_OF(Otter::WindowState::Default);
            break;
        case Otter::WindowState::Maximised:
            os << OTR_NAME_OF(Otter::WindowState::Maximised);
            break;
        case Otter::WindowState::Minimised:
            os << OTR_NAME_OF(Otter::WindowState::Minimised);
            break;
        case Otter::WindowState::Max:
            os << OTR_NAME_OF(Otter::WindowState::Max);
            break;
        default:
            os << "Unknown WindowState";
    }

    return os;
}

#endif //OTTERENGINE_PLATFORM_H
