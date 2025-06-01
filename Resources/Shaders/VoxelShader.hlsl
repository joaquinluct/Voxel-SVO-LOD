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
    float3 normal : NORMAL; // Asegúrate de que esto coincide con tu MarchingCubesVertex
    float4 color : COLOR; // Asegúrate de que esto coincide
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

    // Transforma la posición del vértice del espacio local al espacio de la pantalla
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
    // return input.color; // Devuelve el color del vértice directamente
    // Normalizar la normal para asegurarte de que esté en el rango correcto
    float3 normal = normalize(input.normal);

    // Si la normal apunta hacia arriba (eje Y positivo), pinta de verde
    
    if (normal.y > 0.9f) // Umbral para considerar "hacia arriba"
    {
        return float4(0.0f, 1.0f, 0.0f, 1.0f); // Verde
    }
    else
    {
        return float4(0.55f, 0.27f, 0.07f, 1.0f); // Marrón (RGB aproximado para marrón)
    }
    
}