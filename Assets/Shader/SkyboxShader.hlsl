//--------------------------------------------------------------------------------------
// Constantes del Buffer de Matrices
//--------------------------------------------------------------------------------------
cbuffer MatrixBuffer : register(b0)
{
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
    // Esto asegura que el SkyBox se mueva con la cámara pero no se translade
    matrix viewNoTranslation = view;
    viewNoTranslation._41 = 0.0f; // Ignorar X translación
    viewNoTranslation._42 = 0.0f; // Ignorar Y translación
    viewNoTranslation._43 = 0.0f; // Ignorar Z translación

    // Transformar la posición del vértice al espacio de la pantalla
    output.position = mul(input.position, viewNoTranslation);
    output.position = mul(output.position, projection);

    // El vector de muestreo para el cubemap es la posición original del vértice
    // (o su posición en espacio de mundo/ojo después de la rotación).
    // Para un cubo unitario centrado, input.position.xyz es directamente el vector de dirección.
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