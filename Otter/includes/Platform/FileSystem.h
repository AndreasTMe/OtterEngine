#ifndef OTTERENGINE_FILESYSTEM_H
#define OTTERENGINE_FILESYSTEM_H

#include "Core/BaseTypes.h"

namespace Otter::FileSystem
{
    /**
     * @brief Structure that represents a file. Holds a handle to the file and a boolean that indicates if its validity.
     */
    struct File
    {
        void* Handle = nullptr;
        bool IsValid = false;
    };

    /**
     * @brief Enum that represents the different modes that a file can be opened. Its size is 1 byte.
     */
    enum class OpenMode : UInt8
    {
        None       = 0x00,
        Read       = 0x01,
        Write      = 0x02,
        Append     = 0x03,
        ReadWrite  = 0x04,
        ReadAppend = 0x05,
        Overwrite  = 0x06,
    };

    /**
     * @brief Checks if a file exists.
     *
     * @param filePath The path to the file to check.
     *
     * @return True if the file exists, false otherwise.
     */
    bool FileExists(const char* filePath);

    /**
     * @brief Tries to get the size of a file in bytes.
     *
     * @param file The file to get the size of.
     * @param outFileSize The size of the file in bytes.
     *
     * @return True if the file size was successfully retrieved, false otherwise.
     */
    bool TryGetFileSize(const File* file, Size* outFileSize);

    /**
     * @brief Tries to open a file.
     *
     * @param filePath The path to the file to open.
     * @param openMode The mode to open the file in.
     * @param isBinary Indicates if the file is binary or not.
     * @param outFile The file that was opened.
     *
     * @return True if the file was successfully opened, false otherwise.
     */
    bool TryOpenFile(const char* filePath, const OpenMode& openMode, const bool& isBinary, File* outFile);

    /**
     * @brief Closes a file.
     *
     * @param file The file to close.
     */
    void CloseFile(File* file);

    /**
     * @brief Tries to read a file.
     *
     * @param file The file to read.
     * @param bufferSize The size of the buffer to read the file into.
     * @param outBuffer The buffer to read the file into.
     * @param outBytesRead The number of bytes actually read.
     *
     * @return True if the file was successfully read, false otherwise.
     */
    bool TryReadFile(const File* file, const Size& bufferSize, void* outBuffer, Size* outBytesRead);

    /**
     * @brief Tries to write to a file.
     *
     * @param file The file to write to.
     * @param bufferSize The size of the buffer to write to the file.
     * @param buffer The buffer to write to the file.
     * @param outBytesWritten The number of bytes actually written.
     *
     * @return True if the file was successfully written to, false otherwise.
     */
    bool TryWriteFile(const File* file, const Size& bufferSize, const void* buffer, Size* outBytesWritten);

    /**
     * @brief Tries to append to a file.
     *
     * @param file The file to append to.
     * @param bufferSize The size of the buffer to append to the file.
     * @param buffer The buffer to append to the file.
     * @param outBytesWritten The number of bytes actually written.
     *
     * @return True if the file was successfully appended to, false otherwise.
     */
    bool TryAppendFile(const File* file, const Size& bufferSize, const void* buffer, Size* outBytesWritten);

    /**
     * @brief Tries to read all bytes from a file.
     *
     * @param file The file to read from.
     * @param outBuffer The buffer to read the file into.
     * @param outBytesRead The number of bytes actually read.
     *
     * @return True if the file was successfully read, false otherwise.
     */
    bool TryReadAllBytes(const File* file, void* outBuffer, Size* outBytesRead);

    /**
     * @brief Tries to read all text from a file.
     *
     * @param file The file to read from.
     * @param outTextBuffer The buffer to read the file into.
     * @param outBytesRead The number of bytes actually read.
     *
     * @return True if the file was successfully read, false otherwise.
     */
    bool TryReadAllText(const File* file, char* outTextBuffer, Size* outBytesRead);

    /**
     * @brief Tries to read a line from a file.
     *
     * @param file The file to read from.
     * @param outBuffer The buffer to read the line into.
     * @param outBytesRead The number of bytes actually read.
     *
     * @return True if the line was successfully read, false otherwise.
     */
    bool TryReadLine(const File* file, char* outBuffer, Size* outBytesRead);

    /**
     * @brief Tries to write a line to a file.
     *
     * @param file The file to write to.
     * @param buffer The buffer to write to the file.
     * @param outBytesWritten The number of bytes actually written.
     *
     * @return True if the line was successfully written, false otherwise.
     */
    bool TryWriteLine(const File* file, const char* buffer, Size* outBytesWritten);

    /**
     * @brief Tries to append a line to a file.
     *
     * @param file The file to append to.
     * @param buffer The buffer to append to the file.
     * @param outBytesWritten The number of bytes actually written.
     *
     * @return True if the line was successfully appended, false otherwise.
     */
    bool TryAppendLine(const File* file, const char* buffer, Size* outBytesWritten);
}

#endif //OTTERENGINE_FILESYSTEM_H
