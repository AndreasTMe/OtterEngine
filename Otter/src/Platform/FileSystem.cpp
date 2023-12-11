#include "Platform/FileSystem.h"

#include <sys/stat.h>

#include "Core/Logger.h"

namespace Otter::FileSystem
{
#define OTR_FILE_VALIDATE(file, ...)    \
    if (!file->Handle || !file->IsValid)\
    {                                   \
        OTR_LOG_ERROR(__VA_ARGS__)      \
        return false;                   \
    }

    bool FileExists(const char* const filePath)
    {
#ifdef _MSC_VER
        struct _stat buffer{};
        return _stat(filePath, &buffer) == 0;
#else
        struct stat buffer{ };
        return stat(filePath, &buffer) == 0;
#endif
    }

    bool TryGetFileSize(const File* const file, Size* outFileSize)
    {
        OTR_FILE_VALIDATE(file, "Invalid file pointer passed while trying to get file size")

        fseek((FILE*) file->Handle, 0, SEEK_END);
        *outFileSize = ftell((FILE*) file->Handle);
        rewind((FILE*) file->Handle);

        return true;
    }

    bool TryOpenFile(const char* const filePath, const OpenMode& openMode, const bool& isBinary, File* outFile)
    {
        outFile->Handle  = nullptr;
        outFile->IsValid = false;

        const char* mode;
        switch (openMode)
        {
            case OpenMode::Read:
                mode = isBinary ? "rb" : "r";
                break;
            case OpenMode::Write:
                mode = isBinary ? "wb" : "w";
                break;
            case OpenMode::Append:
                mode = isBinary ? "ab" : "a";
                break;
            case OpenMode::ReadWrite:
                mode = isBinary ? "r+b" : "r+";
                break;
            case OpenMode::ReadAppend:
                mode = isBinary ? "a+b" : "a+";
                break;
            case OpenMode::Overwrite:
                mode = isBinary ? "w+b" : "w+";
                break;
            default:
            {
                OTR_LOG_ERROR("Invalid file open mode passed while trying to open file: {0}", filePath)
            }
                return false;
        }

        outFile->Handle  = fopen(filePath, mode);
        outFile->IsValid = outFile->Handle != nullptr;

        OTR_FILE_VALIDATE(outFile, "Failed to open file: {0}", filePath)

        return outFile->IsValid;
    }

    void CloseFile(File* file)
    {
        if (file->IsValid)
            fclose((FILE*) file->Handle);

        file->Handle  = nullptr;
        file->IsValid = false;
    }

    bool TryReadFile(const File* const file, const Size& bufferSize, void* outBuffer, Size* outBytesRead)
    {
        OTR_FILE_VALIDATE(file, "Invalid file pointer passed while trying to read file")

        *outBytesRead = fread(outBuffer, 1, bufferSize, (FILE*) file->Handle);

        return true;
    }

    bool TryWriteFile(const File* const file, const Size& bufferSize, const void* const buffer, Size* outBytesWritten)
    {
        OTR_FILE_VALIDATE(file, "Invalid file pointer passed while trying to write file")

        *outBytesWritten = fwrite(buffer, 1, bufferSize, (FILE*) file->Handle);
        if (*outBytesWritten != bufferSize)
        {
            OTR_LOG_ERROR("Failed to write all bytes to file")
            return false;
        }

        fflush((FILE*) file->Handle);

        return true;
    }

    bool TryAppendFile(const File* const file, const Size& bufferSize, const void* buffer, Size* outBytesWritten)
    {
        OTR_FILE_VALIDATE(file, "Invalid file pointer passed while trying to append file")

        fseek((FILE*) file->Handle, 0, SEEK_END);

        *outBytesWritten = fwrite(buffer, 1, bufferSize, (FILE*) file->Handle);
        if (*outBytesWritten != bufferSize)
        {
            OTR_LOG_ERROR("Failed to append all bytes to file")
            return false;
        }

        fflush((FILE*) file->Handle);

        return true;
    }

    bool TryReadAllBytes(const File* const file, void* outBuffer, Size* outBytesRead)
    {
        OTR_FILE_VALIDATE(file, "Invalid file pointer passed while trying to read file")

        UInt64 fileSize = 0;
        if (!TryGetFileSize(file, &fileSize))
        {
            OTR_LOG_ERROR("Failed to get file size while trying to read file")
            return false;
        }

        *outBytesRead = fread(outBuffer, 1, fileSize, (FILE*) file->Handle);

        return *outBytesRead == fileSize;
    }

    bool TryReadAllText(const File* const file, char* outTextBuffer, Size* outBytesRead)
    {
        OTR_FILE_VALIDATE(file, "Invalid file pointer passed while trying to read file")

        UInt64 fileSize = 0;
        if (!TryGetFileSize(file, &fileSize))
        {
            OTR_LOG_ERROR("Failed to get file size while trying to read file")
            return false;
        }

        *outBytesRead = fread(outTextBuffer, 1, fileSize, (FILE*) file->Handle);

        return *outBytesRead == fileSize;
    }

    bool TryReadLine(const File* const file, char* outBuffer, Size* outBytesRead)
    {
        OTR_FILE_VALIDATE(file, "Invalid file pointer passed while trying to read line")

        *outBytesRead = 0;
        char c = (char) fgetc((FILE*) file->Handle);
        while (c != '\n' && c != EOF)
        {
            *outBuffer = c;
            ++outBuffer;
            ++(*outBytesRead);
            c = (char) fgetc((FILE*) file->Handle);
        }

        return true;
    }

    bool TryWriteLine(const File* const file, const char* const buffer, Size* outBytesWritten)
    {
        OTR_FILE_VALIDATE(file, "Invalid file pointer passed while trying to write line")

        *outBytesWritten = 0;
        for (UInt64 i = 0; buffer[i] != '\0'; i++)
        {
            fputc(buffer[i], (FILE*) file->Handle);
            ++(*outBytesWritten);
        }

        fputc('\n', (FILE*) file->Handle);
        ++(*outBytesWritten);

        return true;
    }

    bool TryAppendLine(const File* const file, const char* const buffer, Size* outBytesWritten)
    {
        OTR_FILE_VALIDATE(file, "Invalid file pointer passed while trying to append line")

        fseek((FILE*) file->Handle, 0, SEEK_END);

        *outBytesWritten = 0;
        for (UInt64 i = 0; buffer[i] != '\0'; i++)
        {
            fputc(buffer[i], (FILE*) file->Handle);
            ++(*outBytesWritten);
        }

        fputc('\n', (FILE*) file->Handle);
        ++(*outBytesWritten);

        return true;
    }

#undef OTR_FILE_VALIDATE
}
