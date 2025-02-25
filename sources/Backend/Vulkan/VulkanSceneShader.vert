/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Vulkan Scene Vertex Shader
 */

#version 450 core

layout(binding = 1, std140) uniform View
{
    mat4 vpMatrix;
    mat4 wMatrix;
    vec4 modelColor;
    vec4 lightVector;
};

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 color;

layout(location = 0) out vec3 vNormal;
layout(location = 1) out vec4 vColor;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main()
{
    gl_Position = vpMatrix * (wMatrix * vec4(position, 1));
    vNormal     = normalize(mat3(wMatrix) * normal);
    vColor      = modelColor * color;
}
