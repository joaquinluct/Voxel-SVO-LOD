// ====================================================================================
// Estructuras de datos
// ====================================================================================

// Estructura de entrada para el Vertex Shader
struct VSInput
{
    float3 position : POSITION;
};

// Estructura de entrada para el Pixel Shader
struct PSInput
{
    float4 position : SV_POSITION;
    float3 worldDirection : TEXCOORD0; // Dirección del rayo de la cámara en el espacio del mundo
};

// ====================================================================================
// Constant Buffers
// ====================================================================================

// Buffer para la matriz de vista-proyección
// Se utiliza en el Vertex Shader para la transformación de vértices
cbuffer MatrixBuffer : register(b0)
{
    matrix viewProjectionMatrix;
    //matrix view; // Matriz de la cámara (solo la parte de rotación importa)
    //matrix projection; // Matriz de proyección
};

// Buffer para la luz y el color del cielo
// Se utiliza en el Pixel Shader para la iluminación
cbuffer LightBuffer : register(b6)
{
    float4 skyColor;
    float4 lightDirection;
    float4 lightColor;
};


// ====================================================================================
// Vertex Shader (VSMain)
// ====================================================================================

PSInput VSMain(VSInput input)
{
    PSInput output;

    // Multiplica el vértice por la matriz de vista-proyección
    // La posición del skybox no debe ser afectada por la traslación de la cámara,
    // solo por la rotación. Esto se logra usando la matriz de vista sin la traslación.
    output.position = mul(float4(input.position, 1.0f), viewProjectionMatrix);
    //matrix viewNoTranslation = view;
    //viewNoTranslation._41 = 0.0f; // Ignorar X translación
    //viewNoTranslation._42 = 0.0f; // Ignorar Y translación
    //viewNoTranslation._43 = 0.0f; // Ignorar Z translación
    //output.position = mul(input.position, viewNoTranslation);
    //output.position = mul(output.position, projection);

    // Truco para asegurar que el skybox se renderiza en el fondo
    // La coordenada 'w' se establece igual a 'z', haciendo que la profundidad
    // del skybox sea 1.0 (infinito), sin importar la posición del vértice.
    output.position.w = output.position.z;

    // La dirección del rayo de la cámara es simplemente la posición del vértice
    // en el espacio del mundo. Esto se interpola para el pixel shader.
    output.worldDirection = input.position;

    return output;
}

// ====================================================================================
// Pixel Shader (PSMain)
// ====================================================================================

float4 PSMain(PSInput input) : SV_TARGET
{
    // Normaliza la dirección del rayo de la cámara
    float3 cameraRayDir = normalize(input.worldDirection);

    // Normaliza la dirección de la luz (del sol)
    float3 normalizedLightDir = normalize(lightDirection.xyz);

    // Calcula el producto punto entre la dirección del rayo y la dirección de la luz.
    // Un valor cercano a 1.0 significa que el rayo apunta directamente al sol.
    float dotProduct = dot(cameraRayDir, normalizedLightDir);

    // Pinta un disco para el sol con bordes suaves.
    // El 'smoothstep' crea un degradado que da el efecto de brillo.
    float sunDisk = smoothstep(0.999, 1.0, dotProduct);
    float4 sun = lightColor * sunDisk;

    // Usa el color del cielo definido en el constant buffer
    float4 sky = skyColor;

    // Combina el color del sol con el del cielo para obtener el color final.
    return sky + sun;
}