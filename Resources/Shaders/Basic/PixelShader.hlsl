struct PS_INPUT
{
    float4 color : COLOR;
};

float4 main(PS_INPUT input) : SV_Target
{
    return float4(1, 0, 0, 1); // Rojo sólido
    // return input.color; // Devuelve el color sin modificaciones
}