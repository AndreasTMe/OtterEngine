#ifndef OTTERENGINE_MEMORYFOOTPRINT_H
#define OTTERENGINE_MEMORYFOOTPRINT_H

#include "Core/Defines.h"
#include "Core/Types.h"

namespace Otter
{
    struct MemoryDebugPair final
    {
    public:
        MemoryDebugPair()
            : m_Name{ }, m_Ptr(nullptr)
        {
        }

        MemoryDebugPair(const char* const name, void* ptr)
            : m_Ptr(ptr)
        {
            for (UInt8 i = 0; i < 32; ++i)
            {
                if (name[i] == '\0')
                    break;

                m_Name[i] = name[i];
            }
        }

        [[nodiscard]] OTR_INLINE const std::string GetName() const noexcept { return std::string(m_Name); }
        [[nodiscard]] OTR_INLINE const void* GetPointer() const noexcept { return m_Ptr; }

    private:
        char m_Name[32] = { };
        void* m_Ptr = nullptr;
    };

    struct MemoryDebugHandle final
    {
    public:
        MemoryDebugPair* Pairs = nullptr;
        UInt64 Size = 0;
    };

    struct MemoryFootprint final
    {
    public:
        UInt64 Size;
        UInt64 Offset;
        UInt16 Padding;
        UInt16 Alignment;

        MemoryFootprint()
            : m_DebugPair{ }, Size(0), Offset(0), Padding(0), Alignment(0)
        {
        }

        [[nodiscard]] OTR_INLINE static MemoryFootprint For(const MemoryDebugPair& debugPair)
        {
            return MemoryFootprint(debugPair);
        }

        [[nodiscard]] OTR_INLINE const MemoryDebugPair GetData() const noexcept { return m_DebugPair; }

        [[nodiscard]] OTR_INLINE bool IsValid() const noexcept { return m_DebugPair.GetPointer() && Size > 0; }
        [[nodiscard]] OTR_INLINE bool IsAligned(const UInt16 alignment = 0) const noexcept
        {
            if (alignment == 0)
                return Offset % Alignment == 0;

            return Offset % alignment == 0;
        }

        [[nodiscard]] OTR_INLINE bool HasOffset() const noexcept { return Offset > 0; }
        [[nodiscard]] OTR_INLINE bool HasPadding() const noexcept { return Padding > 0; }
        [[nodiscard]] OTR_INLINE bool HasAlignment() const noexcept { return Alignment > 0; }

    private:
        MemoryDebugPair m_DebugPair;

        explicit MemoryFootprint(const MemoryDebugPair& debugPair)
            : m_DebugPair(debugPair), Size(0), Offset(0), Padding(0), Alignment(0)
        {
        }
    };
}

#endif //OTTERENGINE_MEMORYFOOTPRINT_H
