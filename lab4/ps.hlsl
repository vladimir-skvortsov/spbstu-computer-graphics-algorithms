Texture2D myTexture : register(t0);
SamplerState samLinear : register(s0);

struct PSInput {
    float4 pos : SV_Position;
    float2 texcoord : TEXCOORD;
};

float4 main(PSInput input) : SV_Target {
    return myTexture.Sample(samLinear, input.texcoord);
}