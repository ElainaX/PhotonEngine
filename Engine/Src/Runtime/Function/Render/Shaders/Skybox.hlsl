#include "Includes/VertexSimple.hlsl"
#include "Includes/SamplerStatic.hlsl"


struct VertexOutput
{
	float3 sampleVector : SAMPLE_VECTOR;
	float4 positionClip : SV_Position;
};

cbuffer cbObject : register(b0)
{
	float4x4 gWorld;
}
cbuffer cbPass : register(b1)
{
	float4x4 gView;
	float4x4 gInvView;
	float4x4 gProj;
	float4x4 gInvProj;
	float4x4 gProjView;
	float4x4 gInvProjView;
	float3 gEyePos;
	float cbPerObjectPad1;
	float2 gRenderTargetSize;
	float2 gInvRenderTargetSize;
	float gNearZ;
	float gFarZ;
	float gTotalTime;
	float gDeltaTime;
	float4 gAmbientLight;
	
#ifdef MaxLights
	Light gLights[MaxLights];
#endif
}

TextureCube gCubeMap : register(t0);

VertexOutput VS(VertexInput vin)
{
	VertexOutput vout;
	
	float3 posView = mul((float3x3)gView, vin.pos);
	vout.sampleVector = normalize(vin.pos);
	vout.positionClip = mul(gProj, float4(posView, 1.0)).xyzz;
	
	return vout;
}

float4 PS(VertexOutput pin) : SV_Target
{
	return gCubeMap.Sample(g_LinearWrapSampler, normalize(pin.sampleVector));
}
