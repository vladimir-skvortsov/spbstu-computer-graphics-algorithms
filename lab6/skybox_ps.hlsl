TextureCube skyboxTexture : register(t0);
SamplerState samLinear : register(s0);

struct PS_INPUT {
    float4 position : SV_POSITION;
    float3 texcoord : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_Target {
    return skyboxTexture.Sample(samLinear, input.texcoord);
}