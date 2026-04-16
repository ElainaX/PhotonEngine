#pragma once
#include <vector>
#include <algorithm>

#include "Function/Render/DX12RHI/DXPipeline/GraphicsPipelineDescBuilder.h"
#include "Function/Render/RenderPass/PassContext.h"
#include "Function/Render/RenderPass/PassDrawItem.h"

namespace photon
{

	class BaseSubPass
	{
	public:
		virtual void Initialize() {}
		virtual void Shutdown() {}

		// CPU 阶段：筛选 / resolve / 排序
		virtual void Prepare(const PassPrepareContext& ctx) = 0;
		// GPU 阶段：真正发命令
		virtual void Execute(const PassExecuteContext& ctx) = 0;
		virtual ~BaseSubPass() = default;

		const std::vector<PassDrawItem>& GetDrawList() const
		{
			return m_drawList;
		}


	protected:
		void ClearDrawList()
		{
			m_drawList.clear();
		}

		void SortDrawList()
		{
			std::sort(m_drawList.begin(), m_drawList.end(),
				[](const PassDrawItem& lhs, const PassDrawItem& rhs)
				{
					return lhs.sortKey < rhs.sortKey;
				});
		}

	protected:
		// pass 级别的固定覆盖项
		PassPipelineStateOverrides m_pipelineOverrides = {};

		// 本帧整理后的绘制列表
		std::vector<PassDrawItem> m_drawList;

	};
}
