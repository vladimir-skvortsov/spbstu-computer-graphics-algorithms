struct Light {
    float3 Position;
    float3 Color;
};

cbuffer LightBuffer : register(b0) {
    Light light0;
    Light light1;
    float3 ambient;
};

cbuffer BaseColorBuffer : register(b1) {
    float4 baseColor;
};

struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float3 Normal : NORMAL;
    float2 Tex : TEXCOORD;
    float3 WorldPos : WORLDPOS;
};

float4 main(PS_INPUT input) : SV_Target {
    float3 normal = normalize(input.Normal);

    float3 toLight0 = light0.Position - input.WorldPos;
    float dist0 = length(toLight0);
    float3 lightDir0 = normalize(toLight0);
    float range0 = 2.0;
    float att0 = saturate(1.0 - (dist0 / range0) * (dist0 / range0));
    float diff0 = max(dot(normal, lightDir0), 0.0) * att0;

    float3 toLight1 = light1.Position - input.WorldPos;
    float dist1 = length(toLight1);
    float3 lightDir1 = normalize(toLight1);
    float range1 = 2.0;
    float att1 = saturate(1.0 - (dist1 / range1) * (dist1 / range1));
    float diff1 = max(dot(normal, lightDir1), 0.0) * att1;

    float3 diffuse = light0.Color * diff0 + light1.Color * diff1;

    float3 finalColor = baseColor.rgb * diffuse;
    return float4(finalColor, baseColor.a);
}