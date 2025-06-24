// Estructura para los datos de entrada del Vertex Shader
struct VS_INPUT
{
    float3 Position : POSITION; // Posición del vértice
};

// Estructura para los datos de salida del Vertex Shader (y entrada del Pixel Shader)
struct PS_INPUT
{
    float4 Position : SV_POSITION; // Posición del vértice en espacio de clip
    float3 TexCoord : TEXCOORD0;    // Coordenadas de textura
};

// Matriz de vista-proyección (combina la vista de la cámara y la proyección)
float4x4 ViewProjectionMatrix : register(c0);

// Cubemap para la textura del skybox
TextureCube SkyboxTexture : register(t0);
SamplerState SkyboxSampler : register(s0);

// Vertex Shader
PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;

    // Transforma la posición del vértice al espacio de clip.
    // Usamos solo la parte de rotación de la matriz de vista-proyección
    // para que el skybox siempre esté centrado en la cámara.
    output.Position = mul(ViewProjectionMatrix, float4(input.Position, 1.0f));

    // Las coordenadas de textura son simplemente la posición del vértice
    output.TexCoord = input.Position;

    return output;
}

// Pixel Shader
float4 PSMain(PS_INPUT input) : SV_TARGET
{
    // Usa las coordenadas de textura para samplear el cubemap.
    return SkyboxTexture.Sample(SkyboxSampler, input.TexCoord);
}

technique10 SkyboxTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VSMain() ) );
        SetPixelShader( CompileShader( ps_4_0, PSMain() ) );
    }
}