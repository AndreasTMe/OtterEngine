#ifndef OTTERENGINE_SHADER_H
#define OTTERENGINE_SHADER_H

namespace Otter::Graphics
{
    /**
     * @brief An abstraction for a shader used by the engine.
     */
    class Shader
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~Shader() = default;

        /**
         * @brief Binds the shader.
         */
        virtual void Bind() = 0;

        /**
         * @brief Creates a shader.
         *
         * @param filePath The path to the shader file.
         *
         * @return The pointer to the created shader.
         *
         * @note This function has to be implemented by each graphics API.
         */
        [[nodiscard]] static Shader* Create(const char* filePath);

        /**
         * @brief Destroys a shader.
         *
         * @param outShader The pointer to the shader to destroy.
         *
         * @note This function has to be implemented by each graphics API.
         */
        static void Destroy(Shader* outShader);

    protected:
        /**
         * @brief Constructor.
         *
         * @param filePath The path to the shader file.
         */
        explicit Shader(const char* const filePath)
            : m_FilePath(filePath)
        {
        }

        const char* m_FilePath;
    };
}

#endif //OTTERENGINE_SHADER_H
