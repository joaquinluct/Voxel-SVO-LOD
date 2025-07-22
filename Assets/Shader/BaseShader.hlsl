cbuffer MatrixBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};

struct VSInput
{
    float3 pos : POSITION;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    float4 worldPos = mul(float4(input.pos, 1.0f), world);
    float4 viewPos = mul(worldPos, view);
    output.pos = mul(viewPos, projection);

    output.color = float4(worldPos.xyz / 300.0f + 0.5f, 1.0f);
    return output;
}
float4 PSMain(VSOutput input) : SV_TARGET
{
    return input.color;
    // return float4(1, 0, 0, 1); // rojo
}