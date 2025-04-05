struct FSInput {
    float4 Pos : POSITION;
    float2 Tex : TEXCOORD0;
};

struct VSOutput {
    float4 Pos : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
};

VSOutput main(FSInput input) {
    VSOutput output;
    output.Pos = input.Pos;
    output.TexCoord = input.Tex;
    return output;
}