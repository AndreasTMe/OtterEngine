#include "Otter.PCH.h"

#include "Graphics/Vulkan/VulkanShaders.h"

namespace Otter::Graphics::Vulkan
{
    bool LoadShaderModule(const char* const filePath, VkShaderModuleCreateInfo* outCreateInfo)
    {
        std::ifstream file(filePath, std::ios::ate | std::ios::binary);

        if (!file.is_open())
            return false;

        UInt32 fileSize = (UInt32) file.tellg();

        std::vector<UInt32> buffer(fileSize / sizeof(UInt32));

        file.seekg(0);
        file.read((char*) buffer.data(), fileSize);
        file.close();

        VkShaderModuleCreateInfo createInfo = { };
        createInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pNext    = VK_NULL_HANDLE;
        createInfo.codeSize = buffer.size() * sizeof(UInt32);
        createInfo.pCode    = buffer.data();

        *outCreateInfo = createInfo;
        return true;
    }
}