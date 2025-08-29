//--------------------------------------------------------------------------------------
// Constantes del Buffer de Matrices
//--------------------------------------------------------------------------------------
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix; //(ignorado)
    matrix view; // Matriz de la cámara (solo la parte de rotación importa)
    matrix projection; // Matriz de proyección
};

//--------------------------------------------------------------------------------------
// Recursos de Cubemap y Muestreo
//--------------------------------------------------------------------------------------
TextureCube skyboxTexture; // Declaración de un cubemap
SamplerState SampleType; // Muestreador para el cubemap

//--------------------------------------------------------------------------------------
// Estructura de Entrada del Vertex Shader
//--------------------------------------------------------------------------------------
struct VSInput
{
    float4 position : POSITION; // Posición del vértice (de un cubo unitario)
};

//--------------------------------------------------------------------------------------
// Estructura de Salida del Vertex Shader
//--------------------------------------------------------------------------------------
struct PSInput
{
    float4 position : SV_POSITION; // Posición final del vértice en la pantalla
    float3 texCoord : TEXCOORD0; // Vector de dirección para muestrear el cubemap
};

//--------------------------------------------------------------------------------------
// Vertex Shader (VSMain)
//--------------------------------------------------------------------------------------
PSInput VSMain(VSInput input)
{
    PSInput output;

    // Obtener la matriz de vista sin la translación (solo rotación)
    // Se usa la función de HLSL para obtener la matriz de vista sin translación
    matrix viewNoTranslation = view;
    viewNoTranslation[3][0] = 0.0f; // Ignorar X translación
    viewNoTranslation[3][1] = 0.0f; // Ignorar Y translación
    viewNoTranslation[3][2] = 0.0f; // Ignorar Z translación

    // Transformar la posición del vértice al espacio de la pantalla
    output.position = mul(input.position, viewNoTranslation);
    output.position = mul(output.position, projection);

    // El vector de muestreo para el cubemap es la posición original del vértice
    output.texCoord = input.position.xyz;

    return output;
}

//--------------------------------------------------------------------------------------
// Pixel Shader (PSMain)
//--------------------------------------------------------------------------------------
float4 PSMain(PSInput input) : SV_TARGET
{    
    // Muestrear el cubemap usando el vector de dirección interpolado
    float4 skyColor = skyboxTexture.Sample(SampleType, input.texCoord);

    return skyColor;
}