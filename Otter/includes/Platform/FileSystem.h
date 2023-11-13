#ifndef OTTERENGINE_FILESYSTEM_H
#define OTTERENGINE_FILESYSTEM_H

namespace Otter::FileSystem
{
    struct File
    {
        void* Handle = nullptr;
        bool IsValid = false;
    };

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

    bool FileExists(const char* filePath);
    bool TryGetFileSize(const File* file, Size* outFileSize);

    bool TryOpenFile(const char* filePath, const OpenMode& openMode, const bool& isBinary, File* outFile);
    void CloseFile(File * file);

    bool TryReadFile(const File* file, const Size& bufferSize, void* outBuffer, Size* outBytesRead);
    bool TryWriteFile(const File* file, const Size& bufferSize, const void* buffer, Size* outBytesWritten);
    bool TryAppendFile(const File* file, const Size& bufferSize, const void* buffer, Size* outBytesWritten);

    bool TryReadAllBytes(const File* file, void* outBuffer, Size* outBytesRead);
    bool TryReadAllText(const File* file, char* outTextBuffer, Size* outBytesRead);

    bool TryReadLine(const File* file, char* outBuffer, Size* outBytesRead);
    bool TryWriteLine(const File* file, const char* buffer, Size* outBytesWritten);
    bool TryAppendLine(const File* file, const char* buffer, Size* outBytesWritten);
}

#endif //OTTERENGINE_FILESYSTEM_H
