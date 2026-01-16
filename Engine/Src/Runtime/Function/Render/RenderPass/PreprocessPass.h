#pragma once
#include "Function/Render/RenderPass.h"
#include "Function/Render/RenderResourceData.h"
#include "Function/Render/RHI.h"
#include "../Shader/TestShader.h"
#include "../RenderObject/RenderMeshCollection.h"
#include "../DX12RHI/DescriptorHeap/Descriptor.h"
#include "../DX12RHI/DXPipeline/DXGraphicsPipeline.h"
#include "Function/Render/WindowSystem.h"
#include "SubPass/DrawShadowSubPass.h"
#include "../DX12RHI/FrameResource/StaticModelFrameResource.h"

namespace photon 
{
	class PreprocessPass : public RenderPass 
	{
	public:
		void Initialize(RHI* rhi);
		void PrepareContext(RenderResourceData* data) override;
		void Draw() override;

	private:

		std::vector<StaticModelPassConstants> m_PassConstantses;
		std::shared_ptr<DrawShadowSubPass> m_ShadowSubPass;
		// self-contain的ShadowManager
		std::shared_ptr<CascadedShadowManager> m_CascadedShadowManager;
		RHI* m_Rhi = nullptr;
		std::vector<int> m_PassConstantsIndices;

		Shader* m_ShadowShader;
	};


}