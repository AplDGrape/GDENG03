cbuffer PostProcessBuffer : register(b0)
{
    float2 resolution;
    float chromaAmount;
    float vignetteStrength;
    float vignetteRadius;
    float vignetteSmoothness;
    float2 padding;
};

Texture2D sceneTex : register(t0);
SamplerState samplerState : register(s0);

float4 main(float4 pos : SV_POSITION, float2 uv : TEXCOORD) : SV_Target
{
    // Existing chromatic aberration code
    float2 offset = chromaAmount * float2(1.0 / 800.0, 1.0 / 600.0);
    float3 baseColor = sceneTex.Sample(samplerState, uv).rgb;
    
    float r = sceneTex.Sample(samplerState, uv - offset * 1.5).r;
    float g = baseColor.g;
    float b = sceneTex.Sample(samplerState, uv + offset * 1.5).b;
    
    float3 chromaColor = float3(r, g, b);
    
    // New vignette effect
    float2 center = float2(0.5, 0.5);
    float2 uv_centered = uv - center;
    
    // Calculate distance from center, accounting for aspect ratio
    float aspectRatio = resolution.x / resolution.y;
    uv_centered.x *= aspectRatio;
    float distance = length(uv_centered);
    
    // Create vignette mask
    float vignette = 1.0 - smoothstep(vignetteRadius - vignetteSmoothness,
                                      vignetteRadius + vignetteSmoothness,
                                      distance);
    
    // Apply vignette strength
    vignette = lerp(1.0, vignette, vignetteStrength);
    
    // Combine chromatic aberration with vignette
    float3 finalColor = chromaColor * vignette;
    
    return float4(finalColor, 1.0f);
}
