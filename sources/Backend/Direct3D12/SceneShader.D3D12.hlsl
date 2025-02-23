/*
 * LLGL Example ImGui
 * Created on 02/22/2025 by L.Hermanns
 * Published under the BSD-3 Clause License
 * ----------------------------------------
 * Direct3D 12 Scene Shader
 */

struct View
{
    float4x4 vpMatrix;
    float4x4 wMatrix;
    float4   modelColor;
    float4   lightVector;
};

ConstantBuffer<View> view : register(b1);

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
    outp.position   = mul(view.vpMatrix, mul(view.wMatrix, float4(inp.position, 1)));
    outp.normal     = normalize(mul((float3x3)view.wMatrix, inp.normal));
    outp.color      = view.modelColor * inp.color;
}

float4 PSMain(VertexOut inp) : SV_Target
{
    float4 color = inp.color;
    float3 normal = normalize(inp.normal);
    float NdotL = saturate(dot(normal, normalize(-view.lightVector.xyz)));
    return float4(color.rgb * NdotL, color.a);
}

