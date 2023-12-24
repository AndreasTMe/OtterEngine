#ifndef OTTERENGINE_DATABUFFER_H
#define OTTERENGINE_DATABUFFER_H

#include "Core/Collections/List.h"
#include "Graphics/Abstractions/ShaderAttribute.h"
#include "Graphics/Common/Types.BufferType.h"

namespace Otter::Graphics
{
    /**
     * @brief A buffer of data that can be bound to the GPU.
     */
    class DataBuffer
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~DataBuffer() = default;

        /**
         * @brief Writes data to the buffer.
         *
         * @param data The data to write.
         * @param size The size of the data to write.
         */
        virtual void Write(const void* data, UInt32 size) = 0;

        /**
         * @brief Binds the buffer to the GPU.
         */
        virtual void Bind() const = 0;

    protected:
        /**
         * @brief Constructor.
         */
        DataBuffer() = default;
    };

    /**
     * @brief A buffer of data that can be bound to the GPU as a vertex buffer.
     */
    class VertexBuffer : public DataBuffer
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        ~VertexBuffer() override
        {
            m_AttributeLayout.ClearDestructive();
        }

        /**
         * @brief Gets the attribute layout of the vertex buffer.
         *
         * @return The attribute layout of the vertex buffer.
         */
        [[nodiscard]] OTR_INLINE const List<ShaderAttribute>& GetAttributeLayout() const { return m_AttributeLayout; }

        /**
         * @brief Sets the attribute layout of the vertex buffer.
         *
         * @param attributes The attribute layout of the vertex buffer.
         */
        OTR_INLINE void SetAttributeLayout(InitialiserList<ShaderAttribute> attributes)
        {
            m_AttributeLayout.ClearDestructive();
            m_AttributeLayout.Reserve(attributes.size());
            m_AttributeLayout.TryAddRange(attributes);
        }

    protected:
        /**
         * @brief Constructor.
         */
        VertexBuffer() : DataBuffer(), m_AttributeLayout() { }

        List<ShaderAttribute> m_AttributeLayout;
    };

    /**
     * @brief A buffer of data that can be bound to the GPU as an index buffer.
     */
    class IndexBuffer : public DataBuffer
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        ~IndexBuffer() override = default;

        /**
         * @brief Gets the number of indices in the index buffer.
         *
         * @return The number of indices in the index buffer.
         */
        [[nodiscard]] OTR_INLINE UInt32 GetCount() const { return m_Count; }

    protected:
        /**
         * @brief Constructor.
         */
        IndexBuffer() : DataBuffer(), m_Count(0) { }

        UInt32 m_Count;
    };

    /**
     * @brief A buffer of data that can be bound to the GPU as a uniform buffer.
     */
    class UniformBuffer : public DataBuffer
    {
    public:
        /**
         * @brief Virtual destructor.
         */
        ~UniformBuffer() override = default;

        /**
         * @brief Overwrites a section of the uniform buffer.
         *
         * @param data The data to overwrite with.
         * @param size The size of the data to overwrite with.
         * @param offset The offset to overwrite at.
         */
        virtual void Overwrite(const void* data, UInt32 size, UInt32 offset) = 0;

    protected:
        /**
         * @brief Constructor.
         */
        UniformBuffer() : DataBuffer() { }
    };
}

#endif //OTTERENGINE_DATABUFFER_H
