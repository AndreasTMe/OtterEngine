#ifndef OTTERENGINE_ABSTRACTSHADER_H
#define OTTERENGINE_ABSTRACTSHADER_H

#include <string>

namespace Otter::Graphics
{
    class AbstractShader
    {
    public:
        virtual ~AbstractShader() = default;

        virtual void Bind() = 0;

        [[nodiscard]] static AbstractShader* Create(const char* filePath);

    protected:
        explicit AbstractShader(const char* const filePath)
            : m_FilePath(filePath)
        {
        }

        const char* m_FilePath;
    };
}

#endif //OTTERENGINE_ABSTRACTSHADER_H
