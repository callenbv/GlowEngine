// Pixel shader input struct
struct PixelInputType
{
    float4 position : SV_POSITION;
    float4 worldpos : WORLDPOS;
    float4 color : COLOR;
    float3 normal : NORMAL;
    float2 texcoord : TEXCOORD;
    float4 shadowCoord : TEXCOORD1;
};

struct ColorBuffer
{
    float4 objectColor;
};

cbuffer UvScale : register(b0)
{
  float2 uvScale;
  float padding;
  float padding2;
};

cbuffer GlobalLightBuffer : register(b1)
{
  float3 lightDirection;
  float3 lightColor;
  float3 cameraPosition;
};

cbuffer ColorRegister : register(b2)
{
    ColorBuffer colorBuffer;
};

cbuffer OutlineRegister : register(b3)
{
    ColorBuffer outlineBuffer;
};

cbuffer MaterialRegister : register(b4)
{
    float4 baseColor;
    float3 specularColor;
    float shininess;
    float useTexture;
    float3 _padMat;
};

SamplerState SampleType : register(s0);
Texture2D diffuseTexture : register(t0);

// Main entry point
float4 main(PixelInputType input) : SV_TARGET
{
    input.texcoord *= uvScale;

    float3 N = normalize(input.normal);
    float3 L = normalize(-lightDirection);
    float3 V = normalize(cameraPosition - input.worldpos.xyz);
    float3 H = normalize(L + V);

    // Sample texture (always sample; if no texture bound you can bind a 1x1 white)
    float4 tex = diffuseTexture.Sample(SampleType, input.texcoord);
    
    // alpha clip
    if (useTexture > 0.5 && tex.a < 0.5)
        discard;
    
    // Base/albedo = (texture or white) * baseColor
    float4 albedo = lerp(float4(1, 1, 1, 1), tex, useTexture);
    albedo *= baseColor;

    // Diffuse + specular
    float NdotL = max(dot(N, L), 0.0);
    float3 diffuse = NdotL * lightColor;

    float specPow = clamp(shininess, 1.0, 128.0);
    float specTerm = pow(max(dot(N, H), 0.0), specPow);
    float3 specular = specTerm * specularColor * lightColor;

    float3 finalColor = diffuse * albedo.rgb + specular;
    float4 litColor = float4(finalColor, albedo.a);

    // Apply opacity from baseColor alpha (and texture alpha if you want)
    litColor.a = albedo.a;

    // Fog
    float fogStart = 125.0f;
    float fogEnd = 250.0f;
    float fogDistance = length(input.worldpos.xyz - cameraPosition);
    float fogFactor = saturate((fogEnd - fogDistance) / (fogEnd - fogStart));
    float3 fogColor = float3(0.5, 0.5, 0.5);

    return lerp(float4(fogColor, litColor.a), litColor, fogFactor);
}