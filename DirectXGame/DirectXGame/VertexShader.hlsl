struct VS_INPUT
{
	float4 position : POSITION;
	float3 color: COLOR;
};

struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float3 color: COLOR;
};

VS_OUTPUT vsmain( VS_INPUT input )
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.position = input.position;
	output.color = input.color;

	/*if (position.y > 0 && position.y < 1)
	{
		position.x += 0.25f;
	}

	if (position.y > 0 && position.y < 1 && position.x > -1 && position.x < 0)
	{
		position.y -= 0.25f;
	}*/

	return output;
}

//float psmain(float4 pos : SV_Position) : SV_Target
//{
//    return float4(0.8f, 0.9f, 0.4f, 1.0f);
//}