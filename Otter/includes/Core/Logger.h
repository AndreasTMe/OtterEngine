#ifndef OTTERENGINE_LOGGER_H
#define OTTERENGINE_LOGGER_H

#include <cstring>
#include <string>
#include <sstream>

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Platform/Platform.h"

namespace Otter
{
    /**
     * @brief The class used to log messages to the console. Should not be used directly, use the macros instead.
     * This allows for removing specific log levels from builds.
     */
    class Logger final
    {
    public:
        OTR_DISABLE_OBJECT_COPIES(Logger)
        OTR_DISABLE_OBJECT_MOVES(Logger)

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
        OTR_WITH_DEFAULT_CONSTRUCTOR(Logger);

        LogLevel          m_LogLevel   = LogLevel::Trace;
        std::stringstream m_LogMessage = std::stringstream(std::string());
    };
}

#endif // OTTERENGINE_LOGGER_H
