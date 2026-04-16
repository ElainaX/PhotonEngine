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

struct ObjectData
{
	float4x4 gWorld;
};

struct PassData
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

#ifdef MaxCascadedNum
	float2 gShadowMapSize;
	float2 gInvShadowMapSize;
	float4x4 gLightProjViewMatrices[MaxCascadedNum];
	float4 gSpliters[MaxCascadedNum];
#endif
};

struct MaterialData
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

ConstantBuffer<ObjectData> cbObject : register(b0);
ConstantBuffer<PassData> cbPass : register(b1);
ConstantBuffer<MaterialData> cbMaterial : register(b2);


Texture2D gDiffuseMap : register(t0);
Texture2D gNormalMap : register(t1);
Texture2D gRoughnessMap : register(t2);

Texture2DArray<float2> gDepthStencilTextures : register(t3);


float CalcShadowFactor(float3 posWorld);
float ShadowFactorFromShadowMap(Texture2DArray<float2> shadowMap, int slice, float4x4 lightViewProj, float3 posWorld);

//VertexOutput VSMain(VertexInput vin)
//{
//	VertexOutput vout;
//	vout.posWorld = mul(cbObject.gWorld, float4(vin.pos, 1.0));
//	vout.posClip = mul(cbPass.gProjView, vout.posWorld);
//
//	float3x3 invWorld = transpose((float3x3)cbObject.gWorld);
//	
//	vout.normalWorld = mul(invWorld, vin.normal);
//	vout.tangentWorld = mul((float3x3)cbObject.gWorld, vin.tangent);
//	vout.bitangentWorld = normalize(cross(vout.normalWorld, vout.tangentWorld));
//
//	
//	vout.texcoord = vin.texcoord;
//	
//	return vout;
//}
//
//float4 PSMain(VertexOutput pin) : SV_Target
//{
//	Material mat;
//	
//	float3 normalW = normalize(pin.normalWorld);
//	float roughness = cbMaterial.gRoughness;
//	
//	if(cbMaterial.gHasDiffuse)
//		mat.diffuseAlbedo = gDiffuseMap.Sample(g_LinearWrapSampler, pin.texcoord);
//	
//	
//#ifdef EnableNormalMap
//	if(cbMaterial.gHasNormal)
//	{
//		float3x3 TBN_W = transpose(float3x3(normalize(pin.tangentWorld),
//									normalize(pin.bitangentWorld),
//									normalize(pin.normalWorld)));
//		float3 normalTS = gNormalMap.Sample(g_LinearWrapSampler, pin.texcoord).xyz;
//		normalTS = normalTS * 2.0f - 1.0f;
//		normalW = normalize(mul(normalTS, float3x3(
//			normalize(pin.tangentWorld),
//			normalize(pin.bitangentWorld),
//			normalize(pin.normalWorld))));
//	}
//#endif
//
//#ifdef EnableRoughnessMap	
//	if(cbMaterial.gHasRoughness)
//	{
//		roughness = gRoughnessMap.Sample(g_LinearWrapSampler, pin.texcoord).x;
//	}
//#endif	
//	
//	if(cbMaterial.gInverseRoughness)
//	{
//		roughness = 1 - roughness;
//	}
//
//
//
//	mat.fresnelR0 = lerp(mat.diffuseAlbedo.xyz, cbMaterial.gFresnelR0, roughness);
//	mat.roughness = roughness;
//	
//	float3 result = cbPass.gAmbientLight.xyz;
//	float3 viewDir = normalize(cbPass.gEyePos - pin.posWorld.xyz);
//	
//	int index = 0;
//	
//	//return float4(normalize(pin.normalWorld) / 2 + 0.5, 1.0);
//	
////#ifdef NumDirLights
////	for (int i = 0; i < NumDirLights; ++i)
////	{
////		Light dirLight = gLights[index];
////		float3 L_dir = ComputeDirectionalLighting(dirLight, mat, pin.posWorld.xyz, normalW, viewDir);
////		result += L_dir;
////		index++;
////	}
////#endif
////	
////	
////#ifdef NumPointLights
////	for (int i = 0; i < NumPointLights; ++i)
////	{
////		Light pointLight = gLights[index];
////		float3 L_point = ComputePointLighting(pointLight, mat, pin.posWorld.xyz, normalW, viewDir);
////		result += L_point;
////		index++;
////	}
////#endif
////
////
////	
////#ifdef NumSpotLights
////	for (int i = 0; i < NumSpotLights; ++i)
////	{
////		Light spotLight = gLights[index];
////		float3 L_spot = ComputeSpotLighting(spotLight, mat, pin.posWorld.xyz, normalW, viewDir);
////		result += L_spot;
////		index++;
////	}
////#endif
////
////
////#ifdef Shadow
////	float shadowFactor = CalcShadowFactor(pin.posWorld.xyz);
////	result *= shadowFactor;
////#endif
////	
////	result = pow(result, 1.0/2.2);
//	
//	//return float4(1.0, 0.0, 0.0, 1.0);
//	return float4(result, 1.0f);
//}

VertexOutput VSMain(VertexInput vin)
{
	VertexOutput vout;
	vout.posWorld = mul(cbObject.gWorld, float4(vin.pos, 1.0));
	vout.posClip = mul(cbPass.gProjView, vout.posWorld);

	float3x3 invWorld = transpose((float3x3)cbObject.gWorld);
	vout.normalWorld = mul(invWorld, vin.normal);
	vout.tangentWorld = mul((float3x3)cbObject.gWorld, vin.tangent);
	vout.bitangentWorld = normalize(cross(vout.normalWorld, vout.tangentWorld));
	vout.texcoord = vin.texcoord;
	return vout;
}


float4 PSMain(VertexOutput pin) : SV_Target
{
	Material mat;

	float4 baseColor = cbMaterial.gDiffuseAlbedo;

	baseColor = gDiffuseMap.Sample(g_LinearWrapSampler, pin.texcoord);
	mat.diffuseAlbedo = baseColor;

	float3 normalW = normalize(pin.normalWorld);

	return baseColor;

//#ifdef EnableNormalMap
//	if (cbMaterial.gHasNormal)
//	{
//		float3 normalTS = gNormalMap.Sample(g_LinearWrapSampler, pin.texcoord).xyz;
//		normalTS = normalTS * 2.0f - 1.0f;
//
//		float3 T = normalize(pin.tangentWorld);
//		float3 B = normalize(pin.bitangentWorld);
//		float3 N = normalize(pin.normalWorld);
//		float3x3 TBN = float3x3(T, B, N);
//
//		normalW = normalize(mul(normalTS, TBN));
//	}
//#endif
//
//	float roughness = cbMaterial.gRoughness;
//#ifdef EnableRoughnessMap
//	if (cbMaterial.gHasRoughness)
//	{
//		roughness = gRoughnessMap.Sample(g_LinearWrapSampler, pin.texcoord).x;
//	}
//#endif
//
//	if (cbMaterial.gInverseRoughness)
//	{
//		roughness = 1.0f - roughness;
//	}
//
//	mat.fresnelR0 = cbMaterial.gFresnelR0;
//	mat.roughness = roughness;
//
//	float3 result = cbPass.gAmbientLight.xyz;
//	float3 viewDir = normalize(cbPass.gEyePos - pin.posWorld.xyz);
//
//	int index = 0;

//#ifdef NumDirLights
//	for (int i = 0; i < NumDirLights; ++i)
//	{
//		Light dirLight = cbPass.gLights[index];
//		result += ComputeDirectionalLighting(dirLight, mat, pin.posWorld.xyz, normalW, viewDir);
//		index++;
//	}
//#endif
//
//#ifdef NumPointLights
//	for (int i = 0; i < NumPointLights; ++i)
//	{
//		Light pointLight = cbPass.gLights[index];
//		result += ComputePointLighting(pointLight, mat, pin.posWorld.xyz, normalW, viewDir);
//		index++;
//	}
//#endif
//
//#ifdef NumSpotLights
//	for (int i = 0; i < NumSpotLights; ++i)
//	{
//		Light spotLight = cbPass.gLights[index];
//		result += ComputeSpotLighting(spotLight, mat, pin.posWorld.xyz, normalW, viewDir);
//		index++;
//	}
//#endif

	/*result = pow(saturate(result), 1.0 / 2.2);
	return float4(result, 1.0f);*/
}

//float CascadedBlendWeight(float zView, float splitDepth, float fadeWidth)
//{
//	// 在 [splitDepth - fadeWidth, splitDepth + fadeWidth] 内做过渡
//	float t = (zView - (splitDepth - fadeWidth)) / (fadeWidth);
//	return smoothstep(0.0, 1.0, t);
//}
//
//float CalcShadowFactor(float3 posWorld)
//{
//	float factor = 1.0;
//	int c0 = 0;
//	// 首先判断当前片元处于哪个cascaded volume
//	float3 posView = mul(cbPass.gView, float4(posWorld, 1.0)).xyz;
//	//factor = ShadowFactorFromShadowMap(gDepthStencilTextures, 0, gLightProjViewMatrices[0], posWorld);
//	for(int i = 0; i < cbPass.MaxCascadedNum; ++i)
//	{
//		if(posView.z <= cbPass.gSpliters[i].x)
//		{
//			c0 = i;
//			break;
//		}
//	}
//	factor = ShadowFactorFromShadowMap(gDepthStencilTextures, c0, cbPass.gLightProjViewMatrices[c0], posWorld);
//	//return factor;
//	int c1 = min(c0 + 1, cbPass.MaxCascadedNum - 1);
//	if (c0 == c1) return factor;
//
//
//	float w = CascadedBlendWeight(posView.z, cbPass.gSpliters[c0].x, cbPass.gSpliters[c0].x * 0.35);
//	if (w <= 0.0) return factor;
//
//	float factor2 = ShadowFactorFromShadowMap(gDepthStencilTextures, c1, cbPass.gLightProjViewMatrices[c1], posWorld);
//
// 	return lerp(factor, factor2, w);
//}
//
//
//float ShadowFactorFromShadowMap(Texture2DArray<float2> shadowMap, int slice, float4x4 lightViewProj, float3 posWorld)
//{
//	float4 posLightClip = mul(lightViewProj, float4(posWorld, 1.0));
//	float3 posLightNDC = posLightClip.xyz / posLightClip.w;
//	float2 uv = posLightNDC.xy * 0.5 + 0.5;
//	uv.y = 1.0 - uv.y;
//
//
//	// PCF Shadow Factor Calc
//	float sum = 0.0;
//	[unroll] for (int x = -2; x <= 2; ++x)
//		[unroll] for (int y = -2; y <= 2; ++y)
//		{
//		float2 o = float2(x, y) * cbPass.gInvShadowMapSize + uv;
//		float depthInShadowMap = shadowMap.Sample(g_LinearClampSampler, float3(o, slice)).r;
//		sum += posLightNDC.z > depthInShadowMap ? 0.0 : 1.0;
//		}
//
//	return sum / 25.0;
//	
//	
//	//return depthInShadowMap;
//	//if(posLightNDC.z > depthInShadowMap)
//	//	return 0.0;
//	//else
//	//	return 1.0;
//
//	//return depthInShadowMap;
//}