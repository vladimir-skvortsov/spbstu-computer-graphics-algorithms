cbuffer LightColorBuffer : register(b0) {
    float3 lightColor;
    float pad;
};

struct PS_INPUT {
    float4 Pos : SV_POSITION;
};

float4 main(PS_INPUT input) : SV_Target {
    return float4(lightColor, 1.0);
}