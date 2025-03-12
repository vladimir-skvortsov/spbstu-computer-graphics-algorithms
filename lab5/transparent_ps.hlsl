cbuffer TransparentBuffer : register(b0) {
    float4 g_Color;
};

float4 main() : SV_Target {
    return g_Color;
}