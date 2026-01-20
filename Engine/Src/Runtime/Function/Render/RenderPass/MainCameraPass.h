#pragma once
#include "Function/Render/RenderPass.h"
#include "Function/Render/RenderResourceData.h"
#include "Function/Render/RHI.h"
#include "../Shader/TestShader.h"
#include "../RenderObject/RenderMeshCollection.h"
#include "../DX12RHI/DescriptorHeap/Descriptor.h"
#include "../DX12RHI/DXPipeline/DXGraphicsPipeline.h"
#include "Function/Render/WindowSystem.h"
#include "SubPass/TestSubPass.h"
#include "SubPass/DebugDrawSubPass.h"
#include "SubPass/UISubPass.h"
#include "SubPass/DrawSkyboxSubPass.h"
#include "../DX12RHI/FrameResource/StaticModelFrameResource.h"

#include <map>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace photon 
{
	class MainCameraPass : public RenderPass
	{
	public:
		void Initialize(RHI* rhi, WindowSystem* windowSystem);

		void PrepareContext(RenderResourceData * data) override;

		void Draw(EG_FrameContext* frame, PassBlackboard* bb) override;

		void OnlyUI(bool bOnlyUI = true);
		
	private:
		CommonRenderItem* CreateLightRenderItem(LightData* light);


		std::shared_ptr<TestSubPass> m_TestSubpass;
		std::shared_ptr<DebugDrawSubPass> m_DebugDrawSubpass;
		std::shared_ptr<UISubPass> m_UISubpass;
		std::shared_ptr<DrawSkyboxSubPass> m_SkyboxSubpass;
		RenderTargetView* m_TestRenderTargetView = nullptr;
		DepthStencilView* m_TestDepthStencilView = nullptr;
		ShaderResourceView* m_TestShaderResourceView = nullptr;

		StaticModelPassConstants m_PassConstants;
		UINT m_PassConstantsIdx;

		std::unordered_map<LightData*, std::shared_ptr<CommonRenderItem>> m_LightRenderItems;


		std::unordered_map<std::string, std::shared_ptr<CommonRenderItem>> m_InnerCommonRenderItems;



		RenderMeshCollection m_MeshCollection;
		std::shared_ptr<Mesh> m_LightMesh;
		Shader* m_DebugDrawLightShader;

	private:
		bool m_bOnlyUI = false;
		WindowSystem* m_WindowSystem;
	};
}