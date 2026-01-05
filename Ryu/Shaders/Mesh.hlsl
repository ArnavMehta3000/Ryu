#define RootSig \
    "RootFlags(ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT), " \
    "CBV(b0, space=0, visibility=SHADER_VISIBILITY_ALL), " \
    "CBV(b1, space=0, visibility=SHADER_VISIBILITY_VERTEX)"

cbuffer cbPerFrame : register(b0)
{
	float4x4 gView;
	float4x4 gProjection;
	float4x4 gViewProjection;
	float4 gCameraPosition; // xyz = position, w = unused
	float4 gTime; // x = deltaTime, y = totalTime, zw = unused
}

cbuffer cbPerObject : register(b1)
{
	float4x4 gWorld;
	float4x4 gWorldViewProj;
	//float4x4 gNormalMatrix; // Inverse transpose of world for lighting
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
	float3 worldPos : WORLDPOS;
	float3 normal : NORMAL;
	float2 texcoord : TEXCOORD;
	float4 color : COLOR;
};

[RootSignature(RootSig)]
PSInput VSMain(VSInput input)
{
	PSInput result;
    
	float4 worldPos = mul(gWorld, float4(input.position, 1.0f));
	result.position = mul(gWorldViewProj, float4(input.position, 1.0f));
	result.worldPos = worldPos.xyz;
	result.normal = float3(1.0f, 1.0f, 1.0f);
	//result.normal = mul((float3x3) gNormalMatrix, input.normal);
	result.texcoord = input.texcoord;
	result.color = input.color;
    
	return result;
}

[RootSignature(RootSig)]
float4 PSMain(PSInput input) : SV_TARGET
{
    // Simple directional light for visualization
	float3 lightDir = normalize(float3(1.0f, 1.0f, -1.0f));
	float3 normal = normalize(input.normal);
	float ndotl = saturate(dot(normal, lightDir));
    
	float3 ambient = 0.15f;
	float3 diffuse = ndotl * 0.85f;
    
	//float3 baseColor = input.color.rgb;
    float3 baseColor = float3(input.texcoord, 1.0f);
    
	return float4(baseColor * (ambient + diffuse), input.color.a);
}
