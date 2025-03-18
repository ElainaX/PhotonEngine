#pragma once
#include "Mesh.h"
#include "RenderMeshCollection.h"

namespace photon 
{
	enum class RenderLayer
	{
		Opaque,
		Transparency,
		Count
	};

	struct RenderItem 
	{
		RenderItem(RenderLayer _layer)
			: layer(_layer){}

		RenderLayer layer;
	};



	struct OpaqueRenderItem : public RenderItem
	{
		OpaqueRenderItem()
			: RenderItem(RenderLayer::Opaque)
		{}

		// submesh data in RenderMeshCollection
		// MeshData
		RenderMeshCollection* meshCollection = nullptr;
		D3D12_PRIMITIVE_TOPOLOGY primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		UINT64 meshGuid = 0;

		// MaterialData
		

	};
}