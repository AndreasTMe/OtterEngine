#version 450

layout(set = 0, binding = 0) uniform GlobalUniformBufferObject {
    mat4 Projection;
    mat4 View;
} globalUBO;

layout(push_constant) uniform PushConstant {
    mat4 model;
} globalPC;

layout (location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec2 outTexCoord;

void main()
{
    gl_Position = globalUBO.Projection * globalUBO.View * globalPC.model * vec4(inPosition, 1.0);
    outColor = inColor;
    outTexCoord = inTexCoord;
}