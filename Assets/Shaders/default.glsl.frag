#version 450

layout(binding = 1) uniform sampler2D textureSampler;

layout (location = 0) in vec4 inColor;
layout (location = 1) in vec2 inTexCoord;

layout (location = 0) out vec4 outColor;

void main()
{
    outColor = vec4(inColor.rgb * texture(textureSampler, inTexCoord * 2.0).rgb, 1.0);
}