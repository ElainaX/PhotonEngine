#pragma once 
#include <DirectXMath.h>
#include "Resource/ResourceType.h"
#include "Resource/Texture/Texture2D.h"
#include "Function/Render/GameObject.h"
#include "Core/Math/Vector4.h"
#include "Core/Math/Vector3.h"
#include "../DX12RHI/FrameResource/StaticModelFrameResource.h"

namespace photon 
{



	struct Material : public Resource
	{
		Material() = default;
		Material(StaticModelMaterialDataConstants data, Texture2D* diffuse, Texture2D* normal = nullptr, Texture2D* roughness = nullptr)
			: matCBufferData(data), diffuseMap(diffuse), normalMap(normal), roughnessMap(roughness){
			matCBufferIdx = StaticModelMaterialDataConstants::s_CurrMatDataIndex++;
			matCBufferData.bhasDiffuse = (diffuse != nullptr);
			matCBufferData.bhasNormal = (normal != nullptr);
			matCBufferData.bhasRoughness = (roughness != nullptr);
		}

		UINT64 matCBufferIdx = -1;
		StaticModelMaterialDataConstants matCBufferData;
		Texture2D* diffuseMap = nullptr;
		Texture2D* normalMap = nullptr;
		Texture2D* roughnessMap = nullptr;

	};
}