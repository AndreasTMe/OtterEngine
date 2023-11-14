#version 450

layout (location = 0) in vec3 inPosition;

layout (location = 0) out vec4 outFragColor;

void main()
{
    outFragColor = vec4(inPosition, 1.0f);
}