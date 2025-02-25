/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Direct3D 11 Scene Shader
 */

cbuffer View : register(b1)
{
    float4x4 vpMatrix;
    float4x4 wMatrix;
    float4   modelColor;
    float4   lightVector;
}

struct VertexIn
{
    float3 position : POSITION;
    float3 normal   : NORMAL;
    float4 color    : COLOR;
};

struct VertexOut
{
    float4 position : SV_Position;
    float3 normal   : NORMAL;
    float4 color    : COLOR;
};

void VSMain(VertexIn inp, out VertexOut outp)
{
    outp.position   = mul(vpMatrix, mul(wMatrix, float4(inp.position, 1)));
    outp.normal     = normalize(mul((float3x3)wMatrix, inp.normal));
    outp.color      = modelColor * inp.color;
}

float4 PSMain(VertexOut inp) : SV_Target
{
    float4 color = inp.color;
    float3 normal = normalize(inp.normal);
    float NdotL = saturate(dot(normal, normalize(-lightVector.xyz)));
    return float4(color.rgb * NdotL, color.a);
}

