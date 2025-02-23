/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Metal Scene Shader
 */

#include <metal_stdlib>

using namespace metal;

struct View
{
    float4x4 vpMatrix;
    float4x4 wMatrix;
    float4   modelColor;
    float4   lightVector;
}

struct VertexIn
{
    float3 position [[attribute(0)]];
    float3 normal   [[attribute(1)]];
    float4 color    [[attribute(2)]];
};

struct VertexOut
{
    float4 position [[position]];
    float3 normal;
    float4 color;
};

vertex VertexOut VSMain(
    VertexIn       inp  [[stage_in]],
    constant View& view [[buffer(1)]])
{
    VertexOut outp;
    outp.position   = view.vpMatrix * (view.wMatrix * float4(inp.position, 1));
    outp.normal     = normalize((float3x3)view.wMatrix * inp.normal);
    outp.color      = view.modelColor * inp.color;
    return outp;
}

fragment float4 PSMain(
    VertexOut      inp  [[stage_in]],
    constant View& view [[buffer(1)]])
{
    float4 color = inp.color;
    float3 normal = normalize(inp.normal);
    float NdotL = saturate(dot(normal, normalize(-view.lightVector.xyz)));
    return float4(color.rgb * NdotL, color.a);
}

