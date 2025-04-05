//#define MaxLights
//#define NUM_DIR_LIGHTS 
//#define NUM_POINT_LIGHTS
//#define NUM_SPOT_LIGHTS

#include "Includes/Math.hlsl"

struct Light
{
	float3 strength;
	float falloffStart;
	float3 direction;
	float falloffEnd;
	float3 position;
	float spotPower;
};

struct Material
{
	float4 diffuseAlbedo;
	float3 fresnelR0;
	float roughness;
};

float LinearLerpBetween(float d, float falloffStart, float falloffEnd)
{
	return saturate((falloffEnd - d) / (falloffEnd - falloffStart));
}

float ComputeGGXNDF(float3 normal, float3 h, float alpha)
{
	float a2 = alpha * alpha;
	a2 *= a2;
	float NdotH = max(dot(normal, h), 0.0);
	float NdotH2 = NdotH * NdotH;
	
	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

float ComputeSchlickGGXGeometry(float dotResult, float k)
{
	float nom = dotResult;
	float denom = dotResult * (1 - k) + k;
	return nom / denom;
}

float ComputeSmithGeometry(float3 normal, float3 lightDir, float3 viewDir, float alpha)
{
	float k = (alpha + 1);
	k = k * k / 8;
	float NdotV = saturate(dot(normal, viewDir));
	float NdotL = saturate(dot(normal, lightDir));
	float ggx1 = ComputeSchlickGGXGeometry(NdotV, k);
	float ggx2 = ComputeSchlickGGXGeometry(NdotL, k);
	return ggx1 * ggx2;
}

float3 ComputeSchlickFresnel(float3 f0, float3 normal, float3 viewDir)
{
	float cos_theta = saturate(dot(normal, viewDir));
	float k = pow(1 - cos_theta, 5);
	return f0 + (1 - f0) * k;
}

float3 ComputeDirectionalLighting(Light light, Material mat, 
					float3 pos, float3 normal, float3 toEye)
{
	float3 result = 0.0f;
	
	float3 lightDir = normalize(-light.direction);
	
	float3 h = normalize(lightDir + toEye);
	float3 f0 = lerp(float3(mat.fresnelR0), mat.diffuseAlbedo.xyz, (1 - mat.roughness).xxx);
	float3 oneMinusF0 = 1 - f0;
	
	float3 L = light.strength * saturate(dot(normal, lightDir));
	
	float N = ComputeGGXNDF(normal, h, mat.roughness);
	float G = ComputeSmithGeometry(normal, lightDir, toEye, mat.roughness);
	float3 F = ComputeSchlickFresnel(f0, h, toEye);
	
	float denom = 4 * saturate(dot(normal, toEye)) * saturate(dot(normal, lightDir)) + 0.001;

	float3 specular = N * G * F / denom;
	float3 diffuse = mat.diffuseAlbedo.xyz / PI;
	
	float3 kS = F;
	float3 kD = (1 - F) * mat.roughness;
	
	result = (specular + kD * diffuse) * L;
	
	return result;
}

float3 ComputePointLighting(Light light, Material mat,
					float3 pos, float3 normal, float3 toEye)
{
	float3 result = 0.0f;
	
	float3 lightVec = light.position - pos;
	float d = length(lightVec);
	
	if(d > light.falloffEnd)
		return 0.0f;
	
	float attenuation = LinearLerpBetween(d, light.falloffStart, light.falloffEnd);
	light.strength *= attenuation;
	
	light.direction = -lightVec / d;
	
	result = ComputeDirectionalLighting(light, mat, pos, normal, toEye);
	
	return result;
}

float3 ComputeSpotLighting(Light light, Material mat,
					float3 pos, float3 normal, float3 toEye)
{
	float3 result = 0.0f;
	
	float3 lightVec = light.position - pos;
	float d = length(lightVec);
	
	float3 lightDir = lightVec / d;
	
	if (d > light.falloffEnd)
		return 0.0f;
	
	float attenuation = LinearLerpBetween(d, light.falloffStart, light.falloffEnd);
	light.strength *= attenuation;
	float spotFactor = pow(max(dot(-lightDir, normalize(light.direction)), 0.0f), light.spotPower);
	light.strength *= spotFactor;
	
	light.direction = -lightDir;
	result = ComputeDirectionalLighting(light, mat, pos, normal, toEye);

	result *= attenuation;
	
	return result;
}