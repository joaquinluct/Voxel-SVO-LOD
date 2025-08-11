cbuffer MatrixBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix projection;
};
cbuffer LightBuffer : register(b1)
{
    float4 skyColor;
    float4 sunColor;
    float4 lightDirection;
    float4 lightColor;
};
struct VSInput
{
    float3 pos : POSITION;
};

struct VSOutput
{
    float4 pos : SV_POSITION;
    float4 worldPos : TEXCOORD0;
    float4 color : COLOR;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    float4 worldPos = mul(float4(input.pos, 1.0f), world);
    float4 viewPos = mul(worldPos, view);
    output.pos = mul(viewPos, projection);
    output.worldPos = worldPos;
        
    float4 l = normalize(-lightDirection);

    output.color = saturate(l);
    return output;
}

float Random(float2 uv)
{
    // Una semilla simple basada en las coordenadas UV
    float2 seed = uv * 1000.0f;
    
    // Una función de hash simple usando seno y el componente fraccional
    // La multiplicación por un número grande como 12.9898f mejora la distribución
    return frac(sin(dot(seed, float2(12.9898f, 78.233f))) * 43758.5453123f);
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float3 cameraRayDir = normalize(input.worldPos);
    float3 normalizedLightDir = normalize(-lightDirection.xyz);
    
    float dotProduct = dot(cameraRayDir, normalizedLightDir);
    
    float sunY = lightDirection.y;
    
    if (dotProduct > 0.98f && dotProduct < 1.02f)
    {
        //return float4(1.0f, 1.0f, 1.0f, 1.0f); // white color for the sun
        // Calculate the color based on the light direction and sky color
        float3 color = skyColor.rgb * dotProduct;
        // float4 result = sunColor * saturate(dotProduct);
        float sunDisk = smoothstep(0.998, 1.0, dotProduct);
        float4 result = lerp(skyColor, sunColor, sunDisk);
        return result;
    }
    
    float ceilDisk = smoothstep(-1, 1.0, dotProduct);
    float lightIntensity = saturate(ceilDisk + .1f);
    
    float4 finalColor =  skyColor * lightIntensity;
    
    
    float attenuation = 1.0f - saturate(length(input.worldPos) / 1000.0f);

    /*
    if (sunY > .5f)
    {
        float nightDisk = smoothstep(0.5, 1.0, sunY) + .3f;
        return lerp(skyColor, finalColor, nightDisk);
        return finalColor * 0.05f; // Reduce brightness for high sun position
    }
    */
    
        return finalColor;
    
    float sunHigh = smoothstep(-2, 2, dotProduct);
    
    
    return skyColor;
    return float4(0.5, 0.5, 1, 1); // azul medio
}