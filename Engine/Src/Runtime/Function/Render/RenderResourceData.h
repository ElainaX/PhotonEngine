#pragma once 

#include "RenderResourceData.h"
#include "RenderObject/RenderItem.h"
#include "RenderCamera.h"
#include "Function/Util/GameTimer.h"
#include "Resource/Texture/Buffer.h"
#include "Resource/Texture/Texture2D.h"
#include "Resource/Texture/Cubemap.h"
#include "CascadedShadowManager.h"
#include "PassBlackboard.h"
#include "EGFrameContext.h"


#include <memory>
#include <vector>

namespace photon 
{


	class RenderResourceData
	{
	public:
		virtual ~RenderResourceData() {}
	};



	//struct ForwardPipelineRenderResourceData : public RenderResourceData
	//{
	//	EG_FrameContext* frameContext;

	//	//GameTimer* gameTimer;

	//	//std::vector<LightData*> directionalLights;
	//	//std::vector<LightData*> pointLights;
	//	//std::vector<LightData*> spotLights;
	//	// std::shared_ptr<Texture2D> diffuseMap;
	//	//std::shared_ptr<Texture2D> renderTarget;
	//	//std::shared_ptr<Texture2D> depthStencil;
	//	//std::shared_ptr<Texture2D> resourceTex;
	//	//std::vector<CommonRenderItem*> allRenderItems;
	//	//RenderCamera* mainCamera;
	//	//Cubemap* cubemap;

	//	// Main Light For Cascaded Shadow
	//	//LightBase* mainLight;
	//};

	class PassCommonData : public RenderResourceData
	{
	public:
		union
		{
			EG_FrameContext* frame = nullptr;
			// PipelineResourceData* ....
		};

		PassBlackboard* bb = nullptr;
	};


	struct MainPassRenderResourceData : public PassCommonData
	{
		// 需要填充的Data

		//GameTimer* gameTimer;

		//std::vector<LightData*> directionalLights;
		//std::vector<LightData*> pointLights;
		//std::vector<LightData*> spotLights;
		//std::shared_ptr<Texture2D> diffuseMap;
		//std::shared_ptr<Texture2D> renderTarget;
		//std::shared_ptr<Texture2D> depthStencil;
		//std::vector<CommonRenderItem*> allRenderItems;
		//RenderCamera* mainCamera;
		//Cubemap* cubemap;

		// 需要PreprocessPass传入的ShadowMap
		//std::shared_ptr<CascadedShadowManager> cascadedShadowManager;


		// 需要传递给下一个Pass的Data
	};

	struct PreprocessPassRenderResourceData : public PassCommonData
	{
		// 需要填充的Data
		//LightBase* mainLight;
		//RenderCamera* mainCamera;
		std::vector<float> spliters;
		//std::vector<CommonRenderItem*> allRenderItems;
		//// 需要传递给下一个Pass的Data
		//std::shared_ptr<CascadedShadowManager> cascadedShadowManager;
	};

	struct DrawShadowSubPassData : public PassCommonData
	{
		//std::shared_ptr<CascadedShadowManager> cascadedShadowManager;
		//std::vector<CommonRenderItem*> renderItems;
		std::vector<int> passConstantses;
		Shader* shadowShader;
	};

	struct TestSubPassData : public PassCommonData
	{
		Shader* shader;
		std::vector<MacroInfo> macros;
		//RenderTargetView* renderTargetView = nullptr;
		//DepthStencilView* depthStencilView = nullptr;
		//std::vector<CommonRenderItem*> renderItems;
		UINT passConstantIdx = 0;
	};


	struct UISubPassData : public PassCommonData
	{
		//RenderTargetView* renderTargetView = nullptr;
		//DepthStencilView* depthStencilView = nullptr;
	};

	struct DebugDrawLightPassData : public PassCommonData
	{
		//RenderTargetView* renderTargetView = nullptr;
		//DepthStencilView* depthStencilView = nullptr;

		std::vector<CommonRenderItem*> lightRenderItems;
		UINT passConstantIdx = 0;
	};

	struct DrawSkyboxSubPassData : public PassCommonData
	{
		//RenderTargetView* renderTargetView = nullptr;
		//DepthStencilView* depthStencilView = nullptr;
		CommonRenderItem* skyboxRenderItem = nullptr;
		//Cubemap* cubemap = nullptr;
		UINT passConstantIdx = 0;
	};

}