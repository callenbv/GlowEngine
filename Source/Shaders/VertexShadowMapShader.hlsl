// Vertex shader input struct
struct VertexInputType
{
  float4 position : POSITION;
  float3 normal : NORMAL;
};

// Vertex shader output struct
struct ShadowVertexOutput
{
  float4 position : SV_POSITION;
};

// Constant buffers
cbuffer LightMatrixBuffer : register(b0)
{
  matrix lightViewProjectionMatrix;
};

ShadowVertexOutput main(VertexInputType input)
{
  ShadowVertexOutput output;

    // Transform the vertex position into light view-projection space
  output.position = mul(input.position, lightViewProjectionMatrix);

  return output;
}
