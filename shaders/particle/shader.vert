#version 450

layout(binding = 0) uniform uniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 projViewModel;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 2) in vec4 inColor;

layout(location = 0) out vec4 outColor;

void main()
{
    outColor = inColor;
    gl_Position = ubo.projViewModel * vec4(inPosition, 1.0);
    gl_PointSize = 1.0f;
}