#version 450

layout(binding = 2) uniform sceneData
{
    vec4 ambientLight;
    vec4 sunlightDirection;
    vec4 sunlightColor;
} scene;

layout(binding = 3) uniform sampler2D texSampler;
layout(binding = 4) uniform sampler2D texBumpSampler;
layout(binding = 5) uniform sampler2D shadowSampler;

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec4 fragShadowTexCoord;

layout(location = 0) out vec4 outColor;

void main()
{
    vec3 coord = fragShadowTexCoord.xyz / fragShadowTexCoord.w;
    coord = coord * 0.5 + 0.5;

    float closestDepth = texture(shadowSampler, coord.xy).r;
    float currentDepth = fragShadowTexCoord.z; 
    // vec3 bump = texture(texBumpSampler, fragTexCoord).rgb;
    // vec3 bumpNorm = fragNormal + bump.xyz;
    // bumpNorm = normalize(bumpNorm);

    float lightIntensity = dot(normalize(scene.sunlightDirection).xyz, fragNormal);
    if (lightIntensity < 0.0f)
        lightIntensity = 0.0f;

    float bias = max(0.005, 0.05 * (1 - lightIntensity));
    if (currentDepth - closestDepth > bias)
        lightIntensity = 0.0f;


    // lightIntensity = 1.0f;

    lightIntensity += scene.ambientLight.x;
    outColor = texture(texSampler, fragTexCoord) * scene.sunlightColor * lightIntensity;
    // outColor = vec4(closestDepth, currentDepth, 0.0f, 1.0f);
}

