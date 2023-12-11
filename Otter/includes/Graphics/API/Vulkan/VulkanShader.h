#ifndef OTTERENGINE_VULKANSHADER_H
#define OTTERENGINE_VULKANSHADER_H

#include "Graphics/Abstractions/Shader.h"
#include "Graphics/API/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
    class VulkanShader final : public Shader
    {
    public:
        ~VulkanShader() final;

        explicit VulkanShader(const char* const filePath)
            : Shader(filePath)
        {
        }

        void Bind() final;

        OTR_INLINE void SetLogicalDevice(const VkDevice& logicalDevice) { m_LogicalDevice = logicalDevice; }
        OTR_INLINE void SetAllocator(VkAllocationCallbacks* const allocator) { m_Allocator = allocator; }

        [[nodiscard]] OTR_INLINE VkShaderModule GetVertexModule() const { return m_VertexModule; }
        [[nodiscard]] OTR_INLINE VkShaderModule GetFragmentModule() const { return m_FragmentModule; }

    private:
        VkShaderModule m_VertexModule   = VK_NULL_HANDLE;
        VkShaderModule m_FragmentModule = VK_NULL_HANDLE;

        VkDevice m_LogicalDevice = VK_NULL_HANDLE;
        VkAllocationCallbacks* m_Allocator = VK_NULL_HANDLE;

        bool TryLoadShaderModule(const char* filePath, VkShaderModule* outShaderModule);
    };
}

#endif //OTTERENGINE_VULKANSHADER_H
