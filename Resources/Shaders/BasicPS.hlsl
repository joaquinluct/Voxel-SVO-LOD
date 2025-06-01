struct PixelInput
{
    float4 position : SV_POSITION;
};

float4 main(PixelInput input): SV_Position
{
    return float4(1.0f, 0.0f, 0.0f, 1.0f); // Color rojo sólido
}
