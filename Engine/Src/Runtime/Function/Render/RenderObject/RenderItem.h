#pragma once
#include "Mesh.h"
#include "Material.h"
#include "RenderMeshCollection.h"
#include "../DX12RHI/FrameResource/StaticModelFrameResource.h"
#include "../DX12RHI/DX12Define.h"

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
		RenderItem(){}
		virtual ~RenderItem(){}
	};



	struct CommonRenderItem : public RenderItem
	{
		CommonRenderItem()
			: RenderItem()
		{}

		static FrameResourceType s_FrameResourceType;
		using TFrameResource = StaticModelFrameResource;

		UINT numFrameDirty = g_FrameContextCount;

		// submesh data in RenderMeshCollection
		// MeshData
		RenderMeshCollection* meshCollection = nullptr;
		D3D12_PRIMITIVE_TOPOLOGY primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		UINT64 meshGuid = 0;

		Material* material = nullptr;

		Shader* shader = nullptr;

		RenderLayer renderLayer = RenderLayer::Opaque;

		StaticModelFrameResourceRenderItemInfo frameResourceInfo;

		// MaterialData


	};
}