#ifndef OTTERENGINE_SHADER_H
#define OTTERENGINE_SHADER_H

namespace Otter::Graphics
{
    class Shader
    {
    public:
        virtual ~Shader() = default;

        virtual void Bind() = 0;

        [[nodiscard]] static Shader* Create(const char* filePath) { return nullptr; }

    protected:
        explicit Shader(const char* const filePath)
            : m_FilePath(filePath)
        {
        }

        const char* m_FilePath;
    };
}

#endif //OTTERENGINE_SHADER_H
