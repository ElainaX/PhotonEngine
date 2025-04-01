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
		Material(StaticModelMaterialDataConstants data, Texture2D* tex)
			: matCBufferData(data), diffuseMap(tex){
			matCBufferIdx = StaticModelMaterialDataConstants::s_CurrMatDataIndex++;
		}

		UINT64 matCBufferIdx = -1;
		StaticModelMaterialDataConstants matCBufferData;
		Texture2D* diffuseMap;


	};
}