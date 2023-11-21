#include "Otter.PCH.h"

#include "Graphics/API/Vulkan/VulkanShader.h"
#include "Platform/FileSystem.h"

#if OTR_GRAPHICS_VULKAN_ENABLED

namespace Otter::Graphics
{
    Shader* Shader::Create(const char* const filePath)
    {
        return New<Vulkan::VulkanShader>(filePath);
    }

    void Shader::Destroy(Shader* outShader)
    {
        Delete<Vulkan::VulkanShader>((Vulkan::VulkanShader*) outShader);
        outShader = nullptr;
    }
}

#endif

namespace Otter::Graphics::Vulkan
{
    VulkanShader::~VulkanShader()
    {
        OTR_INTERNAL_ASSERT_MSG(m_LogicalDevice != VK_NULL_HANDLE, "Logical device is null")

        if (m_VertexModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(m_LogicalDevice, m_VertexModule, m_Allocator);
            m_VertexModule = VK_NULL_HANDLE;
        }
        if (m_FragmentModule != VK_NULL_HANDLE)
        {
            vkDestroyShaderModule(m_LogicalDevice, m_FragmentModule, m_Allocator);
            m_FragmentModule = VK_NULL_HANDLE;
        }

        m_LogicalDevice = VK_NULL_HANDLE;
        m_Allocator     = VK_NULL_HANDLE;
    }

    void VulkanShader::Bind()
    {
        OTR_INTERNAL_ASSERT_MSG(m_LogicalDevice != VK_NULL_HANDLE, "Logical device is null")
        OTR_INTERNAL_ASSERT_MSG(m_FilePath != nullptr, "File path is empty")

        auto vertexPath = std::string(m_FilePath) + ".vert.spv";
        auto fragPath   = std::string(m_FilePath) + ".frag.spv";

        if (!TryLoadShaderModule(vertexPath.c_str(), &m_VertexModule))
        {
            OTR_LOG_FATAL("Failed to load vertex shader module from file: {0}", vertexPath.c_str())
        }

        if (!TryLoadShaderModule(fragPath.c_str(), &m_FragmentModule))
        {
            OTR_LOG_FATAL("Failed to load fragment shader module from file: {0}", fragPath.c_str())
        }
    }

    bool VulkanShader::TryLoadShaderModule(const char* const filePath, VkShaderModule* outShaderModule)
    {
        OTR_INTERNAL_ASSERT_MSG(m_LogicalDevice != VK_NULL_HANDLE, "Logical device is null")
        OTR_INTERNAL_ASSERT_MSG(filePath != nullptr, "File path is null")

        FileSystem::File file;
        if (!FileSystem::FileExists(filePath)
            || !FileSystem::TryOpenFile(filePath, FileSystem::OpenMode::Read, true, &file))
            return false;

        Size fileSize = 0;
        if (!FileSystem::TryGetFileSize(&file, &fileSize))
        {
            FileSystem::CloseFile(&file);
            return false;
        }

        auto* buffer = Buffer::New<UInt32>(fileSize / sizeof(UInt32));
        if (!FileSystem::TryReadAllBytes(&file, buffer, &fileSize))
        {
            Buffer::Delete(buffer, fileSize);
            FileSystem::CloseFile(&file);
            return false;
        }

        VkShaderModuleCreateInfo createInfo = { };
        createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pNext    = VK_NULL_HANDLE;
        createInfo.codeSize = fileSize;
        createInfo.pCode    = buffer;

        OTR_VULKAN_VALIDATE(vkCreateShaderModule(m_LogicalDevice, &createInfo, m_Allocator, outShaderModule))

        Buffer::Delete(buffer, fileSize);
        FileSystem::CloseFile(&file);

        return true;
    }
}

