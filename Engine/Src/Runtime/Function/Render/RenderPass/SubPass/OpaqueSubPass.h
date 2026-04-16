#pragma once
#include "SubPass.h"

namespace photon
{
	class OpaqueSubPass : public BaseSubPass
	{
	public:
		void Initialize() override;
		void Shutdown() override;

		void Prepare(const PassPrepareContext& ctx) override;
		void Execute(const PassExecuteContext& ctx) override;

	private:
		void BuildDrawList(const PassPrepareContext& ctx);
		uint64_t BuildSortKey(const PassDrawItem& item,
			const PassPrepareContext& ctx) const;

	private:
		TextureFormat m_rtvFormat = TextureFormat::RGBA8_UNorm;
		TextureFormat m_dsvFormat = TextureFormat::D24S8;
	};

}
