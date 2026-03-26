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

cbuffer UvScale : register(b0)
{
    float2 uvScale;
    float2 _uvPad; // keeps 16-byte alignment (optional, but clean)
};

// IMPORTANT: constant buffers are 16-byte packed.
// Use float4's to avoid float3 packing issues.
cbuffer GlobalLightBuffer : register(b1)
{
    float4 lightDir_ws; // xyz = direction, w unused
    float4 lightColor; // xyz = color, w unused
    float4 cameraPos_ws; // xyz = camera pos, w unused
};

cbuffer MaterialRegister : register(b4)
{
    float4 baseColor; // rgb = base, a = alpha
    float4 specularData; // rgb = specularColor, a = shininess
    float4 ambientData; // rgb = ambientColor, a = useTexture (0/1)
};

SamplerState SampleType : register(s0);
Texture2D diffuseTexture : register(t0);

float4 main(PixelInputType input) : SV_TARGET
{
    float useTexture = ambientData.a;
    float shininess = specularData.a;

    input.texcoord *= uvScale;

    float3 N = normalize(input.normal);

    // If lightDir_ws is "direction the light travels" (common for directional light),
    // you want L = -lightDir (vector from surface toward light).
    float3 L = normalize(-lightDir_ws.xyz);

    float3 V = normalize(cameraPos_ws.xyz - input.worldpos.xyz);
    float3 H = normalize(L + V);

    // Sample texture (bind a 1x1 white texture when no texture is present)
    float4 tex = diffuseTexture.Sample(SampleType, input.texcoord);

    // Alpha clip only when textured
    if (useTexture > 0.5 && tex.a < 0.5)
        discard;

    // Base/albedo = baseColor * (tex or white)
    float4 texOrWhite = lerp(float4(1, 1, 1, 1), tex, saturate(useTexture));
    float4 albedo = baseColor * texOrWhite;

    // Lighting
    float NdotL = max(dot(N, L), 0.0);

    float3 ambient = ambientData.rgb * albedo.rgb * 0.3f;
    float3 diffuse = lightColor.rgb * albedo.rgb * NdotL;

    // Specular (Blinn-Phong)
    float specPow = clamp(shininess, 1.0, 256.0);
    float specTerm = pow(max(dot(N, H), 0.0), specPow);
    float3 specular = lightColor.rgb * specularData.rgb * specTerm;

    float3 litRgb = ambient + diffuse + specular;
    float4 litColor = float4(litRgb, albedo.a);

    // Fog
    float fogStart = 125.0;
    float fogEnd = 350.0;
    float fogDistance = length(input.worldpos.xyz - cameraPos_ws.xyz);
    float fogFactor = saturate((fogEnd - fogDistance) / (fogEnd - fogStart));
    float3 fogColor = float3(0.5, 0.5, 0.5);

    return lerp(float4(fogColor, litColor.a), litColor, fogFactor);
}