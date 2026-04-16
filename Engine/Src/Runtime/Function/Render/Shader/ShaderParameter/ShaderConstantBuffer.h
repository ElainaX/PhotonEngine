#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include "Define.h"
#include "Core/Math/Vector4.h"
#include "Core/Math/MathFunction.h"
#include "Function/Render/Light.h"
#include "Function/Render/Shader/ShaderEnum.h"

namespace photon
{

	

	struct ShaderConstantMemberDesc
	{
		std::string name;
		ShaderValueType type = ShaderValueType::Float4;
		uint32_t offset = 0;
		uint32_t size = 0;
	};

	struct ShaderConstantBufferDesc
	{
		std::string name;
		uint32_t bindPoint = 0;
		uint32_t registerSpace = 0;
		uint32_t byteSize = 0;
		ShaderParameterScope scope = ShaderParameterScope::Material;

		std::vector<ShaderConstantMemberDesc> members;
	};

	struct PbrMaterialData
	{
		Vector4 diffuseAlbedo = {1.0f, 1.0f, 1.0f, 1.0f};
		Vector3 fresnelR0 = { 0.0f, 0.0f, 0.0f };
		float roughness = 1.0f;
		DirectX::XMFLOAT4X4 matTransform = Identity4x4();

		bool hasDiffuse = true;
		bool hasNormal = false;
		bool hasRoughness = false;
		bool invRoughness = false;
	};

	struct PassData
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 invView;
		DirectX::XMFLOAT4X4 proj;
		DirectX::XMFLOAT4X4 invProj;
		DirectX::XMFLOAT4X4 viewProj;
		DirectX::XMFLOAT4X4 invViewProj;
		Vector3 eyePos;
		float pad1;
		Vector2 renderTargetSize;
		Vector2 invRenderTargetSize;
		float znear;
		float zfar;
		float totalTime;
		float deltaTime;
		Vector4 ambientLight;

		LightData lights[MaxLights];

		Vector2 shadowMapSize;
		Vector2 invShadowMapSize;
		DirectX::XMFLOAT4X4 lightViewProjs[MaxCascadedNum];
		Vector4 gSpliters[MaxCascadedNum];
	};

	struct ObjectData
	{
		DirectX::XMFLOAT4X4 world;
	};

	struct ConstantBlock
	{
		ShaderParameterScope semantic = ShaderParameterScope::Object;
		std::string name;
		std::vector<std::byte> bytes;
	};
}
