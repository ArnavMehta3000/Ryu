#define RootSig \
	"RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
	"CBV(b0, space=0, visibility=SHADER_VISIBILITY_VERTEX)"

cbuffer cbPerObject : register(b0)
{
    float gOffsetX;
    float gOffsetY;
}

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

[RootSignature(RootSig)]
PSInput VSMain(float3 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = float4(position, 1.0f);
    result.position.xy += float2(gOffsetX, gOffsetY);

    result.color = color;

    return result;
}

[RootSignature(RootSig)]float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}
