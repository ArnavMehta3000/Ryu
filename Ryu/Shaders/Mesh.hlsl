#define RootSig \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "CBV(b0, space=0, visibility=SHADER_VISIBILITY_VERTEX)"

cbuffer cbPerObject : register(b0)
{
	float4x4 gWorldViewProj;
}

struct VSInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
	uint vertexID : SV_VertexID;
};

struct PSInput
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

// Hash function for random color generation
float3 HashToColor(uint seed)
{
	uint h = seed;
	h ^= h >> 16;
	h *= 0x85ebca6b;
	h ^= h >> 13;
	h *= 0xc2b2ae35;
	h ^= h >> 16;
    
	float3 color;
	color.r = ((h >> 0) & 0xFF) / 255.0;
	color.g = ((h >> 8) & 0xFF) / 255.0;
	color.b = ((h >> 16) & 0xFF) / 255.0;
	return color;
}

[RootSignature(RootSig)]
PSInput VSMain(VSInput input)
{
	PSInput result;
	result.position = mul(gWorldViewProj, float4(input.position, 1.0f));
	result.color = float4(HashToColor(input.vertexID), 1.0f);
	return result;
}

[RootSignature(RootSig)]
float4 PSMain(PSInput input) : SV_TARGET
{
	return input.color;
}
