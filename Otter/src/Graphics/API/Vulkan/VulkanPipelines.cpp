#include "Otter.PCH.h"

#include "Graphics/API/Vulkan/VulkanPipelines.h"
#include "Graphics/API/Vulkan/VulkanPoint.h"

namespace Otter::Graphics::Vulkan
{
    void CreatePipeline(const VkDevice& logicalDevice,
                        const VkRenderPass& renderPass,
                        const VkAllocationCallbacks* const allocator,
                        const Enumerable <VkPipelineShaderStageCreateInfo>& shaderStages,
                        const Enumerable <VkDescriptorSetLayout>& descriptorSetLayouts,
                        const Enumerable <VkPushConstantRange>& pushConstantRanges,
                        const VkExtent2D& extent,
                        VkPipelineLayout* outPipelineLayout,
                        VkPipeline* outPipeline)
    {
        OTR_INTERNAL_ASSERT_MSG(logicalDevice != VK_NULL_HANDLE, "Logical device is null")
        OTR_INTERNAL_ASSERT_MSG(renderPass != VK_NULL_HANDLE, "Render pass is null")
        OTR_INTERNAL_ASSERT_MSG(shaderStages.GetCount() > 0, "Shader stages is empty")
        OTR_INTERNAL_ASSERT_MSG(extent.width > 0 && extent.height > 0, "Extent is invalid")

        // Vertex Input
        const auto bindingDescriptions   = VulkanPoint::GetBindingDescriptions();
        const auto attributeDescriptions = VulkanPoint::GetAttributeDescriptions();

        VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{ };
        vertexInputStateCreateInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputStateCreateInfo.vertexBindingDescriptionCount   = bindingDescriptions.Length();
        vertexInputStateCreateInfo.pVertexBindingDescriptions      = bindingDescriptions.GetData();
        vertexInputStateCreateInfo.vertexAttributeDescriptionCount = attributeDescriptions.Length();
        vertexInputStateCreateInfo.pVertexAttributeDescriptions    = attributeDescriptions.GetData();
        vertexInputStateCreateInfo.pNext                           = VK_NULL_HANDLE;

        // Input Assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{ };
        inputAssemblyStateCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyStateCreateInfo.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
        inputAssemblyStateCreateInfo.pNext                  = VK_NULL_HANDLE;

        // Tessellation
        VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo{ };
        tessellationStateCreateInfo.sType              = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        tessellationStateCreateInfo.patchControlPoints = 0;
        tessellationStateCreateInfo.pNext              = VK_NULL_HANDLE;

        // Viewport and Scissor
        VkViewport viewport{ };
        viewport.x        = 0.0f;
        viewport.y        = 0.0f;
        viewport.width    = static_cast<Float32>(extent.width);
        viewport.height   = static_cast<Float32>(extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{ };
        scissor.offset = { 0, 0 };
        scissor.extent = extent;

        VkPipelineViewportStateCreateInfo viewportStateCreateInfo{ };
        viewportStateCreateInfo.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportStateCreateInfo.viewportCount = 1;
        viewportStateCreateInfo.pViewports    = &viewport;
        viewportStateCreateInfo.scissorCount  = 1;
        viewportStateCreateInfo.pScissors     = &scissor;
        viewportStateCreateInfo.pNext         = VK_NULL_HANDLE;

        // Rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{ };
        rasterizationStateCreateInfo.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizationStateCreateInfo.depthClampEnable        = VK_FALSE;
        rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizationStateCreateInfo.polygonMode             = VK_POLYGON_MODE_FILL;
        rasterizationStateCreateInfo.lineWidth               = 1.0f;
        rasterizationStateCreateInfo.cullMode                = VK_CULL_MODE_NONE;
        rasterizationStateCreateInfo.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizationStateCreateInfo.depthBiasEnable         = VK_FALSE;
        rasterizationStateCreateInfo.depthBiasConstantFactor = 0.0f;
        rasterizationStateCreateInfo.depthBiasClamp          = 0.0f;
        rasterizationStateCreateInfo.depthBiasSlopeFactor    = 0.0f;
        rasterizationStateCreateInfo.pNext                   = VK_NULL_HANDLE;

        // Multisampling
        VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{ };
        multisampleStateCreateInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampleStateCreateInfo.sampleShadingEnable   = VK_FALSE;
        multisampleStateCreateInfo.rasterizationSamples  = VK_SAMPLE_COUNT_1_BIT;
        multisampleStateCreateInfo.minSampleShading      = 1.0f;
        multisampleStateCreateInfo.pSampleMask           = VK_NULL_HANDLE;
        multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
        multisampleStateCreateInfo.alphaToOneEnable      = VK_FALSE;
        multisampleStateCreateInfo.pNext                 = VK_NULL_HANDLE;

        // Depth and Stencil testing
        VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo{ };
        depthStencilStateCreateInfo.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilStateCreateInfo.depthTestEnable       = VK_TRUE;
        depthStencilStateCreateInfo.depthWriteEnable      = VK_TRUE;
        depthStencilStateCreateInfo.depthCompareOp        = VK_COMPARE_OP_LESS;
        depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilStateCreateInfo.stencilTestEnable     = VK_FALSE;
        depthStencilStateCreateInfo.front                 = { };
        depthStencilStateCreateInfo.back                  = { };
        depthStencilStateCreateInfo.minDepthBounds        = 0.0f;
        depthStencilStateCreateInfo.maxDepthBounds        = 1.0f;
        depthStencilStateCreateInfo.pNext                 = VK_NULL_HANDLE;

        // Color blending
        VkPipelineColorBlendAttachmentState colorBlendAttachmentState{ };
        colorBlendAttachmentState.colorWriteMask      = VK_COLOR_COMPONENT_R_BIT
                                                        | VK_COLOR_COMPONENT_G_BIT
                                                        | VK_COLOR_COMPONENT_B_BIT
                                                        | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachmentState.blendEnable         = VK_TRUE;
        colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentState.colorBlendOp        = VK_BLEND_OP_ADD;
        colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachmentState.alphaBlendOp        = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{ };
        colorBlendStateCreateInfo.sType           = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendStateCreateInfo.logicOpEnable   = VK_FALSE;
        colorBlendStateCreateInfo.logicOp         = VK_LOGIC_OP_COPY;
        colorBlendStateCreateInfo.attachmentCount = 1;
        colorBlendStateCreateInfo.pAttachments    = &colorBlendAttachmentState;
        colorBlendStateCreateInfo.blendConstants[0] = 0.0f;
        colorBlendStateCreateInfo.blendConstants[1] = 0.0f;
        colorBlendStateCreateInfo.blendConstants[2] = 0.0f;
        colorBlendStateCreateInfo.blendConstants[3] = 0.0f;
        colorBlendStateCreateInfo.pNext = VK_NULL_HANDLE;

        // Dynamic state
        const UInt8    dynamicStatesCount                = 2;
        VkDynamicState dynamicStates[dynamicStatesCount] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR
        };

        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{ };
        dynamicStateCreateInfo.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.dynamicStateCount = dynamicStatesCount;
        dynamicStateCreateInfo.pDynamicStates    = dynamicStates;
        dynamicStateCreateInfo.pNext             = VK_NULL_HANDLE;

        // Pipeline Layout
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{ };
        pipelineLayoutCreateInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.flags                  = 0;
        pipelineLayoutCreateInfo.setLayoutCount         = descriptorSetLayouts.GetCount();
        pipelineLayoutCreateInfo.pSetLayouts            = descriptorSetLayouts.GetData();
        pipelineLayoutCreateInfo.pushConstantRangeCount = pushConstantRanges.GetCount();
        pipelineLayoutCreateInfo.pPushConstantRanges    = pushConstantRanges.GetData();
        pipelineLayoutCreateInfo.pNext                  = VK_NULL_HANDLE;

        OTR_VULKAN_VALIDATE(vkCreatePipelineLayout(logicalDevice,
                                                   &pipelineLayoutCreateInfo,
                                                   allocator,
                                                   outPipelineLayout))

        // Graphics Pipeline
        VkGraphicsPipelineCreateInfo pipelineCreateInfo{ };
        pipelineCreateInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.flags               = 0;
        pipelineCreateInfo.stageCount          = shaderStages.GetCount();
        pipelineCreateInfo.pStages             = shaderStages.GetData();
        pipelineCreateInfo.pVertexInputState   = &vertexInputStateCreateInfo;
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
        pipelineCreateInfo.pTessellationState  = &tessellationStateCreateInfo;
        pipelineCreateInfo.pViewportState      = &viewportStateCreateInfo;
        pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
        pipelineCreateInfo.pMultisampleState   = &multisampleStateCreateInfo;
        pipelineCreateInfo.pDepthStencilState  = &depthStencilStateCreateInfo;
        pipelineCreateInfo.pColorBlendState    = &colorBlendStateCreateInfo;
        pipelineCreateInfo.pDynamicState       = &dynamicStateCreateInfo;
        pipelineCreateInfo.layout              = *outPipelineLayout;
        pipelineCreateInfo.renderPass          = renderPass;
        pipelineCreateInfo.subpass             = 0;
        pipelineCreateInfo.basePipelineHandle  = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex   = -1;
        pipelineCreateInfo.pNext               = VK_NULL_HANDLE;

        OTR_VULKAN_VALIDATE(vkCreateGraphicsPipelines(logicalDevice,
                                                      VK_NULL_HANDLE,
                                                      1,
                                                      &pipelineCreateInfo,
                                                      allocator,
                                                      outPipeline))
    }

    void DestroyPipeline(const VkDevice& logicalDevice,
                         const VkAllocationCallbacks* const allocator,
                         VkPipelineLayout* outPipelineLayout,
                         VkPipeline* outPipeline)
    {
        OTR_INTERNAL_ASSERT_MSG(logicalDevice != VK_NULL_HANDLE, "Logical device is null")
        OTR_INTERNAL_ASSERT_MSG(outPipelineLayout != VK_NULL_HANDLE, "Pipeline layout is null")
        OTR_INTERNAL_ASSERT_MSG(outPipeline != VK_NULL_HANDLE, "Pipeline is null")

        vkDestroyPipelineLayout(logicalDevice, *outPipelineLayout, allocator);
        *outPipelineLayout = VK_NULL_HANDLE;

        vkDestroyPipeline(logicalDevice, *outPipeline, allocator);
        *outPipeline = VK_NULL_HANDLE;
    }
}