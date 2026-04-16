#pragma once
#include "Shader.h"
#include "ShaderCompiler.h"
#include "Function/Render/ResourceHandle.h"
#include "Function/Render/DX12RHI/RootSignature/RootSignatureCache.h"
#include "Function/Util/ResourceSlotCollection.h"

namespace photon
{
	class ShaderProgramCollection : public ResourceSlotCollection<ShaderHandle, ShaderProgramRenderResource>
	{
	public:
		bool Initialize(RootSignatureCache* rootSignatureCache);
		void Shutdown();

		ShaderHandle CreateFromAsset(
			const ShaderProgramAsset& programAsset,
			const std::array<const ShaderStageAsset*, static_cast<size_t>(ShaderStage::Count)>& stageAssets);

		ShaderProgramRenderResource* TryGet(ShaderHandle h)
		{
			auto* e = TryGetEntry(h);
			return e ? &e->resource : nullptr;
		}
		const ShaderProgramRenderResource* TryGet(ShaderHandle h) const
		{
			auto* e = TryGetEntry(h);
			return e ? &e->resource : nullptr;
		}

		void Destroy(ShaderHandle h);

	private:
		ShaderCompiler m_shaderCompiler;
		RootSignatureCache* m_rootSignatureCache = nullptr;

	};

}
