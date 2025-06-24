// VoxelShader.hlsl (o el nombre de tu shader para los chunks)

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInputType
{
    float3 position : POSITION;
    float3 normal : NORMAL; // Aseg�rate de que esto coincide con tu MarchingCubesVertex
    float4 color : COLOR; // Aseg�rate de que esto coincide
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float3 worldPosition : TEXCOORD0; // Necesitamos la posición en el espacio mundial
};

PixelInputType VSMain(VertexInputType input)
{
    PixelInputType output;

    // Transforma la posici�n del v�rtice del espacio local al espacio de la pantalla
    // Multiplica en el orden correcto: World * View * Projection
    float4 worldPosition = mul(float4(input.position, 1.0f), worldMatrix);
    // output.position = mul(worldPosition, viewMatrix);
    // output.position = mul(worldPosition, projectionMatrix);
    
    output.position = mul(float4(input.position, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Pasa la normal y el color al pixel shader
    output.normal = normalize(mul((float3x3) worldMatrix, input.normal));
    output.color = input.color;
    
    output.worldPosition = worldPosition.xyz; // Pasa la posición en el espacio mundial

    return output;
}

float4 PSMain(PixelInputType input) : SV_TARGET
{
    // // Normalizar la normal para asegurarte de que esté en el rango correcto
    float3 normal = normalize(input.normal);
     
    // Calcular un color basado en la posición Y en el espacio mundial
    // Ajusta estos valores para controlar dónde empieza y termina el gradiente
    float gradientStart = 300.0f; // Altura donde empieza el marron
    float gradientEnd = 1100.0f; // Altura donde empieza el verde

    // Calcula el factor de interpolación basado en la posición Y en el espacio mundial
    float gradientFactor = saturate((input.worldPosition.y - gradientStart) / (gradientEnd - gradientStart));

    // Interpola entre marrn y verde
    float4 baseColor = lerp(float4(0.55f, 0.27f, 0.07f, 1.0f), float4(0.0f, 1.0f, 0.0f, 1.0f), gradientFactor); // Marrón a verde

    return baseColor;
    
    // Calcular un color basado en la posición Y en el espacio mundial
    // float gradientFactor = saturate((input.position.y + 10.0f) / 110.0f); // Ajusta estos valores
    // float4 baseColor = lerp(float4(0.55f, 0.27f, 0.07f, 1.0f), float4(0.0f, 1.0f, 0.0f, 1.0f), gradientFactor); // Marrón a verde

    // return baseColor;
    
    
    if (normal.y < 0.0f)
    {
        if (normal.y < -0.9f)
        {
            float gradientFactor = saturate((input.position.y + 10.0f)); // Ajusta estos valores
            float4 baseColor = lerp(float4(0.55f, 0.27f, 0.07f, 1.0f), float4(0.0f, 1.0f, 0.0f, 1.0f), gradientFactor); // Marrón a verde
            // return float4(0.0f, 1.4f, 0.2f, 1.0f); // Verde
        }
        else if (normal.y < -0.5f)
        {
            return float4(0.55f, 0.27f, 0.07f, 1.0f); // Marrón
        }
        else if (normal.y < -0.2f)
        {
            return float4(0.8f, 0.52f, 0.25f, 1.0f); // Marrón claro
        }
        else if (normal.y < 0.2f)
        {
            return float4(1.0f, 1.0f, 1.0f, 1.0f); // Blanco
        }
        else if (normal.y < 0.5f)
        {
            return float4(1.0f, 1.0f, 1.0f, 1.0f); // Blanco
        }
        else if (normal.y < 0.9f)
        {
            return float4(200.0f, 200.0f, 400.0f, 1.0f); // Gris claro
        }
        return float4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else
    {
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }
    
    // return input.color; // Devuelve el color del v�rtice directamente
    // Normalizar la normal para asegurarte de que est� en el rango correcto    
    if (normal.y > 0.0f)
    {
        return float4(0.0f, 1.0f, 0.0f, 1.0f); // Verde
    }
    // De lo contrario, la cara mira hacia abajo o es completamente horizontal (normal.y <= 0.0f).
    // Si quieres que las caras perfectamente horizontales (normal.y == 0.0f) sean verdes,
    // cambia la condici�n a `if (normal.y >= 0.0f)`.
    else
    {
        return float4(0.55f, 0.27f, 0.07f, 1.0f); // Marrón
    }

    // Verde si la normal apunta hacia arriba
    if (normal.y > 0.9f)
    {
        return float4(0.0f, 1.0f, 0.0f, 1.0f); // Verde
    }
    // Azul si la normal apunta hacia abajo
    else if (normal.y < -0.9f)
    {
        return float4(0.0f, 0.4f, 1.0f, 1.0f); // Azul
    }
    // Marrón claro si la normal es casi lateral
    else if (abs(normal.y) < 0.2f)
    {
        return float4(0.55f, 0.27f, 0.07f, 1.0f); // Marrón claro
    }
    // Marrón oscuro para caras oblicuas
    else
    {
        return float4(0.0f, 0.8f, 0.0f, 1.0f); // Verde
    }
    
}