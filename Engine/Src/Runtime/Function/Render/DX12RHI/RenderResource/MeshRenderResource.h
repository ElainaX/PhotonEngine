#pragma once
#include <string>

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexType.h"
#include "Function/Render/ResourceHandle.h"
#include "Function/Render/ResourceTypes.h"
#include "Function/Render/RenderObject/Bounds.h"
#include "Function/Render/RenderObject/ImporterSettings.h"
#include "Function/Render/Resource/MeshCpuData.h"

namespace photon
{
	/*
	 * 为什么用SubMeshRenderDesc？
	 * 假设你有一个大 IB，里面存了很多 mesh。
	其中某个角色 mesh 占了：

	startIndex = 10000
	indexCount = 5000

	这就是你的 IndexBufferRange。

	但是这个角色 mesh 内部又分成：

	body：前 3000 个 index
	hair：后 1200 个 index
	sword：最后 800 个 index

	那 submesh 信息可能就是相对于这个 mesh 自己再分：

	body：localStartIndex = 0, localIndexCount = 3000
	hair：localStartIndex = 3000, localIndexCount = 1200
	sword：localStartIndex = 4200, localIndexCount = 800

	最终绘制时真正的全局起点才是：

	body：10000 + 0
	hair：10000 + 3000
	sword：10000 + 4200
	 * 
	 */



	struct SubmeshRenderDesc
	{
		std::string name;

		uint32_t indexCount = 0;

		// 相对 mesh 的偏移，不是最终 draw 参数
		uint32_t indexStartInMesh = 0;
		uint32_t vertexStartInMesh = 0;

		uint32_t materialSlot = 0;
		DirectX::BoundingBox localBounds;
	};

	struct MeshRenderResource
	{
		MeshHandle handle;
		Guid assetGuid;

		MeshUsage usage = MeshUsage::Static;
		VertexLayoutDesc vertexLayout;

		VertexBuffer bigVb;
		IndexBuffer bigIb;

		SubVertexBuffer meshVb;
		SubIndexBuffer meshIb;

		std::vector<SubmeshRenderDesc> submeshes;
		DirectX::BoundingBox localBounds;

		// Dynamic / Stream 时可选保留一份运行时 CPU mirror
		std::unique_ptr<MeshCpuData> cpuMirror;
		std::unique_ptr<DynamicMeshState> dynamicState;

		bool IsDynamic() const
		{
			return usage != MeshUsage::Static;
		}


	};
}

