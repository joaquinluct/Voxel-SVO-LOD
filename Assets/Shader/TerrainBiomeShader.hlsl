//--------------------------------------------------------------------------------------
// Constantes y Definiciones
//--------------------------------------------------------------------------------------

#ifndef XM_PI
#define XM_PI 3.1415926535f
#endif

// Define el número máximo de biomas (Asegúrate que coincida con tu constante C++)
#define MAX_BIOMES_COUNT 20 

// --- FUNCIONES AUXILIARES ---
// Hash determinista para seleccionar el Sub-set de textura por área
uint Hash2D(float2 p, uint seed)
{
    uint x = uint(p.x * 23.45f) + seed;
    uint y = uint(p.y * 34.56f) + seed;
    uint h = (x << 16) | (x >> 16);
    h ^= y;
    h *= 0x5bd1e995;
    h ^= h >> 15;
    return h;
}

// --- Estructura de datos de un solo Bioma ---
struct BiomeRenderDataHLSL
{
    // Índices para 3 Subsets por cada capa de altura
    uint coastalBaseIndex[3];
    uint plainBaseIndex[3];
    uint mountainBaseIndex[3];
    
    // Parámetros de Transición (Altura H y Suavizado B)
    float coastalTransitionH;
    float coastalTransitionB;
    float plainTransitionH;
    float plainTransitionB;
    float mountainTransitionH;
    float mountainTransitionB;
    
    // Slopes y Seed
    float rockTransitionSlope;
    float snowTransitionSlope;
    uint biomeSeed;
    float padding;
};


//--------------------------------------------------------------------------------------
// Buffers de constantes
//--------------------------------------------------------------------------------------
cbuffer MatrixBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

cbuffer CameraBuffer : register(b1)
{
    float3 cameraPosition;
    float paddingCamera;
};

cbuffer DirectionalLightBuffer : register(b2)
{
    float3 lightDirection;
    float paddingLight1;
    float3 lightColor;
    float paddingLight2;
    float fogStartDistance;
    float fogEndDistance;
    float fogHeightFalloff;
    float paddingFog1;
    float3 fogColor;
    float fogDensity;
};

cbuffer MaterialBuffer : register(b3)
{
    float4 materialAlbedo;
    float3 materialF0;
    float materialRoughness;
    float materialMetallic;
    float materialAO;
    float paddingMaterial[2];
};

cbuffer TextureTransformBuffer : register(b4)
{
    float4 textureTransform;
};

cbuffer LightSpaceMatrices : register(b5)
{
    float4x4 lightViewProjectionMatrix;
};

// --- MULTI-BIOME BUFFER: Array de datos de todos los Biomas (b12) ---
cbuffer MultiBiomeBlendBuffer : register(b12)
{
    BiomeRenderDataHLSL allBiomes[MAX_BIOMES_COUNT];
    
    float terrainScale;
    uint MAX_BIOMES_COUNT_RUNTIME;
    uint ROCK_BASE_INDEX; // Índice del set de Roca Global
    uint SNOW_BASE_INDEX; // Índice del set de Nieve Global
};


Texture2D shadowMap : register(t5);
Texture2DArray terrainTextures : register(t6);

// Samplers
SamplerState baseSampler : register(s0);
SamplerComparisonState shadowMapSampler : register(s1);

// Estructuras de entrada y salida (Con semánticas TEXCOORDn)
struct VSInput
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
    float3 norm : NORMAL;
    float3 tang : TANGENT;
    float4 debugColor : COLOR;
    
    // Datos de Bioma
    float4 biomeIDs : TEXCOORD5;
    float4 biomeWeights : TEXCOORD6;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 worldNorm : TEXCOORD1;
    float2 tex : TEXCOORD2;
    float3 worldTangent : TEXCOORD3;
    float4 lightSpacePosition : TEXCOORD4;
    
    // Datos de Bioma Interpolados
    float4 biomeIDs : TEXCOORD5;
    float4 biomeWeights : TEXCOORD6;
    float4 debugColor : COLOR;
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
    output.debugColor = input.debugColor;

    float U_scale = textureTransform.x;
    float V_scale = textureTransform.y;
    float U_offset = textureTransform.z;
    float V_offset = textureTransform.w;

    output.tex.x = input.tex.x * U_scale + U_offset;
    output.tex.y = input.tex.y * V_scale + V_offset;

    // Pasar los atributos de Bioma
    output.biomeIDs = input.biomeIDs;
    output.biomeWeights = input.biomeWeights;

    return output;
}

// Pixel Shader
float4 PSMain(PSInput input) : SV_TARGET
{
    float height = input.worldPos.y;
    float slope = 1.0f - abs(input.worldNorm.y);
    
    // Coordenadas de textura y Triplanar
    float3 blendedTexCoord = input.worldPos.xyz * terrainScale;
    float2 uvY = blendedTexCoord.xz;
    float2 uvX = blendedTexCoord.zy;
    float2 uvZ = blendedTexCoord.xy;
    
    float3 triplanarWeights = abs(input.worldNorm);
    float triplanarTotal = triplanarWeights.x + triplanarWeights.y + triplanarWeights.z;
    triplanarWeights /= triplanarTotal;

    // --- Definiciones de Muestreo (Macros) ---
#define SAMPLE_PBR_Y(baseIndex, pbrOffset) terrainTextures.Sample(baseSampler, float3(uvY, (float)baseIndex + (float)pbrOffset)).rgb
#define SAMPLE_PBR_TRIPLANAR(baseIndex, pbrOffset) (terrainTextures.Sample(baseSampler, float3(uvX, (float)baseIndex + (float)pbrOffset)).rgb * triplanarWeights.x) + \
                                                       (terrainTextures.Sample(baseSampler, float3(uvY, (float)baseIndex + (float)pbrOffset)).rgb * triplanarWeights.y) + \
                                                       (terrainTextures.Sample(baseSampler, float3(uvZ, (float)baseIndex + (float)pbrOffset)).rgb * triplanarWeights.z)


    // --- 1. Inicialización de la mezcla final ---
    float3 finalAlbedo = 0.0f;
    float3 finalNormalMap = 0.0f;
    float finalRoughness = 0.0f;
    
    uint4 biIDs = (uint4) input.biomeIDs;
    float3 biWeights = input.biomeWeights.xyz;
    
    float totalBlendWeight = biWeights.x + biWeights.y + biWeights.z;
    biWeights /= max(totalBlendWeight, 0.001f);
    
    // --- 2. Bucle para procesar y mezclar hasta 3 biomas ---
    [unroll] 
    for (int i = 0; i < 3; ++i)
    {
        uint biomeID = biIDs[i];
        float weight = biWeights[i];
        
        if (weight <= 0.001f || biomeID >= MAX_BIOMES_COUNT)
            continue;
        
        BiomeRenderDataHLSL currentBiome = allBiomes[biomeID];
        
        // 2.1. Selección de Sub-set de Textura (Variedad)
        uint hash = Hash2D(input.worldPos.xz / 32.0f, currentBiome.biomeSeed);
        
        uint coastalChoice = hash % 3;
        uint plainChoice = (hash / 3) % 3;
        uint mountainChoice = (hash / 9) % 3;

        uint finalCoastalBaseIndex = currentBiome.coastalBaseIndex[coastalChoice];
        uint finalPlainBaseIndex = currentBiome.plainBaseIndex[plainChoice];
        uint finalMountainBaseIndex = currentBiome.mountainBaseIndex[mountainChoice];

        // 2.2. CÁLCULO DE PESOS DE ALTURA Y PENDIENTE (MEZCLA PERTURBADA)
        
        // El factor de perturbación desplaza la línea de transición verticalmente.
        // Usa la Normal Y (plano=1.0, vertical=0.0) para seguir el contorno.
        // El valor 1.0f controla la intensidad de esta perturbación en unidades de mundo.
        float heightPerturbation = input.worldNorm.y * 1.0f;
        
        // Transición COSTA -> LLANURA
        float h_start_plain = currentBiome.coastalTransitionH;
        float h_blend_plain = currentBiome.coastalTransitionB;
        
        float w_to_plain = saturate(smoothstep(
            h_start_plain - h_blend_plain + heightPerturbation,
            h_start_plain + h_blend_plain + heightPerturbation,
            height
        ));
        
        // Transición LLANURA -> MONTAÑA
        float h_start_mountain = currentBiome.plainTransitionH;
        float h_blend_mountain = currentBiome.plainTransitionB;

        float w_to_mountain = saturate(smoothstep(
            h_start_mountain - h_blend_mountain + heightPerturbation,
            h_start_mountain + h_blend_mountain + heightPerturbation,
            height
        ));
        
        float coastalW = 1.0f - w_to_plain;
        float mountainW = w_to_mountain;
        float plainW = w_to_plain * (1.0f - w_to_mountain);
        
        // Modificadores de Pendiente (Nieve y Roca)
        float snowW = saturate(smoothstep(currentBiome.mountainTransitionH, currentBiome.mountainTransitionH + currentBiome.mountainTransitionB, height));
        snowW = max(snowW, saturate(smoothstep(currentBiome.snowTransitionSlope - 0.1f, currentBiome.snowTransitionSlope + 0.1f, slope)));
        
        float rockW = saturate(smoothstep(currentBiome.rockTransitionSlope - 0.1f, currentBiome.rockTransitionSlope + 0.1f, slope));
        rockW *= (1.0f - snowW);
        
        // Normalización y aplicación de modificadores
        float fCoastalW = coastalW;
        float fPlainW = plainW * (1.0f - rockW);
        float fMountainW = mountainW * (1.0f - rockW);
        
        float fRockW = (plainW + mountainW) * rockW;
        float fSnowW = snowW;

        float fTotalW = fCoastalW + fPlainW + fMountainW + fRockW + fSnowW;
        fTotalW = max(fTotalW, 0.001f);
        
        fCoastalW /= fTotalW;
        fPlainW /= fTotalW;
        fMountainW /= fTotalW;
        fRockW /= fTotalW;
        fSnowW /= fTotalW;

        // 2.3. Muestreo PBR y Acumulación
        
        float3 subAlbedo = (SAMPLE_PBR_TRIPLANAR(finalCoastalBaseIndex, 0) * fCoastalW) +
                           (SAMPLE_PBR_Y(finalPlainBaseIndex, 0) * fPlainW) +
                           (SAMPLE_PBR_Y(finalMountainBaseIndex, 0) * fMountainW) +
                           (SAMPLE_PBR_TRIPLANAR(ROCK_BASE_INDEX, 0) * fRockW) +
                           (SAMPLE_PBR_TRIPLANAR(SNOW_BASE_INDEX, 0) * fSnowW);

        float3 subNormal = (SAMPLE_PBR_TRIPLANAR(finalCoastalBaseIndex, 1) * fCoastalW) +
                           (SAMPLE_PBR_Y(finalPlainBaseIndex, 1) * fPlainW) +
                           (SAMPLE_PBR_Y(finalMountainBaseIndex, 1) * fMountainW) +
                           (SAMPLE_PBR_TRIPLANAR(ROCK_BASE_INDEX, 1) * fRockW) +
                           (SAMPLE_PBR_TRIPLANAR(SNOW_BASE_INDEX, 1) * fSnowW);
                           
        float subRoughness = (SAMPLE_PBR_TRIPLANAR(finalCoastalBaseIndex, 2).r * fCoastalW) +
                             (SAMPLE_PBR_Y(finalPlainBaseIndex, 2).r * fPlainW) +
                             (SAMPLE_PBR_Y(finalMountainBaseIndex, 2).r * fMountainW) +
                             (SAMPLE_PBR_TRIPLANAR(ROCK_BASE_INDEX, 2).r * fRockW) +
                             (SAMPLE_PBR_TRIPLANAR(SNOW_BASE_INDEX, 2).r * fSnowW);
                             
        // Acumular la contribución de este bioma
        finalAlbedo += subAlbedo * weight;
        finalNormalMap += subNormal * weight;
        finalRoughness += subRoughness * weight;
    }
    
    // Finalización de la Normal (Descompresión)
    finalNormalMap = finalNormalMap * 2.0f - 1.0f;
    
    // --- 3. CÁLCULO PBR FINAL ---
    float3 F0 = materialF0;
    float finalMetallic = 0.0f;
    
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
    float NdotV = saturate(dot(N, V));

    // PBR Specular (Schlick-GGX)
    float alpha = finalRoughness * finalRoughness;
    float k = (alpha * alpha) / 2.0f;
    
    float D = alpha / (pow(NdotH * NdotH * (alpha * alpha - 1.0f) + 1.0f, 2.0f) * XM_PI);
    float G_L = NdotL / (NdotL * (1.0f - k) + k);
    float G_V = NdotV / (NdotV * (1.0f - k) + k);
    float G = G_L * G_V;
    
    float3 F_schlick = F0 + (1.0f - F0) * pow(1.0f - NdotV, 5.0f);
    float3 specColor = F_schlick * D * G;
    float3 specular = specColor * lightColor.rgb;
    
    float3 ambient = finalAlbedo * 0.03 * materialAO;

    // --- CÁLCULO DE SOMBRAS Y NIEBLA ---
    float shadowFactor = 1.0f;
    float2 shadowTexCoord = input.lightSpacePosition.xy / input.lightSpacePosition.w;
    shadowTexCoord = shadowTexCoord * 0.5f + 0.5f;
    float currentDepth = input.lightSpacePosition.z / input.lightSpacePosition.w;
    float bias = 0.0005f;
    
    if (shadowTexCoord.x >= 0.0f && shadowTexCoord.x <= 1.0f &&
        shadowTexCoord.y >= 0.0f && shadowTexCoord.y <= 1.0f)
    {
        shadowFactor = shadowMap.SampleCmp(shadowMapSampler, shadowTexCoord, currentDepth - bias);
    }
    
    float3 directLight = (diffuse + specular) * shadowFactor;
    float3 ambientLight = ambient;

    // Logica de Niebla Atmosferica
    float distance = length(input.worldPos - cameraPosition);
    float linearFog = saturate((distance - fogStartDistance) / (fogEndDistance - fogStartDistance));
    float heightFog = saturate(exp2(-(input.worldPos.y - fogDensity) * fogHeightFalloff));
    float fogFactor = max(linearFog, heightFog);
    
    float fogAttenCoef = 0.1f + 0.9f * pow(fogFactor, 2.0f);
    float3 attenuatedColor = fogColor * fogAttenCoef;
    
    directLight = lerp(directLight, attenuatedColor, fogFactor);
    ambientLight = lerp(ambientLight, fogColor, fogFactor);

    float3 finalColor = directLight + ambientLight;
    
    finalColor += input.debugColor.rgb;
    
    return float4(finalColor, 1.0f);
}
