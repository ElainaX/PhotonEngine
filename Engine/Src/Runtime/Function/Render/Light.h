#pragma once
#include <DirectXMath.h>
#include "Core/Math/Vector3.h"
#include "GameObject.h"

namespace photon 
{
	struct LightData
	{
		Vector3 strength = { 0.5f, 0.5f, 0.5f }; // 颜色
		float falloffStart = 1.0f;
		Vector3 direction = { 0.0f, -1.0f, 0.0f };
		float falloffEnd = 10.0f;
		Vector3 position = { 0.0f, 0.0f, 0.0f };
		float spotPower = 64.0f;
	};

	class DirLight : public GameObject
	{
	public:
		DirLight(){
			GameObjectName = GetGameObjectType();
		}
		DirLight(LightData _data)
			: data(_data) {
			GameObjectName = GetGameObjectType();
		}

		LightData data;

		std::string GetGameObjectType() override
		{
			return "DirLight";
		}
	};

	class PointLight : public GameObject
	{
	public:
		PointLight() { GameObjectName = GetGameObjectType(); }
		PointLight(LightData _data)
			: data(_data) {
			GameObjectName = GetGameObjectType();
		}

		LightData data;

		std::string GetGameObjectType() override
		{
			return "PointLight";
		}
	};

	class SpotLight : public GameObject
	{
	public:
		SpotLight() { GameObjectName = GetGameObjectType(); }
		SpotLight(LightData _data)
			: data(_data) {
			GameObjectName = GetGameObjectType();
		}

		LightData data;

		std::string GetGameObjectType() override
		{
			return "SpotLight";
		}
	};

#define MaxLights 16
#define MaxDirLights 4
#define MaxPointLights 6 
#define MaxSpotLights 6 


	inline DirLight CreateDirectionalLight(Vector3 strength, Vector3 dir)
	{
		LightData ret;
		ret.strength = strength;
		ret.direction = dir;
		return ret;
	}

	inline PointLight CreatePointLight(Vector3 strength, Vector3 position, Vector3 dir, float falloffStart, float falloffEnd)
	{
		LightData ret;
		ret.strength = strength;
		ret.position = position;
		ret.direction = dir;
		ret.falloffStart = falloffStart;
		ret.falloffEnd = falloffEnd;
		return ret;
	}

	inline SpotLight CreateSpotLight(Vector3 strength, Vector3 position, Vector3 dir, float falloffStart, float falloffEnd, float spotPower)
	{
		LightData ret;
		ret.strength = strength;
		ret.position = position;
		ret.direction = dir;
		ret.falloffStart = falloffStart;
		ret.falloffEnd = falloffEnd;
		ret.spotPower = spotPower;
		return ret;
	}
}