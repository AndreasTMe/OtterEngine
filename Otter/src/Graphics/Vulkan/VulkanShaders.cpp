#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanShaders.h"
#include "Platform/FileSystem.h"

namespace Otter::Graphics::Vulkan
{
    bool TryCreateShaderModule(const VkDevice& logicalDevice,
                               const VkAllocationCallbacks* allocator,
                               const char* filePath,
                               VkShaderModule* outShaderModule)
    {
        OTR_INTERNAL_ASSERT_MSG(logicalDevice != VK_NULL_HANDLE, "Logical device is null")
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

        OTR_VULKAN_VALIDATE(vkCreateShaderModule(logicalDevice, &createInfo, allocator, outShaderModule))

        Buffer::Delete(buffer, fileSize);
        FileSystem::CloseFile(&file);

        return true;
    }

    void DestroyShaderModule(const VkDevice& logicalDevice,
                             const VkAllocationCallbacks* allocator,
                             VkShaderModule& shaderModule)
    {
        OTR_INTERNAL_ASSERT_MSG(logicalDevice != VK_NULL_HANDLE, "Logical device is null")
        OTR_INTERNAL_ASSERT_MSG(shaderModule != VK_NULL_HANDLE, "Shader module handle is null")

        vkDestroyShaderModule(logicalDevice, shaderModule, allocator);
    }
}