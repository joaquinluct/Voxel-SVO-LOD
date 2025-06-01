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
};

PixelInputType VSMain(VertexInputType input)
{
    PixelInputType output;

    // Transforma la posici�n del v�rtice del espacio local al espacio de la pantalla
    // Multiplica en el orden correcto: World * View * Projection
    output.position = mul(float4(input.position, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Pasa la normal y el color al pixel shader
    output.normal = mul((float3x3) worldMatrix, input.normal);
    output.color = input.color;

    return output;
}

float4 PSMain(PixelInputType input) : SV_TARGET
{
    // return input.color; // Devuelve el color del v�rtice directamente
    // Normalizar la normal para asegurarte de que est� en el rango correcto
    float3 normal = normalize(input.normal);

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