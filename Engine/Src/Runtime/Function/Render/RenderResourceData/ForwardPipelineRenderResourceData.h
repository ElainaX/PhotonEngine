#pragma once
#include "../RenderResourceData.h"
#include "../RenderObject/RenderItem.h"
#include "../RenderCamera.h"
#include "Function/Util/GameTimer.h"
#include <memory>
#include <vector>
namespace photon
{
	class Texture2D;
	struct ForwardPipelineRenderResourceData : public RenderResourceData
	{
		GameTimer* gameTimer;

		// std::shared_ptr<Texture2D> diffuseMap;
		std::shared_ptr<Texture2D> renderTarget;
		std::shared_ptr<Texture2D> depthStencil;
		//std::shared_ptr<Texture2D> resourceTex;
		std::vector<CommonRenderItem*> allRenderItems;
		RenderCamera* mainCamera;
	};
}