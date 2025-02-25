/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * OpenGL Scene Vertex Shader
 */

#version 330 core

layout(std140) uniform View
{
    mat4 vpMatrix;
    mat4 wMatrix;
    vec4 modelColor;
    vec4 lightVector;
};

in vec3 position;
in vec3 normal;
in vec4 color;

out vec3 vNormal;
out vec4 vColor;

void main()
{
    gl_Position = vpMatrix * (wMatrix * vec4(position, 1));
    vNormal     = normalize(mat3(wMatrix) * normal);
    vColor      = modelColor * color;
}
