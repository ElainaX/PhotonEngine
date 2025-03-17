#pragma once

#include "Resource/ResourceType.h"
#include "../DX12RHI/DX12Resource/VertexType.h"

#include <d3d12.h>
#include <string>
#include <wrl.h>

namespace photon 
{
	struct MeshDesc 
	{
		std::string name = "Mesh";
		VertexType type;
		Microsoft::WRL::ComPtr<ID3DBlob> vertexRawData;
		Microsoft::WRL::ComPtr<ID3DBlob> indexRawData;

	};

	struct Mesh : public Resource
	{
		Mesh() = default;
		Mesh(MeshDesc desc);

		// 创建的时候需要填写
		VertexType type;
		Microsoft::WRL::ComPtr<ID3DBlob> vertexRawData;
		Microsoft::WRL::ComPtr<ID3DBlob> indexRawData;

		// 给MeshCollection用的
		UINT indexCount = 0;
		UINT startIndexLocation = 0;
		INT baseVertexLocation = 0;
		bool bIsLoad = false;
	};
}