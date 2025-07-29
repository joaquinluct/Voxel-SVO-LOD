// ShadowMapShader.hlsl

// Constant Buffer para las matrices de la luz
cbuffer LightSpaceMatrices : register(b0)
{
    float4x4 worldMatrix; // Matriz de mundo del objeto
    float4x4 lightViewProjectionMatrix; // Matriz (View de Luz * Proyección de Luz)
};

// Estructura de entrada del Vertex Shader
// Debe coincidir con el InputLayout que usas para el pase de sombras.
// Si tus modelos tienen normales, texcoords, tangentes, etc., deben estar aquí
// para que el InputLayout sea compatible, aunque no las uses en el VS.
struct VSInput
{
    float3 pos : POSITION;
};

// Estructura de salida del Vertex Shader
// Solo necesitamos la posición en el espacio de clip.
struct VSOutput
{
    float4 pos : SV_POSITION; // Posición en el espacio de clip
};

// Vertex Shader para el pase de profundidad
VSOutput VSMain(VSInput input)
{
    VSOutput output;

    // Transformar la posición del vértice al espacio mundo
    float4 worldPos = mul(float4(input.pos, 1.0f), worldMatrix);

    // Transformar la posición del vértice al espacio de clip de la luz
    output.pos = mul(worldPos, lightViewProjectionMatrix);

    return output;
}

// Pixel Shader para el pase de profundidad (Opcional)
// Este Pixel Shader no hace nada, solo devuelve un color dummy.
// La profundidad se escribe automáticamente por el rasterizador.
// Puedes usar un Pixel Shader nulo si tu hardware lo permite y solo necesitas escribir profundidad.
float4 PSMain() : SV_TARGET
{
    return float4(0.0f, 0.0f, 0.0f, 0.0f); // Color transparente/negro (será ignorado)
}