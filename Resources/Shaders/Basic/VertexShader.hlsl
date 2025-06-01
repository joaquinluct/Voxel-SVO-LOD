struct VS_INPUT
{
    float4 pos : POSITION;
    float4 color : COLOR;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

cbuffer MatrixBuffer
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};


VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;

    // Aplicar transformación completa
    output.pos = mul(input.pos, worldMatrix);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);

    output.color = input.color;

    // Si el vértice está detrás de la cámara (z < 0 en espacio de clip)
    if (output.pos.z < 0.0f)
    {
        output.color = float4(1, 1, 0, 1); // Amarillo
    }

    return output;
}