#pragma once
#include "RenderPass/PassContext.h"

namespace photon 
{
	class RenderPass
	{
	public:
		virtual ~RenderPass() = default;
		virtual void Initialize(const RenderPipelineServices& services)
		{
			m_services = services;
		}
		virtual void Prepare(const PassPrepareContext& ctx) = 0;
		virtual void Execute(const PassExecuteContext& ctx) = 0;

	protected:
		RenderPipelineServices m_services = {};
	};
}