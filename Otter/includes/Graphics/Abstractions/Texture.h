#ifndef OTTERENGINE_TEXTURE_H
#define OTTERENGINE_TEXTURE_H

#include "Core/Defines.h"
#include "Core/BaseTypes.h"

namespace Otter::Graphics
{
    /**
     * @brief An abstraction for a texture used by the engine.
     */
    class Texture
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~Texture() = default;

        /**
         * @brief Binds the texture to the GPU.
         */
        virtual void Bind() = 0;

        /**
         * @brief Unbinds the texture from the GPU.
         */
        virtual void Unbind() = 0;

        /**
         * @brief Creates a texture from a file.
         *
         * @param filePath The path to the file.
         *
         * @return A pointer to the texture.
         *
         * @note This function has to be implemented by each graphics API.
         */
        [[nodiscard]] static Texture* Create(const char* filePath);

        /**
         * @brief Destroys a texture.
         *
         * @param outTexture A pointer to the texture to destroy.
         *
         * @note This function has to be implemented by each graphics API.
         */
        static void Destroy(Texture* outTexture);

        /**
         * @brief Gets the width of the texture.
         *
         * @return The width of the texture.
         */
        [[nodiscard]] OTR_INLINE UInt32 GetWidth() const { return m_Width; }

        /**
         * @brief Gets the height of the texture.
         *
         * @return The height of the texture.
         */
        [[nodiscard]] OTR_INLINE UInt32 GetHeight() const { return m_Height; }

    protected:
        /**
         * @brief Constructor.
         */
        explicit Texture(const char* const filePath)
            : m_FilePath(filePath), m_Width(0), m_Height(0)
        {
        }

        const char* m_FilePath;

        UInt32 m_Width;
        UInt32 m_Height;
    };
}

#endif //OTTERENGINE_TEXTURE_H
