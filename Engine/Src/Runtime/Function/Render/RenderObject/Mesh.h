﻿#pragma once

#include "Resource/ResourceType.h"
#include "../DX12RHI/DX12Resource/VertexType.h"
#include "Function/Render/GameObject.h"

#include <d3d12.h>
#include <string>
#include <wrl.h>

namespace photon 
{
	struct MeshDesc 
	{
		std::wstring name = L"Mesh";
		VertexType type;
		D3D12_PRIMITIVE_TOPOLOGY topology;
		Microsoft::WRL::ComPtr<ID3DBlob> vertexRawData;
		Microsoft::WRL::ComPtr<ID3DBlob> indexRawData;

	};

	struct Mesh : public Resource
	{
		Mesh(MeshDesc desc);

		// 创建的时候需要填写
		VertexType type;
		D3D12_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		Microsoft::WRL::ComPtr<ID3DBlob> vertexRawData;
		Microsoft::WRL::ComPtr<ID3DBlob> indexRawData;

		// 给MeshCollection用的
		UINT indexCount = 0;
		UINT startIndexLocation = 0;
		INT baseVertexLocation = 0;
		bool bIsLoad = false;

	};
}