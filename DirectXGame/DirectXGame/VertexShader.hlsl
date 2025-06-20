struct VS_INPUT
{
    float3 position : POSITION;
    float3 color : COLOR;
    float3 color1 : COLOR1;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 color : COLOR;
    float3 color1 : COLOR1;
};

cbuffer constant : register(b0)
{
    row_major float4x4 m_world;
    row_major float4x4 m_view;
    row_major float4x4 m_proj;
    unsigned int m_time;
};

VS_OUTPUT vsmain(VS_INPUT input)
{
    VS_OUTPUT output;
    float4 pos = float4(input.position, 1.0f);
    pos = mul(pos, m_world);
    pos = mul(pos, m_view);
    pos = mul(pos, m_proj);
    output.position = pos;
    output.color = input.color;
    output.color1 = input.color1;
    return output;
}
