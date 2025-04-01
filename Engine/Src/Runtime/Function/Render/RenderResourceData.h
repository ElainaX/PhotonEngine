#pragma once 

#include "RenderResourceData.h"
#include "RenderObject/RenderItem.h"
#include "RenderCamera.h"
#include "Function/Util/GameTimer.h"
#include "Resource/Texture/Buffer.h"
#include "Resource/Texture/Texture2D.h"


#include <memory>
#include <vector>

namespace photon 
{


	class RenderResourceData
	{
	public:
		virtual ~RenderResourceData() {}
	};


	struct MainPassRenderResourceData : public RenderResourceData
	{
		GameTimer* gameTimer;

		std::vector<LightData*> directionalLights;
		//std::shared_ptr<Texture2D> diffuseMap;
		std::shared_ptr<Texture2D> renderTarget;
		std::shared_ptr<Texture2D> depthStencil;
		std::vector<CommonRenderItem*> allRenderItems;
		RenderCamera* mainCamera;
	};

	struct ForwardPipelineRenderResourceData : public RenderResourceData
	{
		GameTimer* gameTimer;

		std::vector<LightData*> directionalLights;
		// std::shared_ptr<Texture2D> diffuseMap;
		std::shared_ptr<Texture2D> renderTarget;
		std::shared_ptr<Texture2D> depthStencil;
		//std::shared_ptr<Texture2D> resourceTex;
		std::vector<CommonRenderItem*> allRenderItems;
		RenderCamera* mainCamera;
	};

	struct TestSubPassData : public RenderResourceData
	{
		Shader* shader;
		std::vector<MacroInfo> macros;
		RenderTargetView* renderTargetView = nullptr;
		DepthStencilView* depthStencilView = nullptr;
		std::vector<CommonRenderItem*> renderItems;
		UINT passConstantIdx = 0;
	};

	struct UISubPassData : public RenderResourceData
	{
		RenderTargetView* renderTargetView = nullptr;
		DepthStencilView* depthStencilView = nullptr;
	};

}