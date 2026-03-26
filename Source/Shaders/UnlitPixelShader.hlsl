struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 worldpos : WORLDPOS;
    float4 color : COLOR;
    float3 normal : NORMAL; // unused in unlit
    float2 texcoord : TEXCOORD;
    float4 shadowCoord : TEXCOORD1; // unused in unlit
};

cbuffer UvScale : register(b0)
{
    float2 uvScale;
    float2 _uvPad;
};

cbuffer MaterialRegister : register(b4)
{
    float4 baseColor; // rgb tint, a = alpha (dissolve)
    float4 specularData; // unused
    float4 ambientData; // rgb unused, a = useTexture (0/1)
};

SamplerState SampleType : register(s0);
Texture2D diffuseTexture : register(t0);

float4 main(PixelInputType input) : SV_TARGET
{
    float useTexture = ambientData.a;

    // UV scale
    float2 uv = input.texcoord * uvScale;

    // Sample (bind a 1x1 white when no texture, or just lerp)
    float4 tex = diffuseTexture.Sample(SampleType, uv);

    // Unlit color = baseColor * (tex or white)
    float4 texOrWhite = lerp(float4(1, 1, 1, 1), tex, saturate(useTexture));
    float4 outColor = baseColor * texOrWhite;

    return outColor;
}