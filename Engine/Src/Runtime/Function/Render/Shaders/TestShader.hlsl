// VertexShader

#include "Includes/VertexSimple.hlsl"

struct VertexOutput
{
	float4 posClip : SV_POSITION;
	float3 color: COLOR;
};

VertexOutput VS(VertexInput vin)
{
	VertexOutput vout;
	
	vout.posClip = float4(vin.pos, 1.0);
	vout.color = float3(1.0, 0.0, 0.0);
	
	
	return vout;
}

float4 PS(VertexOutput pin) : SV_Target
{
#if defined(MEME) && MEME > 4
	return float4(pin.color, 1.0);
#else 
	return float4(pin.color * 0.2, 1.0);
#endif
}