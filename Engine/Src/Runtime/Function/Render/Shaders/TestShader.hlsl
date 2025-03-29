// VertexShader

#include "Includes/VertexSimple.hlsl"
#include "Includes/SamplerStatic.hlsl"
#include "Includes/SimpleLight.hlsl"

struct VertexOutput
{
	float4 posWorld : POSITIONW;
	float4 posClip : SV_POSITION;
	
	float3 normalWorld : NORMALW;
	float3 tangentWorld : TANGENTW;
	float3 bitangentWorld : BITANGENTW;
		
	
	float2 texcoord : TEX;
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
	
//#ifdef MaxLights
//	Light gLights[MaxLights];
//#endif
}

cbuffer cbMaterial : register(b2)
{
	float4 gDiffuseAlbedo;
	float3 gFresnelR0;
	float gRoughness;
	float4x4 gMatTransform;
};

Texture2D gDiffuseMap : register(t0);



VertexOutput VS(VertexInput vin)
{
	VertexOutput vout;
	vout.posWorld = mul(gWorld, float4(vin.pos, 1.0));
	vout.posClip = mul(gProjView, vout.posWorld);
	
	float3x3 invWorld = transpose((float3x3) gWorld);
	vout.normalWorld = mul(invWorld, vin.normal);
	vout.tangentWorld = mul(invWorld, vin.tangent);
	vout.bitangentWorld = normalize(cross(vout.normalWorld, vout.tangentWorld));
	
	vout.texcoord = vin.texcoord;
	
	return vout;
}

float4 PS(VertexOutput pin) : SV_Target
{
	Material mat;
	mat.diffuseAlbedo = gDiffuseAlbedo * gDiffuseMap.Sample(g_PointWrapSampler, pin.texcoord);
	mat.fresnelR0 = gFresnelR0;
	mat.roughness = gRoughness;
	
	//float3 result = 5*gAmbientLight;
	float3 viewDir = normalize(gEyePos - pin.posWorld.xyz);
#ifdef NumDirLights
	for (int i = 0; i < NumDirLights; ++i)
	{
		Light dirLight = gLights[i];
		float3 L_dir = ComputeDirectionalLighting(dirLight, mat, pin.posWorld.xyz, pin.normalWorld, viewDir);
		result += L_dir;
	}
#endif
	return float4(1.0, 0.0, 0.0, 1.0);
	//return float4(result, 1.0f);
}