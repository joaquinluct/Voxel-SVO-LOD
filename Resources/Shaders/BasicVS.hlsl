cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

struct VertexInput
{
    float3 position : POSITION;
};

struct PixelInput
{
    float4 position : SV_POSITION;
};

PixelInput main(VertexInput input)
{
    PixelInput output;

    // Transformamos la posición del vértice
    float4 worldPos = mul(float4(input.position, 1.0f), worldMatrix);
    float4 viewPos = mul(worldPos, viewMatrix);
    output.position = mul(viewPos, projectionMatrix);

    return output;
}
