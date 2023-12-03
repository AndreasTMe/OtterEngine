#ifndef OTTERENGINE_MEMORYFOOTPRINT_H
#define OTTERENGINE_MEMORYFOOTPRINT_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Collections/Utils/KeyValuePair.h"

namespace Otter
{
    struct DebugHandle final
    {
    public:
        KeyValuePair<const char*, void*>* Data = nullptr;
        UInt64                          Size   = 0;
    };

    struct MemoryFootprint final
    {
    public:
        UInt64 Size;
        UInt64 Offset;
        UInt16 Padding;
        UInt16 Alignment;

        MemoryFootprint() = default;

        [[nodiscard]] OTR_INLINE static MemoryFootprint For(const KeyValuePair<const char*, void*>& data)
        {
            return MemoryFootprint(data);
        }

        [[nodiscard]] OTR_INLINE const KeyValuePair<const char*, void*> GetData() const noexcept { return m_Data; }

        [[nodiscard]] OTR_INLINE bool IsValid() const noexcept { return m_Data.Key && m_Data.Value && Size > 0; }
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
        KeyValuePair<const char*, void*> m_Data;

        explicit MemoryFootprint(const KeyValuePair<const char*, void*>& data)
            : m_Data(data), Size(0), Offset(0), Padding(0), Alignment(0)
        {
        }
    };
}

#endif //OTTERENGINE_MEMORYFOOTPRINT_H
