#ifndef OTTERENGINE_DATABUFFER_H
#define OTTERENGINE_DATABUFFER_H

#include "Core/Defines.h"
#include "Core/Types.h"
#include "Core/Collections/List.h"
#include "Graphics/Abstractions/ShaderAttribute.h"
#include "Graphics/Common/Types.BufferType.h"

namespace Otter::Graphics
{
    class DataBuffer
    {
    public:
        virtual ~DataBuffer() = default;

        virtual void Write(const void* data, UInt32 size) = 0;
        virtual void Bind() const = 0;

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

        [[nodiscard]] OTR_INLINE const List <ShaderAttribute>& GetAttributeLayout() const { return m_AttributeLayout; }
        OTR_INLINE void SetAttributeLayout(InitialiserList<ShaderAttribute> attributes)
        {
            m_AttributeLayout.ClearDestructive();
            m_AttributeLayout.Reserve(attributes.size());
            m_AttributeLayout.TryAddRange(attributes);
        }

    protected:
        VertexBuffer() : DataBuffer(), m_AttributeLayout() { }

        List <ShaderAttribute> m_AttributeLayout;
    };

    class IndexBuffer : public DataBuffer
    {
    public:
        ~IndexBuffer() override = default;

        [[nodiscard]] OTR_INLINE UInt32 GetCount() const { return m_Count; }

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
