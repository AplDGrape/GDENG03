cbuffer PostProcessBuffer : register(b0)
{
    float2 resolution;
    float chromaAmount;
    float padding; // align to 16 bytes if needed
};

Texture2D sceneTex : register(t0);
SamplerState samplerState : register(s0);

float4 main(float4 pos : SV_POSITION, float2 uv : TEXCOORD) : SV_Target
{
    float2 offset = chromaAmount * float2(1.0 / 800.0, 1.0 / 600.0); // adjust for resolution
    float3 baseColor = sceneTex.Sample(samplerState, uv).rgb;

    // Chromatic aberration simulation
    float r = sceneTex.Sample(samplerState, uv - offset * 1.5).r;
    float g = baseColor.g;
    float b = sceneTex.Sample(samplerState, uv + offset * 1.5).b;

    return float4(r, g, b, 1.0f);
}