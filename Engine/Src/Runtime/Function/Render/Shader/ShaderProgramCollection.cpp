#include "ShaderProgramCollection.h"

namespace photon
{
	bool ShaderProgramCollection::Initialize(RootSignatureCache* rootSignatureCache)
	{
		m_rootSignatureCache = rootSignatureCache;
		return m_rootSignatureCache != nullptr;
	}

	void ShaderProgramCollection::Shutdown()
	{
		ResetAll();
		m_rootSignatureCache = nullptr;
	}

	ShaderHandle ShaderProgramCollection::CreateFromAsset(const ShaderProgramAsset& programAsset,
		const std::array<const ShaderStageAsset*, static_cast<size_t>(ShaderStage::Count)>& stageAssets)
	{
		if (!m_rootSignatureCache)
			return {};

		ShaderProgramRenderResource rr = {};
		rr.assetGuid = programAsset.meta.guid;
		rr.parameters = programAsset.parameters;
		rr.constantBuffers = programAsset.constantBuffers;

		bool anyStageCompiled = false;
		for (uint32_t i = 0; i < ShaderStageCount(); ++i)
		{
			ShaderStage stage = static_cast<ShaderStage>(i);
			if (!programAsset.HasStage(stage))
				continue;

			const ShaderStageAsset* stageAsset = stageAssets[i];
			if (!stageAsset)
				continue;

			CompiledShaderBytecode bc = m_shaderCompiler.Compile(*stageAsset);
			if (bc.Empty())
				continue;

			rr.stages[i].valid = true;
			rr.stages[i].bytecode = std::move(bc);
			anyStageCompiled = true;
		}

		if (!anyStageCompiled)
			return {};

		RootSignatureDesc rsDesc = BuildRootSignatureDesc(programAsset);
		rr.rootSignature = m_rootSignatureCache->GetOrCreate(rsDesc);

		ShaderHandle handle = AllocateHandle();
		Entry* entry = TryGetEntry(handle);
		if (!entry)
			return {};
		rr.handle = handle;
		entry->resource = std::move(rr);
		return handle;
	}

	void ShaderProgramCollection::Destroy(ShaderHandle h)
	{
		FreeHandle(h);
	}
}
