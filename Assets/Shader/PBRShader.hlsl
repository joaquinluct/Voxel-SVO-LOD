// En TextureMapShader.hlsl

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

    // Transformación de posición
    output.pos = mul(float4(input.pos, 1.0f), worldMatrix);
    output.worldPos = output.pos.xyz; // Posición en espacio mundo
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);

    // Transformación de la normal y la tangente a espacio mundo
    output.worldNorm = normalize(mul(input.norm, (float3x3) worldMatrix));
    output.worldTangent = normalize(mul(input.tang, (float3x3) worldMatrix));

    // Pasar coordenadas de textura
    output.tex = input.tex;

    return output;
}

// Pixel Shader
float4 PSMain(PSInput input) : SV_TARGET
{
    // Normalizar la normal y la tangente interpoladas
    float3 N = normalize(input.worldNorm);
    float3 T = normalize(input.worldTangent);

    // Calcular la bitangente en el PS (más común)
    float3 B = normalize(cross(N, T));

    // Construir la matriz TBN (Tangent, Bitangent, Normal)
    float3x3 TBN = float3x3(T, B, N);

    // --- Obtener los parámetros del material (PBR) ---
    // Puedes usar texturas o los valores constantes del buffer.
    // Aquí priorizamos las texturas si están disponibles, si no, los valores del constant buffer.
    float4 baseColor = albedoTexture.Sample(SamplerType, input.tex);
    float roughness = roughnessTexture.Sample(SamplerType, input.tex).r;
    float metallic = metallicTexture.Sample(SamplerType, input.tex).r;
    float ao = aoTexture.Sample(SamplerType, input.tex).r;
    float3 F0 = materialF0; // F0 para dieléctricos es constante, para metales se recalcula

    // Si no tienes textura para algo, usa el valor del constant buffer
    if (baseColor.a == 0 && baseColor.r == 0 && baseColor.g == 0 && baseColor.b == 0)
    { // Comprobar si la textura está "vacía" o no asignada
        baseColor = materialAlbedo;
    }
    // Repite esta lógica para roughness, metallic, ao si quieres un fallback a valores constantes
    // Es mejor tener un sistema para saber si la textura está asignada o no desde C++.

    // Muestrear el mapa normal y transformarlo a espacio mundo
    float3 sampledNormal = normalTexture.Sample(SamplerType, input.tex).rgb;
    sampledNormal = sampledNormal * 2.0f - 1.0f; // Mapear de [0,1] a [-1,1]
    N = normalize(mul(sampledNormal, TBN)); // Normal final en espacio mundo

    // --- CÁLCULO DE ILUMINACIÓN PBR ---
    // (Simplificado, esto sería mucho más complejo en un motor PBR completo)

    // Vector de la cámara (observador)
    float3 V = normalize(cameraPosition - input.worldPos);

    // Vector de la luz
    float3 L = normalize(-lightDirection); // La dirección de la luz es hacia la luz

    // Componente difusa (Lambertian)
    float NdotL = saturate(dot(N, L));
    float3 diffuse = baseColor.rgb * NdotL * lightColor.rgb;

    // Componente especular (microfaceta, aproximación de Blinn-Phong para simplificar)
    float3 H = normalize(L + V); // Half-vector
    float NdotH = saturate(dot(N, H));
    float specularIntensity = pow(NdotH, (1.0 - roughness) * 1000.0 + 1.0); // Roughness afecta el brillo
    
    // Para la parte especular PBR (Frensel, D, G) esto sería mucho más complejo
    float3 F = F0 + (1.0 - F0) * pow(1.0 - saturate(dot(H, V)), 5.0); // Aproximación de Fresnel

    // Si es metálico, el albedo se convierte en F0 y no hay difuso "blanco"
    float3 specColor = lerp(F, baseColor.rgb, metallic); // Para metales, F0 es el color base
    float3 specular = specColor * specularIntensity * lightColor.rgb;

    // Oclusión ambiental
    float3 ambient = baseColor.rgb * 0.03 * ao; // Pequeña luz ambiental * AO

    // Color final
    float3 finalColor = diffuse + specular + ambient;

    return float4(finalColor, baseColor.a);
}