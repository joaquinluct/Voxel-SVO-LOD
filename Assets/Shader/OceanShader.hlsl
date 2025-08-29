//--------------------------------------------------------------------------------------
// Buffers de constantes estándar
//--------------------------------------------------------------------------------------
cbuffer MatrixBuffer : register(b0)
{
    float4x4 worlRealMatrix;
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
};

cbuffer CameraData : register(b1)
{
    float3 cameraPosition;
};

cbuffer DirectionalLight : register(b2)
{
    float3 lightDirection;
    float4 lightColor;
};

// Nuevo: Buffer de datos del agua
cbuffer WaterBuffer : register(b7)
{
    float waveHeight;
    float waveSpeed;
    float waveLength;
    float waveFrequency;
    float waterDensity;
    float waterViscosity;
    float reflectionIntensity;
    float refractionIndex;
    float foamAmount;
    float causticsIntensity;
    float4 waterColor;
    bool isUnderwater;
    bool hasFoam;
    bool hasCaustics;
    bool isReflective;
    bool isRefractive;
};

cbuffer TimeBuffer : register(b8)
{
    float time;
};

// Nuevo buffer de constantes para las matrices de instancia
cbuffer InstanceBuffer : register(b9)
{
    float4x4 instanceWorldMatrix[256]; // Array de matrices de transformación
};

// Textures
//Texture2D waterTexture : register(t0);
//SamplerState waterSampler : register(s0);

// Vertex and Pixel Shaders Input/Output
struct VSInput
{
    float3 pos : POSITION;
    uint instanceID : SV_InstanceID; // Nuevo: ID de la instancia
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float3 worldPos : WORLDPOS;    
    float3 normal : NORMAL;
};

// Vertex Shader
PSInput VSMain(VSInput input)
{
    PSInput output;

    // Obtener la matriz de transformación de esta instancia
    float4x4 worldMatrix = instanceWorldMatrix[input.instanceID];

    // Aplicar deformación de olas (si quieres mantenerla)
    float waveOffset = sin(input.pos.x * waveFrequency + time * waveSpeed) * waveHeight;
    waveOffset += sin(input.pos.z * waveFrequency + time * waveSpeed) * waveHeight;

    float4 localPos = float4(input.pos.x, input.pos.y + waveOffset, input.pos.z, 1.0f);

    // Transformar al espacio mundo
    float4 worldPos = mul(localPos, worldMatrix);
    output.worldPos = worldPos.xyz;

    // Transformar al espacio de cámara y proyectar
    output.pos = mul(projectionMatrix, mul(viewMatrix, worldPos));

    // Calcular normal deformada (opcional)
    float dx = cos(input.pos.x * waveFrequency + time * waveSpeed) * waveHeight * waveFrequency;
    float dz = cos(input.pos.z * waveFrequency + time * waveSpeed) * waveHeight * waveFrequency;
    float3 waveNormal = normalize(float3(-dx, 1.0f, -dz));

    // Transformar la normal al espacio mundo
    output.normal = normalize(mul(waveNormal, (float3x3) worldMatrix));

    return output;
}

// Pixel Shader
float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(0, 0, 1, 1); // Color azul simple para el agua
    
    // Color simple del agua usando el parámetro del buffer
    float3 finalWaterColor = waterColor.rgb;

    // Simulación básica de iluminación
    float3 N = normalize(input.normal);
    float3 L = normalize(-lightDirection);
    float3 V = normalize(cameraPosition - input.worldPos);

    float NdotL = saturate(dot(N, L));
    float3 diffuse = NdotL * finalWaterColor * lightColor.rgb;

    // Especular
    float3 H = normalize(L + V);
    float NdotH = saturate(dot(N, H));
    float specularIntensity = pow(NdotH, 20.0f);
    float3 specular = specularIntensity * float3(1.0f, 1.0f, 1.0f);

    float3 ambient = finalWaterColor * 0.1f;

    float3 finalColor = diffuse + specular + ambient;

    return float4(finalColor, waterColor.a);
}


// ** DEBUG: Se pinta un punto verde en el centro de la pantalla.
// Si ese punto se mantiene en el centro de la pantalla, el problema está en el mesh base. 
// Si no, está en la instancia o la cámara.
//if (input.pos.x == 0 && input.pos.z == 0)
//{
//    output.pos = mul(projectionMatrix, mul(viewMatrix, float4(0, 1, 0, 1)));
//}
// ** Fin DEBUG