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
	
#ifdef MaxLights
	Light gLights[MaxLights];
#endif
}

cbuffer cbMaterial : register(b2)
{
	float4 gDiffuseAlbedo;
	float3 gFresnelR0;
	float gRoughness;
	float4x4 gMatTransform;
	bool gHasDiffuse;
	bool gHasNormal;
	bool gHasRoughness;
	bool gInverseRoughness;
};

Texture2D gDiffuseMap : register(t0);
Texture2D gNormalMap : register(t1);
Texture2D gRoughnessMap : register(t2);



VertexOutput VS(VertexInput vin)
{
	VertexOutput vout;
	vout.posWorld = mul(gWorld, float4(vin.pos, 1.0));
	vout.posClip = mul(gProjView, vout.posWorld);
	
	float3x3 invWorld = transpose((float3x3) gWorld);
	
	vout.normalWorld = mul(invWorld, vin.normal);
	vout.tangentWorld = mul((float3x3)gWorld, vin.tangent);
	vout.bitangentWorld = normalize(cross(vout.normalWorld, vout.tangentWorld));

	
	vout.texcoord = vin.texcoord;
	
	return vout;
}

float4 PS(VertexOutput pin) : SV_Target
{
	Material mat;
	
	float3 normalW = normalize(pin.normalWorld);
	float roughness = gRoughness;
	
	if(gHasDiffuse)
		mat.diffuseAlbedo = gDiffuseMap.Sample(g_LinearWrapSampler, pin.texcoord);
	
	
#ifdef EnableNormalMap
	if(gHasNormal)
	{
		float3x3 TBN_W = transpose(float3x3(normalize(pin.tangentWorld),
									normalize(pin.bitangentWorld),
									normalize(pin.normalWorld)));
		normalW = normalize(gNormalMap.Sample(g_LinearWrapSampler, pin.texcoord).xyz * 0.5 + 0.5);
		normalW = mul(TBN_W, normalW);
	}
#endif

#ifdef EnableRoughnessMap	
	if(gHasRoughness)
	{
		roughness = gRoughnessMap.Sample(g_LinearWrapSampler, pin.texcoord).x;
	}
#endif	
	
	if(gInverseRoughness)
	{
		roughness = 1 - roughness;
	}



	mat.fresnelR0 = lerp(mat.diffuseAlbedo.xyz, gFresnelR0, roughness);
	mat.roughness = roughness;
	
	float3 result = gAmbientLight;
	float3 viewDir = normalize(gEyePos - pin.posWorld.xyz);
	
	int index = 0;
	
	//return float4(normalize(pin.normalWorld) / 2 + 0.5, 1.0);
	
#ifdef NumDirLights
	for (int i = 0; i < NumDirLights; ++i)
	{
		Light dirLight = gLights[index];
		float3 L_dir = ComputeDirectionalLighting(dirLight, mat, pin.posWorld.xyz, normalW, viewDir);
		result += L_dir;
		index++;
	}
#endif
	
	
#ifdef NumPointLights
	for (int i = 0; i < NumPointLights; ++i)
	{
		Light pointLight = gLights[index];
		float3 L_point = ComputePointLighting(pointLight, mat, pin.posWorld.xyz, normalW, viewDir);
		result += L_point;
		index++;
	}
#endif


	
#ifdef NumSpotLights
	for (int i = 0; i < NumSpotLights; ++i)
	{
		Light spotLight = gLights[index];
		float3 L_spot = ComputeSpotLighting(spotLight, mat, pin.posWorld.xyz, normalW, viewDir);
		result += L_spot;
		index++;
	}
#endif
	
	result = pow(result, 1.0/2.2);
	
	//return float4(1.0, 0.0, 0.0, 1.0);
	return float4(result, 1.0f);
}