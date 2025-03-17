#pragma once
#include "RHI.h"
#include "RenderType.h"
#include "RenderPipeline.h"
#include "RenderResourceData.h"
#include "ResourceManager.h"

#include <memory>
#include <unordered_map>

namespace photon 
{
	class WindowSystem;
	class GameTimer;

	struct RenderSystemInitInfo
	{
		std::shared_ptr<WindowSystem> windowSystem;
	};


	class RenderSystem
	{
	public:
		RenderSystem() = default;
		~RenderSystem();

		void Initialize(RenderSystemInitInfo initInfo);
		void Tick(GameTimer& gt);
		std::shared_ptr<RHI> GetRHI();

		void SetRenderPipelineType(RenderPipelineType renderType);


	private:
		std::shared_ptr<RHI> m_Rhi;
		std::unordered_map<RenderPipelineType, std::shared_ptr<RenderPipeline>> m_RenderPipelines;
		std::shared_ptr<RenderResourceData> m_ResourceData;
		//std::shared_ptr<RenderCamera> m_MainRenderCamera;
		//std::shared_ptr<RebderScene> m_RenderScene;

	};

}