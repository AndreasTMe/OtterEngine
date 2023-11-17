#ifndef OTTERENGINE_VULKANPIPELINES_H
#define OTTERENGINE_VULKANPIPELINES_H

#include "Core/Collections/Enumerable.h"
#include "Graphics/API/Vulkan/VulkanBase.Includes.h"

namespace Otter::Graphics::Vulkan
{
    void CreatePipeline(const VkDevice& logicalDevice,
                        const VkRenderPass& renderPass,
                        const VkAllocationCallbacks* allocator,
                        const Enumerable <VkPipelineShaderStageCreateInfo>& shaderStages,
                        const Enumerable <VkDescriptorSetLayout>& descriptorSetLayouts,
                        const Enumerable <VkPushConstantRange>& pushConstantRanges,
                        const VkExtent2D& extent,
                        VkPipelineLayout* outPipelineLayout,
                        VkPipeline* outPipeline);
    void DestroyPipeline(const VkDevice& logicalDevice,
                         const VkAllocationCallbacks* allocator,
                         VkPipelineLayout* outPipelineLayout,
                         VkPipeline* outPipeline);
}

#endif //OTTERENGINE_VULKANPIPELINES_H
