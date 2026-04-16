#pragma once
#include "AssetMeta.h"

namespace photon 
{

	struct ModelAsset
	{
		AssetMeta meta;

		// 暂时只保留一个合并后的 mesh 逻辑引用
		// 多 submesh / 多 material 关系都放在 MeshAsset 里
		Guid meshGuid;

		DirectX::BoundingBox localBounds;
	};
}
