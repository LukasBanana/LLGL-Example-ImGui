/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * OpenGL Scene Fragment Shader
 */

#version 330 core

layout(std140) uniform View
{
    mat4 vpMatrix;
    mat4 wMatrix;
    vec4 modelColor;
    vec4 lightVector;
};

in vec3 vNormal;
in vec4 vColor;

out vec4 outColor;

void main()
{
    vec4 color = vColor;
    vec3 normal = normalize(vNormal);
    float NdotL = max(0.0, dot(normal, normalize(-lightVector.xyz)));
    outColor = vec4(color.rgb * NdotL, color.a);
}

