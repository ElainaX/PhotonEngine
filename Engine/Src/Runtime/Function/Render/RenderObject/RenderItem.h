#pragma once
#include "Mesh.h"
#include "Material.h"
#include "RenderMeshCollection.h"
#include "Function/Render/GameObject.h"
#include "Core/Math/Quaternion.h"
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

	inline std::string GetRenderLayerString(RenderLayer layer)
	{
		switch (layer)
		{
		case RenderLayer::Opaque:
			return "Opaque";
		case RenderLayer::Transparency:
			return "Transparency";
		}
	}

	inline std::vector<std::string> GetAllRenderLayerStringList()
	{
		return { "Opaque", "Transparency" };
	}

	struct RenderItem : public GameObject
	{
		RenderItem() {}
		virtual ~RenderItem(){}
		virtual std::string GetGameObjectType() override { return "RenderItem"; }
	};

	struct StaticFrameResourceEditor
	{
		bool bDirty = true;

		void SetTranslation(Vector3 trans) { translation = trans; bDirty = true; }
		void SetScale(Vector3 _scale) {  scale = _scale; bDirty = true; }
		void SetRotation(Vector3 rot) { rotationXYZ = rot; bDirty = true; }


		Vector3 translation = { 0.0f, 0.0f, 0.0f };
		Vector3 scale = {1.0f, 1.0f, 1.0f};
		Vector3 rotationXYZ = { 0.0f, 0.0f, 0.0f };

		StaticModelObjectConstants ToObjectConstants()
		{
			using namespace DirectX;
			StaticModelObjectConstants ret;
			auto rot = XM_PI * rotationXYZ / 180.0f;
			Quaternion quat(rot.x, rot.y, rot.z);
			auto quatFloat4 = quat.ToDXFloat4();
			DirectX::XMMATRIX mat = DirectX::XMMatrixAffineTransformation(XMLoadFloat3((XMFLOAT3*)&scale), 
										XMLoadFloat3((XMFLOAT3*)&translation), XMLoadFloat4((XMFLOAT4*)&quatFloat4),
										XMLoadFloat3((XMFLOAT3*)&translation));
			XMStoreFloat4x4(&ret.world, mat);
			return ret;
		}
	};


	struct CommonRenderItem : public RenderItem
	{
		CommonRenderItem()
			: RenderItem()
		{
			GameObjectName = GetGameObjectType();
		}

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



		std::string GetGameObjectType() override;
		void SetDirty(bool bDirty = true)
		{
			if(bDirty)
			{
				numFrameDirty = g_FrameContextCount;
			}
			else 
			{

				numFrameDirty = 0;
			}
		}

	};
}