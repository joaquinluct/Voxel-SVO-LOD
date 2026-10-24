// TextSDF.hlsl
// Signed Distance Field text shader (simple SDF smoothstep with optional outline)

struct VSINPUT
{
    float3 Position : POSITION;
    float2 TexCoord : TEXCOORD;
    float4 Color : COLOR; // vertex color (text color)
};

struct PSINPUT
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD0;
    float4 Color : COLOR0;
};

cbuffer MatrixBuffer : register(b13)
{
    matrix projectionMatrix; // orthographic projection
}

// Parameters for SDF rendering (edge threshold, outline width, smoothness)
cbuffer SDFParams : register(b14)
{
    float edge;        // contour center (usually 0.5)
    float outlineWidth; // width in texture space to consider outline (approx)
    float smoothness;   // multiplier for fwidth to control AA
    float pad;
}

// Texture and sampler bindings must match code expectations
Texture2D fontTexture : register(t7);
SamplerState fontSampler : register(s3);

PSINPUT VSMain(VSINPUT input)
{
    PSINPUT o;
    o.Position = mul(float4(input.Position, 1.0f), projectionMatrix);
    o.Position.w = 1.0f;
    o.TexCoord = input.TexCoord;
    o.Color = input.Color;
    return o;
}

// Utility: convert sampled value to signed distance assuming texture encodes distance in red channel
// and the glyph contour is at 0.5 (common SDF packing). We rely on automatic derivatives for smoothing.

float4 PSMain(PSINPUT input) : SV_TARGET0
{
    // Sample SDF value
    float sdf = fontTexture.Sample(fontSampler, input.TexCoord).r;

    // Estimate smoothing width in texture space using derivatives
    float width = fwidth(sdf);
    // Avoid zero width
    width = max(width, 1e-6);

    // Center of contour is 0.5 in many SDFs; compute alpha via smoothstep
    float edge = 0.5;
    float alpha = smoothstep(edge - width * 0.5, edge + width * 0.5, sdf);

    // Optional simple outline: sample neighboring pixels to detect ink and expand
    // For a cheap outline, we can compute an outer mask using a slightly larger threshold
    float outline = smoothstep(edge - width * 1.0, edge - width * 0.25, sdf);

    // Compose final color: outline (black) behind text color
    float3 textCol = input.Color.rgb;
    float3 outlineCol = float3(0.0, 0.0, 0.0);

    // Mix outline and text color: outline only where outline present and text not present
    float3 color = lerp(input.Color.rgb, outlineCol, outline * (1.0 - alpha));

    return float4(color, input.Color.a * alpha);
}
