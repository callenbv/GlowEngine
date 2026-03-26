struct VertexInputType
{
    float4 position : POSITION;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 worldpos : WORLDPOS;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 shadowCoord : TEXCOORD1;
};

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
    matrix lightViewProjectionMatrix;
};

PixelInputType main(VertexInputType input)
{
    PixelInputType output;

    // Transform the vertex position into world space
    output.worldpos = mul(input.position, worldMatrix);

    // Transform the vertex position into view space
    float4 viewPos = mul(output.worldpos, viewMatrix);

    // Transform the vertex position into projection space
    output.position = mul(viewPos, projectionMatrix);

    // Transform the vertex position into light view-projection space for shadow mapping
    output.shadowCoord = mul(output.worldpos, lightViewProjectionMatrix);

    // Pass through the color, texture coordinates, and normal
    output.color = input.color;
    output.texcoord = input.texcoord;
    output.normal = mul(input.normal, (float3x3)worldMatrix);

    return output;
}
