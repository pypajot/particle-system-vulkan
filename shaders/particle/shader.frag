#version 450

// layout(binding = 1) uniform sceneData
// {
//     vec4 ambientLight;
//     vec4 sunlightDirection;
//     vec4 sunlightColor;
// } scene;

layout(location = 0) in vec4 inColor;

layout(location = 0) out vec4 outColor;

void main()
{
    // vec3 bump = texture(texBumpSampler, fragTexCoord).rgb;
    // vec3 bumpNorm = fragNormal + bump.xyz;
    // bumpNorm = normalize(bumpNorm);
    // float lightIntensity = dot(scene.sunlightDirection.xyz, fragNormal);
    // if (lightIntensity < 0.0f)
    //     lightIntensity = 0.0f;
    // lightIntensity += scene.ambientLight.x;
    outColor = inColor;
    // outColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}

