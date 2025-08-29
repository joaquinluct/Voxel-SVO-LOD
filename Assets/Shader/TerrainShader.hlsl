// En TerrainShader.hlsl

// Buffers de constantes estándar
cbuffer MatrixBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

cbuffer CameraBuffer : register(b1)
{
    float3 cameraPosition;
};

cbuffer DirectionalLightBuffer : register(b2)
{
    float3 lightDirection;
    float3 lightColor;
};


cbuffer MaterialBuffer : register(b3)
{
    float4 materialAlbedo;
    float3 materialF0;
    float materialRoughness;
    float materialMetallic;
    float materialAO;
};

cbuffer TextureTransformBuffer : register(b4)
{
    float4 textureTransform; // (U_scale, V_scale, U_offset, V_offset)
};

cbuffer LightSpaceMatrices : register(b5)
{
    float4x4 lightViewProjectionMatrix;
};

// --- Nuevo buffer para los parámetros de mezcla de terreno ---
cbuffer TerrainBlendBuffer : register(b11)
{
    float grassHeight;
    float dirtHeight;
    float snowHeight;
    float slopeStart;
    float slopeEnd;
    float terrainScale;
};



// Textura del mapa de sombras
Texture2D shadowMap : register(t5);
// --- Nuevo array de texturas para las capas del bioma ---
// Las texturas deben estar ordenadas en el array:
// [0]=grass_albedo, [1]=dirt_albedo, [2]=rock_albedo, [3]=snow_albedo,
// [4]=grass_normal, [5]=dirt_normal, [6]=rock_normal, [7]=snow_normal,
// [8]=grass_roughness, [9]=dirt_roughness, [10]=rock_roughness, [11]=snow_roughness
Texture2DArray terrainTextures : register(t6);

// Samplers
SamplerState baseSampler : register(s0);
SamplerComparisonState shadowMapSampler : register(s1);

// Estructuras de entrada y salida del Vertex Shader (VS)
struct VSInput
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
    float3 norm : NORMAL;
    float3 tang : TANGENT;
    float4 debugColor : COLOR; // Color para debug (si lo necesitas)
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

// Vertex Shader
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

    float U_scale = textureTransform.x;
    float V_scale = textureTransform.y;
    float U_offset = textureTransform.z;
    float V_offset = textureTransform.w;

    output.tex.x = input.tex.x * U_scale + U_offset;
    output.tex.y = input.tex.y * V_scale + V_offset;

    return output;
}

// Pixel Shader
float4 PSMain(PSInput input) : SV_TARGET
{
    // --- 1. Lógica de mezcla de texturas de terreno ---


    // Coordenadas de textura para triplanar mapping
    float3 blendedTexCoord = input.worldPos.xyz * terrainScale;
    float2 uvX = blendedTexCoord.zy;
    float2 uvY = blendedTexCoord.xz;
    float2 uvZ = blendedTexCoord.xy;
    
    // Pesos de mezcla para triplanar mapping
    float3 triplanarWeights = abs(input.worldNorm);
    triplanarWeights /= (triplanarWeights.x + triplanarWeights.y + triplanarWeights.z);
    
    // --- Muestreo de las texturas usando Texture2DArray ---
    // La tercera coordenada (z) es el índice de la capa.
    
    // Albedo: Las capas 0, 1, 2 y 3
    float3 sampledGrassAlbedo = terrainTextures.Sample(baseSampler, float3(uvY, 0.0f)).rgb;
    float3 sampledRockAlbedo = terrainTextures.Sample(baseSampler, float3(uvY, 1.0f)).rgb;
    float3 sampledDirtAlbedo = terrainTextures.Sample(baseSampler, float3(uvX, 2.0f)).rgb * triplanarWeights.x +
                               terrainTextures.Sample(baseSampler, float3(uvY, 2.0f)).rgb * triplanarWeights.y +
                               terrainTextures.Sample(baseSampler, float3(uvZ, 2.0f)).rgb * triplanarWeights.z;
    float3 sampledSnowAlbedo = terrainTextures.Sample(baseSampler, float3(uvX, 3.0f)).rgb * triplanarWeights.x +
                               terrainTextures.Sample(baseSampler, float3(uvY, 3.0f)).rgb * triplanarWeights.y +
                               terrainTextures.Sample(baseSampler, float3(uvZ, 3.0f)).rgb * triplanarWeights.z;

    // Normales: Las capas 4, 5, 6 y 7
    float3 sampledGrassNormal = terrainTextures.Sample(baseSampler, float3(uvY, 4.0f)).rgb;
    float3 sampledRockNormal = terrainTextures.Sample(baseSampler, float3(uvY, 5.0f)).rgb;
    float3 sampledDirtNormal = terrainTextures.Sample(baseSampler, float3(uvX, 6.0f)).rgb * triplanarWeights.x +
                               terrainTextures.Sample(baseSampler, float3(uvY, 6.0f)).rgb * triplanarWeights.y +
                               terrainTextures.Sample(baseSampler, float3(uvZ, 6.0f)).rgb * triplanarWeights.z;
    float3 sampledSnowNormal = terrainTextures.Sample(baseSampler, float3(uvX, 7.0f)).rgb * triplanarWeights.x +
                               terrainTextures.Sample(baseSampler, float3(uvY, 7.0f)).rgb * triplanarWeights.y +
                               terrainTextures.Sample(baseSampler, float3(uvZ, 7.0f)).rgb * triplanarWeights.z;
    
    // Roughness: Las capas 8, 9, 10 y 11
    float sampledGrassRoughness = terrainTextures.Sample(baseSampler, float3(uvY, 8.0f)).r;
    float sampledRockRoughness = terrainTextures.Sample(baseSampler, float3(uvY, 9.0f)).r;
    float sampledDirtRoughness = terrainTextures.Sample(baseSampler, float3(uvX, 10.0f)).r * triplanarWeights.x +
                                 terrainTextures.Sample(baseSampler, float3(uvY, 10.0f)).r * triplanarWeights.y +
                                 terrainTextures.Sample(baseSampler, float3(uvZ, 10.0f)).r * triplanarWeights.z;
    float sampledSnowRoughness = terrainTextures.Sample(baseSampler, float3(uvX, 11.0f)).r * triplanarWeights.x +
                                 terrainTextures.Sample(baseSampler, float3(uvY, 11.0f)).r * triplanarWeights.y +
                                 terrainTextures.Sample(baseSampler, float3(uvZ, 11.0f)).r * triplanarWeights.z;

    // --- 2. Cálculo de pesos de mezcla ---
    float height = input.worldPos.y;
    float slope = 1.0f - abs(input.worldNorm.y);

    float rockWeight = saturate(smoothstep(slopeStart, slopeEnd, slope));
    float snowWeight = saturate(smoothstep(snowHeight - 40.0f, snowHeight + 40.0f, height));
    float dirtWeight = saturate(smoothstep(dirtHeight - 60.0f, dirtHeight + 60.0f, height)) * saturate(1.0f - rockWeight);

    float grassWeight = saturate(1.0f - rockWeight - snowWeight - dirtWeight);

    float totalWeight = grassWeight + dirtWeight + rockWeight + snowWeight;
    grassWeight /= totalWeight;
    dirtWeight /= totalWeight;
    rockWeight /= totalWeight;
    snowWeight /= totalWeight;

    // --- 3. Mezcla final de texturas y propiedades PBR ---
    float3 finalAlbedo = (sampledGrassAlbedo * grassWeight) +
                         (sampledDirtAlbedo * dirtWeight) +
                         (sampledRockAlbedo * rockWeight) +
                         (sampledSnowAlbedo * snowWeight);

    float3 finalNormalMap = (sampledGrassNormal * grassWeight) +
                            (sampledDirtNormal * dirtWeight) +
                            (sampledRockNormal * rockWeight) +
                            (sampledSnowNormal * snowWeight);
    finalNormalMap = finalNormalMap * 2.0f - 1.0f;

    float finalRoughness = (sampledGrassRoughness * grassWeight) +
                           (sampledDirtRoughness * dirtWeight) +
                           (sampledRockRoughness * rockWeight) +
                           (sampledSnowRoughness * snowWeight);

    float finalMetallic = 0.0f;

    // --- 4. El resto del código PBR (usando los valores finales) ---
    float3 F0 = materialF0;

    float3 N = normalize(input.worldNorm);
    float3 T = normalize(input.worldTangent);
    float3 B = normalize(cross(N, T));
    float3x3 TBN = float3x3(T, B, N);

    N = normalize(mul(finalNormalMap, TBN));

    float3 V = normalize(cameraPosition - input.worldPos);
    float3 L = normalize(-lightDirection);

    float NdotL = saturate(dot(N, L));
    float3 diffuse = finalAlbedo * NdotL * lightColor.rgb;

    float3 H = normalize(L + V);
    float NdotH = saturate(dot(N, H));
    float specularIntensity = pow(NdotH, (1.0 - finalRoughness) * 1000.0 + 1.0);
    
    float3 F = F0 + (1.0 - F0) * pow(1.0 - saturate(dot(H, V)), 5.0);

    float3 specColor = lerp(F, finalAlbedo.rgb, finalMetallic);
    float3 specular = specColor * specularIntensity * lightColor.rgb;

    float3 ambient = finalAlbedo * 0.03 * materialAO;

    float2 shadowTexCoord = input.lightSpacePosition.xy / input.lightSpacePosition.w;
    shadowTexCoord = shadowTexCoord * 0.5f + 0.5f;

    float currentDepth = input.lightSpacePosition.z / input.lightSpacePosition.w;
    float bias = 0.0005f;
    
    float shadowFactor = 1.0f;
    if (shadowTexCoord.x >= 0.0f && shadowTexCoord.x <= 1.0f &&
        shadowTexCoord.y >= 0.0f && shadowTexCoord.y <= 1.0f)
    {
        shadowFactor = shadowMap.SampleCmp(shadowMapSampler, shadowTexCoord, currentDepth - bias);
    }

    float3 finalColor = (diffuse + specular) * shadowFactor + ambient;
    
    return float4(finalColor, 1.0f);
}