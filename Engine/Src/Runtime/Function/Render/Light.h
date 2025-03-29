#pragma once
#include <DirectXMath.h>
#include "Core/Math/Vector3.h"

namespace photon 
{
	struct Light
	{
		Vector3 strength = { 0.5f, 0.5f, 0.5f }; // 颜色
		float falloffStart = 1.0f;
		Vector3 direction = { 0.0f, -1.0f, 0.0f };
		float falloffEnd = 10.0f;
		Vector3 position = { 0.0f, 0.0f, 0.0f };
		float spotPower = 64.0f;
	};

#define MaxLights 16
#define MaxDirLights 4
#define MaxPointLights 6 
#define MaxSpotLights 6 


	inline Light CreateDirectionalLight(Vector3 strength, Vector3 dir)
	{
		Light ret;
		ret.strength = strength;
		ret.direction = dir;
		return ret;
	}

	inline Light CreatePointLight(Vector3 strength, Vector3 position, Vector3 dir, float falloffStart, float falloffEnd)
	{
		Light ret;
		ret.strength = strength;
		ret.position = position;
		ret.direction = dir;
		ret.falloffStart = falloffStart;
		ret.falloffEnd = falloffEnd;
		return ret;
	}

	inline Light CreateSpotLight(Vector3 strength, Vector3 position, Vector3 dir, float falloffStart, float falloffEnd, float spotPower)
	{
		Light ret;
		ret.strength = strength;
		ret.position = position;
		ret.direction = dir;
		ret.falloffStart = falloffStart;
		ret.falloffEnd = falloffEnd;
		ret.spotPower = spotPower;
		return ret;
	}
}