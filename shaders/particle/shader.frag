#version 450

layout(binding = 2) uniform sceneData
{
    vec4 ambientLight;
    vec4 sunlightDirection;
    vec4 sunlightColor;
} scene;

layout(binding = 5) uniform sampler2D shadowSampler;

layout(location = 0) in vec4 inColor;
layout(location = 1) in vec4 fragShadowTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 coord = fragShadowTexCoord.xyz / fragShadowTexCoord.w;
    coord = coord * 0.5 + 0.5;

    float closestDepth = texture(shadowSampler, coord.xy).r;
    float currentDepth = fragShadowTexCoord.z;
    float light = 1.0f;
    float bias = 0.05;
    if (currentDepth - closestDepth > bias)
        light = 0.0f;
    // vec3 bump = texture(texBumpSampler, fragTexCoord).rgb;
    // vec3 bumpNorm = fragNormal + bump.xyz;
    // bumpNorm = normalize(bumpNorm);
    // float lightIntensity = dot(scene.sunlightDirection.xyz, fragNormal);
    // if (lightIntensity < 0.0f)
    //     lightIntensity = 0.0f;
    // lightIntensity += scene.ambientLight.x;
    outColor = inColor * (light + scene.ambientLight);
    // outColor = vec4(closestDepth, currentDepth, 0.0f, 1.0f);
}

