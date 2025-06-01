// TextShader.hlsl

struct VSINPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
    float4 Color : COLOR; // Color del vértice (color del texto)
};

struct PSINPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0; // Coordenadas de textura del vértice
    float4 Color : COLOR0; // Color del vértice
};

cbuffer MatrixBuffer : register(b1) // <--- ¡Importante! Mantenemos b1 aquí
{
    matrix projectionMatrix; // Esta será tu matriz ortográfica
};

// Declaración de la textura de la fuente y el sampler
Texture2D fontTexture : register(t0); // Asume que la textura está en el slot t0
SamplerState fontSampler : register(s0); // Asume que el sampler está en el slot s0

PSINPUT VSMain(VSINPUT input)
{
    PSINPUT output;
    // Solo necesitamos multiplicar por la matriz de proyección (ortográfica) para la UI
    output.Position = mul(float4(input.Position, 1.0f), projectionMatrix);
    output.TexCoord = input.TexCoord;
    output.Color = input.Color;
    return output;
}

float4 PSMain(PSINPUT input) : SV_TARGET0
{
    // Para ver si el blending funciona, devuelve un color fijo con alfa.
    // return float4(1.0f, 0.0f, 0.0f, 0.5f); // Rojo semi-transparente
    // Si ves rectángulos rojos, el blending base funciona.

    // Para ver las coordenadas de textura
    // return float4(input.TexCoord.x, input.TexCoord.y, 0.0f, 1.0f); // X como rojo, Y como verde
    // Si ves un degradado de color en cada "cuadrado de carácter", las TexCoords están llegando.

    // Si tienes un problema con el Sampler o la Textura
    // return fontTexture.Sample(fontSampler, input.TexCoord); // Ver si la textura se muestra directamente
    
    // Muestra la textura de la fuente en las coordenadas de textura interpoladas
    // Dado que DejaVuSansMono.jpg es un atlas en blanco y negro, el canal 'r' (rojo)
    // suele contener la información de luminancia/opacidad de los caracteres.
    
    float alpha = fontTexture.Sample(fontSampler, input.TexCoord).r; //

    // Multiplica el color del texto por el valor alfa obtenido de la textura.
    // Esto hará que las partes negras de la textura (donde no hay carácter) sean transparentes
    // y las partes blancas (el carácter en sí) tengan la opacidad total del color del texto.
    
    return float4(input.Color.rgb, input.Color.a * alpha);
}