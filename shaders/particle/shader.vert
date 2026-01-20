#version 450

layout(binding = 0) uniform uniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 projViewModel;
} ubo;

layout(binding = 1) uniform uniformBufferObjectLight
{
    mat4 model;
    mat4 view;
    mat4 proj;
    mat4 projViewModel;
} uboLight;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inVelocity;
layout(location = 2) in vec3 inColor;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec4 fragShadowTexCoord;


void main()
{
    outColor = vec4(inColor, 1.0);
    gl_Position = ubo.projViewModel * vec4(inPosition, 1.0);
    gl_PointSize = 1.0;
    fragShadowTexCoord = uboLight.projViewModel * vec4(inPosition, 1.0);
}