// En TextureMapShader.hlsl

cbuffer MatrixBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

cbuffer DirectionalLightBuffer : register(b1)
{
    float3 lightDirection;
    float3 lightColor;
};

cbuffer CameraBuffer : register(b2)
{
    float3 cameraPosition;
};

cbuffer MaterialBuffer : register(b3)
{
    float4 materialAlbedo;
    float materialRoughness;
    float materialMetallic;
    float3 materialF0;
    float materialAO;
};

cbuffer LightSpaceMatrices : register(b4)
{
    float4x4 lightViewProjectionMatrix;
};

Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D roughnessTexture : register(t2);
Texture2D metallicTexture : register(t3);
Texture2D aoTexture : register(t4);
SamplerState SamplerType : register(s0);

Texture2D shadowMap : register(t5);
SamplerComparisonState ShadowSampler : register(s1);

struct VSInput
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
    float3 norm : NORMAL;
    float3 tang : TANGENT;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 worldNorm : TEXCOORD1;
    float2 tex : TEXCOORD2;
    float3 worldTangent : TEXCOORD3;
    float4 lightSpacePosition : TEXCOORD4;
};

PSInput VSMain(VSInput input)
{
    PSInput output;

    float4 worldPos = mul(float4(input.pos, 1.0f), worldMatrix);
    output.worldPos = worldPos.xyz;

    output.pos = mul(worldPos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);

    output.lightSpacePosition = mul(worldPos, lightViewProjectionMatrix);

    output.worldNorm = normalize(mul(input.norm, (float3x3) worldMatrix));
    output.worldTangent = normalize(mul(input.tang, (float3x3) worldMatrix));

    output.tex = input.tex;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float3 N = normalize(input.worldNorm);
    float3 T = normalize(input.worldTangent);

    float3 B = normalize(cross(N, T));

    float3x3 TBN = float3x3(T, B, N);

    float4 baseColor = albedoTexture.Sample(SamplerType, input.tex);
    float roughness = roughnessTexture.Sample(SamplerType, input.tex).r;
    float metallic = metallicTexture.Sample(SamplerType, input.tex).r;
    float ao = aoTexture.Sample(SamplerType, input.tex).r;
    float3 F0 = materialF0;

    if (baseColor.a == 0 && baseColor.r == 0 && baseColor.g == 0 && baseColor.b == 0)
    {
        baseColor = materialAlbedo;
    }

    float3 sampledNormal = normalTexture.Sample(SamplerType, input.tex).rgb;
    sampledNormal = sampledNormal * 2.0f - 1.0f;
    N = normalize(mul(sampledNormal, TBN));

    float3 V = normalize(cameraPosition - input.worldPos);

    float3 L = normalize(-lightDirection);

    float NdotL = saturate(dot(N, L));
    float3 diffuse = baseColor.rgb * NdotL * lightColor.rgb;

    float3 H = normalize(L + V);
    float NdotH = saturate(dot(N, H));
    float specularIntensity = pow(NdotH, (1.0 - roughness) * 1000.0 + 1.0);
    
    float3 F = F0 + (1.0 - F0) * pow(1.0 - saturate(dot(H, V)), 5.0);

    float3 specColor = lerp(F, baseColor.rgb, metallic);
    float3 specular = specColor * specularIntensity * lightColor.rgb;

    float3 ambient = baseColor.rgb * 0.03 * ao;

    float2 shadowTexCoord = input.lightSpacePosition.xy / input.lightSpacePosition.w;
    shadowTexCoord = shadowTexCoord * 0.5f + 0.5f;

    float currentDepth = input.lightSpacePosition.z / input.lightSpacePosition.w;

    float bias = 0.0005f;
    
    float shadowFactor = 1.0f;

    if (shadowTexCoord.x >= 0.0f && shadowTexCoord.x <= 1.0f &&
        shadowTexCoord.y >= 0.0f && shadowTexCoord.y <= 1.0f)
    {
        shadowFactor = shadowMap.SampleCmp(ShadowSampler, shadowTexCoord, currentDepth - bias);
    }

    float3 finalColor = (diffuse + specular) * shadowFactor + ambient;

    return float4(finalColor, baseColor.a);
}