#ifndef OTTERENGINE_DATABUFFER_H
#define OTTERENGINE_DATABUFFER_H

#include "Core/Types.h"
#include "Core/Collections/List.h"
#include "Graphics/Common/Types.BufferType.h"
#include "Graphics/Common/Types.ShaderAttributeType.h"

namespace Otter::Graphics
{
    class DataBuffer
    {
    public:
        virtual ~DataBuffer() = default;

        virtual void Write(const void* data, UInt32 size) = 0;
        virtual void Bind() const = 0;

        [[nodiscard]] static DataBuffer* Create(BufferType type);
        static void Destroy(BufferType type, DataBuffer* outBuffer);

    protected:
        DataBuffer() = default;
    };

    class VertexBuffer : public DataBuffer
    {
    public:
        ~VertexBuffer() override
        {
            m_AttributeLayout.ClearDestructive();
        }

        [[nodiscard]] const List <ShaderAttributeType>& GetAttributeLayout() const { return m_AttributeLayout; }
        void SetAttributeLayout(InitialiserList<ShaderAttributeType> attributes) { m_AttributeLayout = attributes; }

    protected:
        VertexBuffer() : DataBuffer(), m_AttributeLayout() { }

        List <ShaderAttributeType> m_AttributeLayout;
    };

    class IndexBuffer : public DataBuffer
    {
    public:
        ~IndexBuffer() override = default;

        [[nodiscard]] UInt32 GetCount() const { return m_Count; }

    protected:
        IndexBuffer() : DataBuffer(), m_Count(0) { }

        UInt32 m_Count;
    };

    class UniformBuffer : public DataBuffer
    {
    public:
        ~UniformBuffer() override = default;

        virtual void Overwrite(const void* data, UInt32 size, UInt32 offset) = 0;

    protected:
        UniformBuffer() : DataBuffer() { }
    };
}

#endif //OTTERENGINE_DATABUFFER_H
