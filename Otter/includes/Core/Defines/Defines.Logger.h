#ifndef OTTERENGINE_DEFINES_LOGGER_H
#define OTTERENGINE_DEFINES_LOGGER_H

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

#endif //OTTERENGINE_DEFINES_LOGGER_H
