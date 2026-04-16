#pragma once
#include "Function/Render/RenderItem.h"
#include "Function/Render/ResourceHandle.h"


namespace photon
{
	// 某个 pass 在本帧真正准备绘制的轻量项
	struct PassDrawItem
	{
		const RenderItem* item = nullptr;

		MeshHandle mesh = {};
		MaterialHandle material = {};
		ShaderHandle shader = {};

		uint32_t submeshIndex = 0;

		PipelineStateHandle pipeline = {};
		RootSignatureHandle rootSignature = {};

		// Prepare 阶段预先 attach 到 frame heap
		FrameDescriptorHandle baseColorSrv = {};
		FrameDescriptorHandle normalSrv = {};
		FrameDescriptorHandle roughnessSrv = {};
		FrameDescriptorHandle materialCbv = {};
		FrameDescriptorHandle objectCbv = {};

		// 排序键：opaque 优先按状态排序，transparent 优先按深度排序
		uint64_t sortKey = 0;

		bool IsValid() const
		{
			return item != nullptr
				&& mesh.handle.IsValid()
				&& material.handle.IsValid()
				&& shader.handle.IsValid()
				&& pipeline.handle.IsValid()
				&& rootSignature.handle.IsValid();
		}
	};
}
