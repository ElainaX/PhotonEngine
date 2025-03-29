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
#include "../RenderResourceData/MainPassRenderResourceData.h"
#include "../DX12RHI/FrameResource/StaticModelFrameResource.h"

#include <map>
#include <memory>

namespace photon 
{
	class MainCameraPass : public RenderPass
	{
	public:
		void Initialize(RHI* rhi, WindowSystem* windowSystem);

		void PrepareContext(RenderResourceData*) override;

		void Draw() override;
		
	private:
		std::shared_ptr<TestSubPass> m_TestSubpass;
		RenderTargetView* m_TestRenderTargetView = nullptr;
		DepthStencilView* m_TestDepthStencilView = nullptr;
		ShaderResourceView* m_TestShaderResourceView = nullptr;

		StaticModelPassConstants m_PassConstants;
		UINT m_PassConstantsIdx;

	private:

		WindowSystem* m_WindowSystem;
	};
}