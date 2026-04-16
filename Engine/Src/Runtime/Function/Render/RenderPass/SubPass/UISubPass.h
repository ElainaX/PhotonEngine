#pragma once
#include "SubPass.h"

namespace photon
{
	class UISubPass : public BaseSubPass
	{
	public:
		void Prepare(const PassPrepareContext& ctx) override;
		void Execute(const PassExecuteContext& ctx) override;
	};
}