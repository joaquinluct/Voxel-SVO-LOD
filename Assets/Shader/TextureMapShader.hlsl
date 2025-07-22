//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------
cbuffer MatrixBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

// --- NUEVO: Buffer para la luz direccional (sol) ---
cbuffer LightBuffer : register(b1) // Asignamos un nuevo slot, por ejemplo, b1
{
    float4 LightAmbientColor; // Color ambiental de la luz
    float4 LightDiffuseColor; // Color difuso de la luz
    float4 LightSpecularColor; // Color especular de la luz
    float3 LightDirection; // Dirección de la luz (normalizada, apunta DESDE la luz)
    float paddingLight; // Relleno para alinear a 16 bytes
};

// --- NUEVO: Buffer para la posición de la cámara (necesario para especular) ---
cbuffer CameraBuffer : register(b2) // Otro nuevo slot, por ejemplo, b2
{
    float3 CameraPosition; // Posición de la cámara en espacio de mundo
    float paddingCamera; // Relleno
};

// --- OPCIONAL: Buffer para las propiedades del material si no vienen de texturas PBR completas ---
// Si tus texturas PBR ya te dan todo, podrías no necesitar este,
// pero es útil para MaterialShininess (exponente especular)
cbuffer MaterialBuffer : register(b3) // Otro nuevo slot, por ejemplo, b3
{
    // float4 MaterialAmbient;   // Si tuvieras un color ambiental del material
    // float4 MaterialDiffuse;   // Si tuvieras un color difuso del material
    // float4 MaterialSpecular;  // Si tuvieras un color especular del material
    float MaterialShininess; // Brillo/potencia especular (para el highlight)
    float3 paddingMaterial; // Relleno
};

//--------------------------------------------------------------------------------------
// Texture and Sampler Resources
//--------------------------------------------------------------------------------------
Texture2D albedoTexture : register(t0); // Color base
Texture2D normalTexture : register(t1); // Detalle de superficie (baches)
Texture2D roughnessTexture : register(t2); // Rugosidad (dispersión de la luz)
Texture2D aoTexture : register(t3); // Oclusión ambiental (sombras de contacto)

SamplerState SamplerType : register(s0); // Sampler para todas las texturas PBR

//--------------------------------------------------------------------------------------
// Vertex Shader Input & Output Structures
//--------------------------------------------------------------------------------------
// VSInput: Lo que recibimos del Vertex Buffer (corresponde a tu PBRVertex en C++)
struct VSInput
{
    float4 pos : POSITION; // Posición del vértice (x, y, z, w=1.0)
    float3 norm : NORMAL; // Normal del vértice
    float2 tex : TEXCOORD; // Coordenadas de textura UV
    // --- IMPORTANTE: Si usas Normal Mapping, NECESITAS la tangente del vértice ---
    float3 tangent : TANGENT; // Añadir si necesitas tangentes para normal mapping avanzado
};

// VSOutput / PSInput: Lo que el VS envía al PS (interpolado automáticamente)
struct PSInput
{
    float4 pos : SV_POSITION; // Posición final del píxel en espacio de clip
    float3 worldPos : TEXCOORD0; // Posición del píxel en espacio de mundo (para cálculos de luz)
    float3 worldNorm : TEXCOORD1; // Normal del píxel en espacio de mundo
    float2 tex : TEXCOORD2; // Coordenadas de textura UV interpoladas
    // --- IMPORTANTE: Pasar la tangente del mundo si la usas para Normal Mapping ---
    float3 worldTangent : TEXCOORD3; // Tangente del píxel en espacio de mundo
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PSInput VSMain(VSInput input)
{
    PSInput output;

    // 1. Transformar la posición del vértice
    // De espacio de objeto a espacio de mundo
    output.worldPos = mul(input.pos, world).xyz;
    // De espacio de mundo a espacio de clip (pasando por vista y proyección)
    output.pos = mul(float4(output.worldPos, 1.0f), view);
    output.pos = mul(output.pos, projection);

    // 2. Transformar la normal del vértice a espacio de mundo
    // Solo se aplican rotación y escala de la matriz world (por eso (float3x3)world)
    output.worldNorm = mul(input.norm, (float3x3) world);
    output.worldNorm = normalize(output.worldNorm); // Normalizar para asegurar que sea de longitud 1

    // 3. Pasar las coordenadas de textura sin modificar
    output.tex = input.tex;

    // --- IMPORTANTE: Transformar la tangente al espacio de mundo si la usas ---
    output.worldTangent = mul(input.tangent, (float3x3) world);
    output.worldTangent = normalize(output.worldTangent);

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PSMain(PSInput input) : SV_TARGET
{
    // 1. Muestrear las texturas PBR
    float4 albedoColor = albedoTexture.Sample(SamplerType, input.tex);
    float3 sampledNormal = normalTexture.Sample(SamplerType, input.tex).rgb; // Rango [0,1]
    float roughness = roughnessTexture.Sample(SamplerType, input.tex).r;
    float ambientOcclusion = aoTexture.Sample(SamplerType, input.tex).r;

    // 2. Calcular la normal final del píxel (usando el normal map)
    // Remapear la normal muestreada de [0,1] a [-1,1]
    sampledNormal = sampledNormal * 2.0f - 1.0f;

    // Construir la matriz Tangente-Bitangente-Normal (TBN)
    // Necesitamos la bitangente. Si no la calculas en C++, puedes hacerlo aquí:
    float3 N = normalize(input.worldNorm);
    float3 T = normalize(input.worldTangent);
    float3 B = normalize(cross(N, T)); // Bitangente (asegúrate de que N y T son ortogonales)

    // A veces, el cross product N x T puede dar una Bitangente invertida,
    // si eso ocurre, puedes añadir un multiplicador al valor "w" de la tangente en tu modelo
    // (tangent.w) que indica la orientación de la bitangente, o simplemente invertirla si los resultados son incorrectos.
    // Ejemplo: B = normalize(cross(N, T) * input.tangent.w);

    float3x3 TBN = float3x3(T, B, N); // Matriz para transformar de espacio Tangente a espacio de Mundo

    // Transformar la normal del mapa (que está en espacio tangente) a espacio de mundo
    float3 finalNormal = normalize(mul(sampledNormal, TBN));

    // 3. Cálculos de iluminación con la luz direccional
    // Dirección desde la superficie hacia la luz. LightDirection apunta DESDE la luz.
    // Para los cálculos de iluminación, necesitamos la dirección HACIA la luz.
    float3 lightDir = normalize(-LightDirection); // Invertimos la dirección de la luz

    // Dirección desde la superficie hacia la cámara (observador)
    float3 viewDir = normalize(CameraPosition - input.worldPos);

    // --- Componente Ambiental (solo con AO) ---
    // La luz ambiental es uniforme y atenuada por la oclusión ambiental.
    float3 ambient = LightAmbientColor.rgb * albedoColor.rgb * ambientOcclusion;

    // --- Componente Difuso (Lambertian) ---
    // Depende del ángulo entre la normal de la superficie y la dirección de la luz.
    // max(0.0f, ...) asegura que no tengamos iluminación negativa.
    float diffuseFactor = max(0.0f, dot(finalNormal, lightDir));
    float3 diffuse = LightDiffuseColor.rgb * albedoColor.rgb * diffuseFactor;

    // --- Componente Especular (Phong/Blinn-Phong, no PBR completo, pero funcional) ---
    // Un simple Phong/Blinn-Phong. Un PBR real usaría un modelo como Cook-Torrance.
    float3 specular = float3(0.0f, 0.0f, 0.0f);
    if (diffuseFactor > 0.0f) // Solo si hay luz directa en la superficie
    {
        // Vector de reflexión de la luz
        // reflect(I, N) donde I es el vector que apunta HACIA la superficie
        // y N es la normal de la superficie.
        // lightDir apunta HACIA la luz, así que -lightDir apunta HACIA la superficie.
        float3 reflectedLight = normalize(reflect(-lightDir, finalNormal));

        // Ángulo entre el vector de reflexión y el vector de vista
        float specFactor = pow(max(0.0f, dot(reflectedLight, viewDir)), MaterialShininess);
        specular = LightSpecularColor.rgb * specFactor; // Asumimos color especular blanco
    }

    // Si estás usando metalness, tendrías que mezclar albedo y specular de forma diferente.
    // Aquí, se suma para un modelo tipo "plástico/dielectric".

    // 4. Combinar todos los componentes de iluminación
    float3 finalLighting = ambient + diffuse + specular;

    // El color final es la multiplicación del color de albedo por la iluminación calculada.
    float4 finalColor = float4(albedoColor.rgb * finalLighting, albedoColor.a);

    return finalColor;
}