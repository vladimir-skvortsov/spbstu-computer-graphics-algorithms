Texture2D diffuseMap : register(t0);
Texture2D normalMap : register(t1);
SamplerState samLinear : register(s0);

cbuffer LightBuffer : register(b0) {
    float3 light0Pos;
    float pad0;
    float3 light0Color;
    float pad1;
    float3 light1Pos;
    float pad2;
    float3 light1Color;
    float pad3;
};

struct PSInput {
    float4 Pos : SV_POSITION;
    float3 WorldPos : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float2 TexCoord : TEXCOORD2;
    uint TexIndex : TEXCOORD3;
};

float3 ComputeTangent(float3 n) {
    return (abs(n.y) > 0.99) ? float3(1, 0, 0) : normalize(cross(float3(0, 1, 0), n));
}

float4 main(PSInput input) : SV_Target {
    float4 diffuseColor = diffuseMap.Sample(samLinear, input.TexCoord);

    float3 normalSample = normalMap.Sample(samLinear, input.TexCoord).rgb * 2.0 - 1.0;

    float3 tangent = ComputeTangent(normalize(input.Normal));
    float3 bitangent = normalize(cross(normalize(input.Normal), tangent));
    float3x3 TBN = float3x3(tangent, bitangent, normalize(input.Normal));

    float3 perturbedNormal = normalize(mul(normalSample, TBN));

    float3 lightDir0 = normalize(light0Pos - input.WorldPos);
    float diff0 = saturate(dot(perturbedNormal, lightDir0));

    float3 lightDir1 = normalize(light1Pos - input.WorldPos);
    float diff1 = saturate(dot(perturbedNormal, lightDir1));

    float3 color = 0;
    if (diff0 > 0)
        color += diffuseColor.rgb * light0Color * diff0;
    if (diff1 > 0)
        color += diffuseColor.rgb * light1Color * diff1;

    return float4(color, diffuseColor.a);
}