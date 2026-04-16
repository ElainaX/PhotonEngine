#pragma once
#include "Function/Render/RenderPass.h"

namespace photon
{
	class PreprocessPass : public RenderPass
	{
	public:
		void Initialize(const RenderPipelineServices& services) override;
		void Prepare(const PassPrepareContext& ctx) override;
		void Execute(const PassExecuteContext& ctx) override;
	};
}