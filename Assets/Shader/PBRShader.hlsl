// PBRShader.hlsl

// Constant Buffer para matrices (World, View, Projection)
// Corresponde a MatrixBufferType en C++
cbuffer MatrixBuffer : register(b0)
{
    float4x4 worldMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

// Constant Buffer para Luz Direccional (PBR)
// Corresponde a Light::DirectionalLight en C++
cbuffer DirectionalLightBuffer : register(b1)
{
    float3 lightDirection; // Dirección de la luz (hacia la luz, normalizada)
    float3 lightColor; // Color e intensidad de la luz
};

// Constant Buffer para datos de Cámara
// Corresponde a Light::CameraData en C++
cbuffer CameraBuffer : register(b2)
{
    float3 cameraPosition; // Posición de la cámara en espacio mundo
};

// Constant Buffer para datos de Material (PBR)
// Corresponde a Light::MaterialData en C++
cbuffer MaterialBuffer : register(b3)
{
    float4 materialAlbedo; // Color base del material (RGBA)
    float materialRoughness; // Rugosidad
    float materialMetallic; // Metalicidad
    float3 materialF0; // Reflectividad especular a 0 grados para dieléctricos
    float materialAO; // Oclusión ambiental
};

cbuffer TextureTransformBuffer : register(b4)
{
    float4 textureTransform; // (U_scale, V_scale, U_offset, V_offset)
};

// ... (tus texturas y samplers, si los usas en lugar de los valores constantes del MaterialBuffer)
Texture2D albedoTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D roughnessTexture : register(t2);
Texture2D metallicTexture : register(t3);
Texture2D aoTexture : register(t4);
SamplerState SamplerType : register(s0);

// Estructuras de entrada/salida del shader
struct VSInput
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
    float3 norm : NORMAL;
    float3 tang : TANGENT; // Para normal mapping
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 worldPos : TEXCOORD0;
    float3 worldNorm : TEXCOORD1;
    float2 tex : TEXCOORD2;
    float3 worldTangent : TEXCOORD3;
    // float3 worldBitangent : TEXCOORD4; // Si la calculas en VS
};


// Vertex Shader
PSInput VSMain(VSInput input)
{
    PSInput output;

    output.pos = mul(float4(input.pos, 1.0f), worldMatrix);
    output.worldPos = output.pos.xyz;
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);


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
    // return float4(.1f, .0f, .0f, 1.0f); // Original para comprobar que el PS se ejecuta

    // *** ESTRATEGIA DE DEBUGGING: COMENTA/DESCOMENTA ESTAS LÍNEAS ***

    // [PRUEBA 1] ¿Recibo las coordenadas de textura? (Deberías ver un gradiente de color)
    // return float4(input.tex.x, input.tex.y, 0.0f, 1.0f);

    // [PRUEBA 2] ¿Estoy muestreando la textura correctamente? (Deberías ver la textura sin iluminación)
    // float4 baseColor1 = albedoTexture.Sample(SamplerType, input.tex);
    // return baseColor1;

    // Normalizar la normal y la tangente interpoladas
    float3 N = normalize(input.worldNorm);
    float3 T = normalize(input.worldTangent);

    // Calcular la bitangente en el PS (más común)
    float3 B = normalize(cross(N, T));

    // [PRUEBA 3] ¿Estoy recibiendo normales correctas? 
    // (Deberías ver la geometría coloreada por sus normales)
    // float3 normalColor = N * 0.5 + 0.5; // Mapear [-1, 1] a [0, 1] para el color
    // return float4(normalColor, 1.0f);

    // Construir la matriz TBN (Tangent, Bitangent, Normal)
    float3x3 TBN = float3x3(T, B, N);

    // --- Obtener los parámetros del material (PBR) ---
    float4 baseColor = albedoTexture.Sample(SamplerType, input.tex);
    float roughness = roughnessTexture.Sample(SamplerType, input.tex).r;
    float metallic = metallicTexture.Sample(SamplerType, input.tex).r;
    float ao = aoTexture.Sample(SamplerType, input.tex).r;
    float3 F0 = materialF0;

    // Muestrear el mapa normal y transformarlo a espacio mundo
    float3 sampledNormal = normalTexture.Sample(SamplerType, input.tex).rgb;
    sampledNormal = sampledNormal * 2.0f - 1.0f;
    N = normalize(mul(sampledNormal, TBN));

    // [PRUEBA 4] ¿Está el normal mapping funcionando? 
    // (Deberías ver las normales del normal map)
    // float3 normalColorMapped = N * 0.5 + 0.5;
    // return float4(normalColorMapped, 1.0f);

    // --- CÁLCULO DE ILUMINACIÓN PBR ---

    // Vector de la cámara (observador)
    float3 V = normalize(cameraPosition - input.worldPos);

    // Vector de la luz
    float3 L = normalize(-lightDirection);

    // [PRUEBA 5] ¿Se calcula bien la componente difusa? 
    // (Deberías ver la iluminación base)
    float NdotL = saturate(dot(N, L));
    // return float4(NdotL, NdotL, NdotL, 1.0f);

    float3 diffuse = baseColor.rgb * NdotL * lightColor.rgb;

    // [PRUEBA 6] ¿Se calcula bien la componente difusa con el color? 
    // (Deberías ver la textura iluminada)
    // return float4(diffuse, baseColor.a);
 
    // Componente especular (microfaceta, aproximación de Blinn-Phong para simplificar)
    float3 H = normalize(L + V);
    float NdotH = saturate(dot(N, H));
    float specularIntensity = pow(NdotH, (1.0 - roughness) * 1000.0 + 1.0);
    float3 F = F0 + (1.0 - F0) * pow(1.0 - saturate(dot(H, V)), 5.0);

    float3 specColor = lerp(F, baseColor.rgb, metallic);
    float3 specular = specColor * specularIntensity * lightColor.rgb;

    // Oclusión ambiental
    float3 ambient = baseColor.rgb * 0.03 * ao;

    // [PRUEBA 7] ¿Se calcula bien el resultado final?
    float3 finalColor = diffuse + specular + ambient;

    return float4(finalColor, baseColor.a);
}