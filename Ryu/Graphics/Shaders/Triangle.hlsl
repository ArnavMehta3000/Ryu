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

PSInput VSMain(float3 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = float4(position, 1.0f);
    result.position.xy += float2(gOffsetX, gOffsetY);
    
    result.color = color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}
