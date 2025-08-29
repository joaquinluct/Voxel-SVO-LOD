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

cbuffer MatrixBuffer : register(b13)
{
    matrix projectionMatrix; // Esta será tu matriz ortográfica
};

// Declaración de la textura de la fuente y el sampler
Texture2D fontTexture : register(t7); // Asume que la textura está en el slot t0
SamplerState fontSampler : register(s3); // Asume que el sampler está en el slot s0

PSINPUT VSMain(VSINPUT input)
{
    PSINPUT output;
    // Solo necesitamos multiplicar por la matriz de proyección (ortográfica) para la UI
    output.Position = mul(float4(input.Position, 1.0f), projectionMatrix);
    output.Position.w = 1.0f; // Asegúrate de que la componente w sea 1 para el clip space
    // output.Position = float4(input.Position, 1.0f);
    output.TexCoord = input.TexCoord;
    output.Color = input.Color;
    return output;
}

float4 PSMain(PSINPUT input) : SV_TARGET0
{
// Código de txto con borde
    
    // Primer pase: El borde
    // Para renderizar un borde, necesitas un valor de alfa adicional.
    // Una forma simple es muestrear la misma textura en el mismo punto,
    // pero con un umbral diferente para el alfa.
    /*
    float alpha = fontTexture.Sample(fontSampler, input.TexCoord).r;
    float borderAlpha = fontTexture.Sample(fontSampler, input.TexCoord).r;
    float borderThreshold = 0.2f;
    float isBorder = step(borderThreshold, borderAlpha);
    float isText = step(0.6f, alpha); // Umbral para el texto principal (un poco más alto)
    float finalAlpha = isText + isBorder * (1.0f - isText);
    float4 borderColor = float4(0.0f, 0.0f, 0.0f, 1.0f); // Negro
    float4 finalColor = lerp(borderColor, input.Color, isText);    
    return float4(finalColor.rgb, finalColor.a * finalAlpha);
    */
    
    
// Código original comentado para depuración y pruebas
    
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