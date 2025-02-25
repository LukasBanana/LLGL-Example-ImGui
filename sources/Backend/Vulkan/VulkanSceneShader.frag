/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Vulkan Scene Fragment Shader
 */

#version 450 core

layout(binding = 1, std140) uniform View
{
    mat4 vpMatrix;
    mat4 wMatrix;
    vec4 modelColor;
    vec4 lightVector;
};

layout(location = 0) in vec3 vNormal;
layout(location = 1) in vec4 vColor;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 color = vColor;
    vec3 normal = normalize(vNormal);
    float NdotL = max(0.0, dot(normal, normalize(-lightVector.xyz)));
    outColor = vec4(color.rgb * NdotL, color.a);
}

