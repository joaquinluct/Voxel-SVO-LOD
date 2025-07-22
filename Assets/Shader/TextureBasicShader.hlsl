 cbuffer MatrixBuffer : register(b0)
{
     matrix world;
     matrix view;
     matrix projection;
};

// 1. Añadimos un registro para la textura (t0) y el sampler (s0)
Texture2D shaderTexture : register(t0);

SamplerState SamplerType : register(s0);

struct VSInput
{
    float3 pos : POSITION;
    float2 tex : TEXCOORD;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float2 tex : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    float4 worldPos = mul(float4(input.pos, 1.0f), world);
    float4 viewPos = mul(worldPos, view);
    output.pos = mul(viewPos, projection);

    output.tex = input.tex;
    return output;
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    // return input.color;
    // 5. Muestrear la textura usando las coordenadas de textura interpoladas
    return shaderTexture.Sample(SamplerType, input.tex);
}