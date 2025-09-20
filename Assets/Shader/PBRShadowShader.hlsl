// En TextureMapShader.hlsl

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

Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D roughnessTexture : register(t2);
Texture2D metallicTexture : register(t3);
Texture2D aoTexture : register(t4);
Texture2D shadowMap : register(t5);

SamplerState baseSampler : register(s0);
SamplerComparisonState shadowMapSampler : register(s1);

struct VSInput
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
    float3 norm : NORMAL;
    float3 tang : TANGENT;
    float4 debugColor : COLOR;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 worldNorm : TEXCOORD1;
    float2 tex : TEXCOORD2;
    float3 worldTangent : TEXCOORD3;
    float4 lightSpacePosition : TEXCOORD4;
    float4 debugColor : COLOR;
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
    output.debugColor = input.debugColor;

    float U_scale = textureTransform.x;
    float V_scale = textureTransform.y;
    float U_offset = textureTransform.z;
    float V_offset = textureTransform.w;

    output.tex.x = input.tex.x * U_scale + U_offset;
    output.tex.y = input.tex.y * V_scale + V_offset;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    // return float4(1, 0, 1, 1); // Color magenta para depuración
    // Muestrea el color base del albedo desde la textura
    // float4 baseColor1 = albedoTexture.Sample(baseSampler, input.tex);
    // return baseColor1;
    
    // Normaliza la normal y el tangente en espacio mundial
    float3 N = normalize(input.worldNorm);
    float3 T = normalize(input.worldTangent);

    // Calcula el bitangente usando el producto cruzado
    float3 B = normalize(cross(N, T));

    // Matriz TBN para transformar normales del espacio de textura al espacio mundial
    float3x3 TBN = float3x3(T, B, N);

    // Muestrea las texturas de materiales
    float4 baseColor = albedoTexture.Sample(baseSampler, input.tex);
    float roughness = roughnessTexture.Sample(baseSampler, input.tex).r;
    float metallic = metallicTexture.Sample(baseSampler, input.tex).r;
    float ao = aoTexture.Sample(baseSampler, input.tex).r;
    float3 F0 = materialF0;

    // Si el color base es transparente o negro, usa el albedo por defecto del material
    if (baseColor.a == 0 && baseColor.r == 0 && baseColor.g == 0 && baseColor.b == 0)
    {
        baseColor = materialAlbedo;
    }

    // Muestrea la textura de normales y la transforma al espacio mundial
    float3 sampledNormal = normalTexture.Sample(baseSampler, input.tex).rgb;
    sampledNormal = sampledNormal * 2.0f - 1.0f;
    N = normalize(mul(sampledNormal, TBN));

    // Calcula el vector de vista (hacia la cámara)
    float3 V = normalize(cameraPosition - input.worldPos);

    // Calcula el vector de luz (dirección opuesta a la luz)
    float3 L = normalize(-lightDirection);

    // Difusa: calcula la intensidad difusa usando el ángulo entre la normal y la luz
    float NdotL = saturate(dot(N, L));
    float3 diffuse = baseColor.rgb * NdotL * lightColor.rgb;

    // Especular: calcula el brillo especular usando el modelo de microfacet
    float3 H = normalize(L + V);
    float NdotH = saturate(dot(N, H));
    float specularIntensity = pow(NdotH, (1.0 - roughness) * 1000.0 + 1.0);
    
    // Fresnel: calcula el factor de fresnel para el material
    float3 F = F0 + (1.0 - F0) * pow(1.0 - saturate(dot(H, V)), 5.0);

    // Mezcla el color especular entre el fresnel y el color base según el metalicidad
    float3 specColor = lerp(F, baseColor.rgb, metallic);
    float3 specular = specColor * specularIntensity * lightColor.rgb;

    // Calcula la componente ambiental usando el AO
    float3 ambient = baseColor.rgb * 0.03 * ao;

    // Calcula las coordenadas de textura para el mapa de sombras
    float2 shadowTexCoord = input.lightSpacePosition.xy / input.lightSpacePosition.w;
    shadowTexCoord = shadowTexCoord * 0.5f + 0.5f;

    // Calcula la profundidad actual en el espacio de luz
    float currentDepth = input.lightSpacePosition.z / input.lightSpacePosition.w;

    // Pequeño sesgo para evitar artefactos de sombra
    float bias = 0.0005f;
    
    float shadowFactor = 1.0f;

    // Verifica si la coordenada de sombra está dentro del rango válido
    if (shadowTexCoord.x >= 0.0f && shadowTexCoord.x <= 1.0f &&
         shadowTexCoord.y >= 0.0f && shadowTexCoord.y <= 1.0f)
    {
        // Muestrea el mapa de sombras usando comparación de profundidad
        shadowFactor = shadowMap.SampleCmp(shadowMapSampler, shadowTexCoord, currentDepth - bias);
    }

    // Combina los componentes difuso, especular y ambiental, aplicando el factor de sombra
    float3 finalColor = (diffuse + specular) * shadowFactor + ambient;
    
    // Mezclar con el color de debug si es distinto de 0
    finalColor = finalColor + input.debugColor.rgb;

    // Devuelve el color final con el canal alfa del albedo
    return float4(finalColor, baseColor.a);
}
