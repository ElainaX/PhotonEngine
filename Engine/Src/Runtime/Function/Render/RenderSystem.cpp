#include "RenderSystem.h"
#include "DX12RHI/DX12RHI.h"
#include "ForwardRenderPipeline.h"

namespace photon 
{

	RenderSystem::~RenderSystem()
	{
		
	}

	void RenderSystem::Initialize(RenderSystemInitInfo initInfo)
	{
		LOG_INFO("RenderSystemInit");

		m_Rhi = std::make_shared<DX12RHI>();
		RHIInitInfo rhiInitInfo;
		rhiInitInfo.window_System = initInfo.windowSystem;
		m_Rhi->Initialize(rhiInitInfo);


		m_RenderPipelines[RenderPipelineType::ForwardPipeline] = 
			std::make_shared<ForwardRenderPipeline>();
		m_RenderPipelines[RenderPipelineType::ForwardPipeline]->Initialize(m_Rhi);

		m_ResourceData = std::make_shared<RenderResourceData>();
		Texture2DDesc texDesc;
		Vector2i swapchainWidthAndHeight = initInfo.windowSystem->GetClientWidthAndHeight();
		texDesc.width = swapchainWidthAndHeight.x;
		texDesc.height = swapchainWidthAndHeight.y;
		texDesc.format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texDesc.heapProp = ResourceHeapProperties::Static;
		texDesc.flag = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
		m_ResourceData->texA = m_Rhi->CreateTexture2D(texDesc);
	}

	void RenderSystem::Tick(GameTimer& gt)
	{
		m_RenderPipelines[RenderPipelineType::ForwardPipeline]->Render(m_Rhi, m_ResourceData);
	}

	std::shared_ptr<photon::RHI> RenderSystem::GetRHI()
	{
		return m_Rhi;
	}

	void RenderSystem::SetRenderPipelineType(RenderPipelineType renderType)
	{

	}

}