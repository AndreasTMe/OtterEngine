#ifndef OTTERENGINE_LOGGER_H
#define OTTERENGINE_LOGGER_H

#include <cstring>
#include <string>
#include <sstream>

#include "Core/Defines.h"
#include "Core/BaseTypes.h"
#include "Platform/Platform.h"

namespace Otter
{
    /**
     * @brief The type of the log level. Its size is 1 byte.
     */
    enum class LogLevel : UInt8
    {
        /// @brief The trace log level.
        Trace = 0x00,

        /// @brief The debug log level.
        Debug = 0x01,

        /// @brief The info log level.
        Info = 0x02,

        /// @brief The warning log level.
        Warning = 0x03,

        /// @brief The error log level.
        Error = 0x04,

        /// @brief The fatal log level.
        Fatal = 0x05,
    };

    /**
     * @brief The class used to log messages to the console. Should not be used directly, use the macros instead.
     * This allows for removing specific log levels from builds.
     */
    class Logger final
    {
    public:
        /**
         * @brief Deleted copy constructor.
         */
        Logger(Logger&) = delete;

        /**
         * @brief Deleted copy constructor.
         */
        Logger(const Logger&) = delete;

        /**
         * @brief Deleted copy assignment operator.
         */
        Logger& operator=(const Logger&) = delete;

        /**
         * @brief Deleted move constructor.
         */
        Logger(Logger&&) = delete;

        /**
         * @brief Deleted move assignment operator.
         */
        Logger& operator=(Logger&&) = delete;

        /**
         * @brief The logger builder/singleton.
         *
         * @return The logger.
         */
        [[nodiscard]] OTR_INLINE static Logger* GetBuilder()
        {
            static Logger builder;
            return &builder;
        }

        /**
         * @brief Prepares the logger for logging a message, and formats the message.
         *
         * @param logLevel The log level of the message.
         * @param message The message to log.
         * @param args Additional arguments to insert into the message using the format {0}, {1}, etc.
         *
         * @return The logger.
         */
        template<typename... TArgs>
        Logger* Prepare(LogLevel logLevel, const char* message, TArgs&& ... args)
        {
            m_LogLevel = logLevel;
            m_LogMessage.clear();

            std::stringstream ss = std::stringstream("");
            switch (m_LogLevel)
            {
                case LogLevel::Trace:
                    ss << "[TRACE]: ";
                    break;
                case LogLevel::Debug:
                    ss << "[DEBUG]: ";
                    break;
                case LogLevel::Info:
                    ss << "[INFO]: ";
                    break;
                case LogLevel::Warning:
                    ss << "[WARN]: ";
                    break;
                case LogLevel::Error:
                    ss << "[ERROR]: ";
                    break;
                case LogLevel::Fatal:
                    ss << "[FATAL]: ";
                    break;

#pragma clang diagnostic push
#pragma ide diagnostic ignored "UnreachableCode"

                default:
                    ss << "[INVALID LOG LEVEL]: ";
                    break;

#pragma clang diagnostic pop
            }

            ss << message;

            int         index     = 0;
            std::string logString = ss.str();
            std::string toReplace = '{' + std::to_string(index) + '}';

            ss.str(std::string());
            ([&]
            {
                try
                {
                    auto found = logString.find(toReplace);
                    if (!found)
                        return;

                    ss << args;
                    logString.replace(found, toReplace.length(), ss.str());

                    index++;
                    toReplace = '{' + std::to_string(index) + '}';
                    ss.str(std::string());
                }
                catch (...)
                {
                    ss.str(std::string());
                    m_LogLevel = LogLevel::Fatal;
                    logString  = "[FATAL]: Failed to format log message, invalid arguments passed.";
                }
            }(), ...);

            m_LogMessage << logString.c_str();

            return this;
        }

        /**
         * @brief Prepares the logger for logging an assertion failure, and formats the message.
         *
         * @param assertion The assertion that failed.
         * @param message The message to log.
         * @param args Additional arguments to insert into the message using the format {0}, {1}, etc.
         *
         * @return The logger.
         */
        template<typename... TArgs>
        Logger* PrepareAssertion(const char* assertion, const char* message, TArgs&& ... args)
        {
            std::string assertionMessage = "Assertion failed: ";
            assertionMessage += assertion;

            if (message != nullptr)
            {
                assertionMessage += ", ";
                assertionMessage += message;
            }

            return Prepare(LogLevel::Fatal, assertionMessage.c_str(), args...);
        }

        /**
         * @brief Captures the source of the log message.
         *
         * @param file The file the log message was created in.
         * @param line The line the log message was created on.
         *
         * @return The logger.
         */
        Logger* CaptureSource(const char* file, UInt32 line)
        {
            m_LogMessage << "\n\tfrom " << file << ":" << line << std::endl;

            return this;
        }

        /**
         * @brief Logs the message to the console and clears the internal message buffer.
         */
        void Log()
        {
            Platform::Log(m_LogMessage.str().c_str(), (UInt8) m_LogLevel);
            m_LogMessage.str(std::string());
        }

    private:
        /**
         * @brief Constructor.
         */
        Logger() = default;

        /**
         * @brief Destructor.
         */
        ~Logger() = default;

        LogLevel          m_LogLevel   = LogLevel::Trace;
        std::stringstream m_LogMessage = std::stringstream(std::string());
    };
}

#if OTR_DEBUG

#define OTR_LOG_TRACE(...)                              \
    Otter::Logger::GetBuilder()                         \
        ->Prepare(Otter::LogLevel::Trace, __VA_ARGS__)  \
        ->CaptureSource(__FILE__, __LINE__)             \
        ->Log();
#define OTR_LOG_DEBUG(...)                              \
    Otter::Logger::GetBuilder()                         \
        ->Prepare(Otter::LogLevel::Debug, __VA_ARGS__)  \
        ->CaptureSource(__FILE__, __LINE__)             \
        ->Log();
#define OTR_LOG_INFO(...)                               \
    Otter::Logger::GetBuilder()                         \
        ->Prepare(Otter::LogLevel::Info, __VA_ARGS__)   \
        ->CaptureSource(__FILE__, __LINE__)             \
        ->Log();
#define OTR_LOG_WARNING(...)                            \
    Otter::Logger::GetBuilder()                         \
        ->Prepare(Otter::LogLevel::Warning, __VA_ARGS__)\
        ->CaptureSource(__FILE__, __LINE__)             \
        ->Log();
#define OTR_LOG_ERROR(...)                              \
    Otter::Logger::GetBuilder()                         \
        ->Prepare(Otter::LogLevel::Error, __VA_ARGS__)  \
        ->CaptureSource(__FILE__, __LINE__)             \
        ->Log();
#define OTR_LOG_FATAL(...)                              \
    Otter::Logger::GetBuilder()                         \
        ->Prepare(Otter::LogLevel::Fatal, __VA_ARGS__)  \
        ->CaptureSource(__FILE__, __LINE__)             \
        ->Log();

#elif OTR_EDITOR

#define OTR_LOG_TRACE(...)
#define OTR_LOG_DEBUG(...)
#define OTR_LOG_INFO(...)                               \
    Otter::Logger::GetBuilder()                         \
        ->Prepare(Otter::LogLevel::Info, __VA_ARGS__)   \
        ->CaptureSource(__FILE__, __LINE__)             \
        ->Log();
#define OTR_LOG_WARNING(...)                            \
    Otter::Logger::GetBuilder()                         \
        ->Prepare(Otter::LogLevel::Warning, __VA_ARGS__)\
        ->CaptureSource(__FILE__, __LINE__)             \
        ->Log();
#define OTR_LOG_ERROR(...)                              \
    Otter::Logger::GetBuilder()                         \
        ->Prepare(Otter::LogLevel::Error, __VA_ARGS__)  \
        ->CaptureSource(__FILE__, __LINE__)             \
        ->Log();
#define OTR_LOG_FATAL(...)                              \
    Otter::Logger::GetBuilder()                         \
        ->Prepare(Otter::LogLevel::Fatal, __VA_ARGS__)  \
        ->CaptureSource(__FILE__, __LINE__)             \
        ->Log();

#elif OTR_RUNTIME

#define OTR_LOG_TRACE(...)
#define OTR_LOG_DEBUG(...)
#define OTR_LOG_INFO(...)
#define OTR_LOG_WARNING(...)
#define OTR_LOG_ERROR(...)
#define OTR_LOG_FATAL(...)                              \
    Otter::Logger::GetBuilder()                         \
        ->Prepare(Otter::LogLevel::Fatal, __VA_ARGS__)  \
        ->CaptureSource(__FILE__, __LINE__)             \
        ->Log();

#endif

template<typename OStream>
OStream& operator<<(OStream& os, const Otter::LogLevel& logLevel)
{
    switch (logLevel)
    {
        case Otter::LogLevel::Trace:
            os << OTR_NAME_OF(Otter::LogLevel::Trace);
            break;
        case Otter::LogLevel::Debug:
            os << OTR_NAME_OF(Otter::LogLevel::Debug);
            break;
        case Otter::LogLevel::Info:
            os << OTR_NAME_OF(Otter::LogLevel::Info);
            break;
        case Otter::LogLevel::Warning:
            os << OTR_NAME_OF(Otter::LogLevel::Warning);
            break;
        case Otter::LogLevel::Error:
            os << OTR_NAME_OF(Otter::LogLevel::Error);
            break;
        case Otter::LogLevel::Fatal:
            os << OTR_NAME_OF(Otter::LogLevel::Fatal);
            break;
        default:
            os << "Unknown LogLevel";
    }

    return os;
}

#endif // OTTERENGINE_LOGGER_H
