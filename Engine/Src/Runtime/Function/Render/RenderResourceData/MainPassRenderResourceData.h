#pragma once
#include "../RenderResourceData.h"
#include "../RenderObject/RenderItem.h"
#include <memory>
#include <vector>
namespace photon 
{
	class Texture2D;
	struct MainPassRenderResourceData : public RenderResourceData
	{
		std::shared_ptr<Texture2D> renderTarget;
		std::shared_ptr<Texture2D> depthStencil;
		std::vector<CommonRenderItem*> allRenderItems;
	};
}