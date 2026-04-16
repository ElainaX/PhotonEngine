#pragma once
#include <filesystem>

#include "AssetMeta.h"
#include "Bounds.h"
#include "ImporterSettings.h"
#include "Function/Render/ResourceTypes.h"
#include "Function/Render/Resource/MeshCpuData.h"

namespace photon
{
	enum class MeshType : uint8_t
	{
		StaticMesh, // staticMesh使用VertexPNTUV
		SkinnedMesh // skinnedMesh使用VertexSkinned
	};

	struct SubmeshAssetDesc
	{
		std::string name;

		// 相对整个MeshCpuData的范围
		uint32_t vertexStart = 0;
		uint32_t vertexCount = 0;

		uint32_t indexStart = 0;
		uint32_t indexCount = 0;

		// 这个子网格使用哪个材质槽
		uint32_t materialSlot = 0;

		DirectX::BoundingBox localBounds;
	};

	struct MeshAsset
	{
		AssetMeta meta;

		MeshUsage usage = MeshUsage::Static;
		MeshImportSettings importSettings;

		MeshType type = MeshType::StaticMesh;

		MeshCpuData cpuData;
		std::vector<SubmeshAssetDesc> submeshes;

		// 默认材质槽表：slot -> material guid
		std::vector<Guid> defaultMaterialGuids;

		DirectX::BoundingBox localBounds;

		bool cpuDataResident = true;

		bool HasCpuData() const
		{
			return cpuDataResident && !cpuData.Empty();
		}

		void ReleaseCpuData()
		{
			cpuData.Clear();
			cpuDataResident = false;
		}
	};

}
