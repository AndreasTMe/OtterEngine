#ifndef OTTERENGINE_MEMORYFOOTPRINT_H
#define OTTERENGINE_MEMORYFOOTPRINT_H

#include "Core/Defines.h"
#include "Core/BaseTypes.h"

namespace Otter
{
    /**
     * @brief A struct used for debugging purposes. It holds a name and a pointer to a resource.
     */
    struct MemoryDebugPair final
    {
    public:
        /**
         * @brief Default constructor.
         */
        MemoryDebugPair()
            : m_Name{ }, m_Ptr(nullptr)
        {
        }

        /**
         * @brief Constructor.
         *
         * @param name The name of the resource.
         * @param ptr The pointer to the resource.
         */
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

        /**
         * @brief Getter for the name of the resource.
         *
         * @return The name of the resource.
         */
        [[nodiscard]] OTR_INLINE const std::string GetName() const noexcept { return std::string(m_Name); }

        /**
         * @brief Getter for the pointer to the resource.
         *
         * @return The pointer to the resource.
         */
        [[nodiscard]] OTR_INLINE const void* GetPointer() const noexcept { return m_Ptr; }

    private:
        char m_Name[32] = { };
        void* m_Ptr = nullptr;
    };

    /**
     * @brief A struct used for debugging purposes. It holds an array of MemoryDebugPair structs and the size of the array.
     */
    struct MemoryDebugHandle final
    {
    public:
        MemoryDebugPair* Pairs = nullptr;
        UInt64 Size = 0;
    };

    /**
     * @brief A struct used for debugging purposes. It holds information about a memory block's footprint,
     * such as its size, offset, padding and alignment.
     */
    struct MemoryFootprint final
    {
    public:
        UInt64 Size;
        UInt64 Offset;
        UInt16 Padding;
        UInt16 Alignment;

        /**
         * @brief Default constructor.
         */
        MemoryFootprint()
            : m_DebugPair{ }, Size(0), Offset(0), Padding(0), Alignment(0)
        {
        }

        /**
         * @brief Factory that creates a MemoryFootprint struct.
         *
         * @param debugPair The debug pair to create the MemoryFootprint struct from.
         *
         * @return The created MemoryFootprint struct.
         */
        [[nodiscard]] OTR_INLINE static MemoryFootprint For(const MemoryDebugPair& debugPair)
        {
            return MemoryFootprint(debugPair);
        }

        /**
         * @brief Getter for the debug pair.
         *
         * @return The debug pair.
         */
        [[nodiscard]] OTR_INLINE const MemoryDebugPair GetData() const noexcept { return m_DebugPair; }

        /**
         * @brief Checks if the debug pair is valid.
         *
         * @return True if the debug pair is valid, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsValid() const noexcept { return m_DebugPair.GetPointer() && Size > 0; }

        /**
         * @brief Checks if the memory block is aligned.
         *
         * @param alignment The alignment to check for. If 0, the memory block's actual alignment is used.
         *
         * @return True if the memory block is aligned, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool IsAligned(const UInt16 alignment = 0) const noexcept
        {
            if (alignment == 0)
                return Offset % Alignment == 0;

            return Offset % alignment == 0;
        }

        /**
         * @brief Checks if the memory block has an offset.
         *
         * @return True if the memory block has an offset, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool HasOffset() const noexcept { return Offset > 0; }

        /**
         * @brief Checks if the memory block has padding.
         *
         * @return True if the memory block has padding, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool HasPadding() const noexcept { return Padding > 0; }

        /**
         * @brief Checks if the memory block has an alignment.
         *
         * @return True if the memory block has an alignment, false otherwise.
         */
        [[nodiscard]] OTR_INLINE bool HasAlignment() const noexcept { return Alignment > 0; }

    private:
        MemoryDebugPair m_DebugPair;

        /**
         * @brief Constructor.
         *
         * @param debugPair The debug pair to create the MemoryFootprint struct from.
         */
        explicit MemoryFootprint(const MemoryDebugPair& debugPair)
            : m_DebugPair(debugPair), Size(0), Offset(0), Padding(0), Alignment(0)
        {
        }
    };
}

#endif //OTTERENGINE_MEMORYFOOTPRINT_H
