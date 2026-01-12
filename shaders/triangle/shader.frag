#version 450

layout(binding = 1) uniform sceneData
{
    vec4 ambientLight;
    vec4 sunlightDirection;
    vec4 sunlightColor;
} scene;

layout(binding = 2) uniform sampler2D texSampler;
layout(binding = 3) uniform sampler2D texBumpSampler;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    // vec3 bump = texture(texBumpSampler, fragTexCoord).rgb;
    // vec3 bumpNorm = fragNormal + bump.xyz;
    // bumpNorm = normalize(bumpNorm);
    float lightIntensity = dot(scene.sunlightDirection.xyz, fragNormal);
    if (lightIntensity < 0.0f)
        lightIntensity = 0.0f;
    lightIntensity += scene.ambientLight.x;
    outColor = texture(texSampler, fragTexCoord) * scene.sunlightColor * lightIntensity;
}

