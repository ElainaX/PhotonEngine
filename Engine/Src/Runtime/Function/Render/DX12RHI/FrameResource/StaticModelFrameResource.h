#pragma once

#include "FrameResource.h"
#include "../DX12Resource/ConstantBuffer.h"
#include "Function/Render/RHI.h"
#include "Core/Math/Vector4.h"
#include "Function/Render/Light.h"

#include <memory>
#include <unordered_map>
#include <d3d12.h>

namespace photon 
{
	struct StaticModelFrameResourceDesc : public FrameResourceDesc
	{
		UINT allObjectNum = 0;
		UINT allPassNum = 0;
		UINT allMatDatasNum = 0;
	};

	struct StaticModelObjectConstants
	{
		static UINT64 s_CurrObjectIndex;

		DirectX::XMFLOAT4X4 world;
	};

	struct StaticModelMaterialDataConstants
	{
		static UINT64 s_CurrMatDataIndex;

		Vector4 diffuseAlbedo;
		Vector3 fresnelR0;
		float roughness;
		DirectX::XMFLOAT4X4 matTransform;
		int bhasDiffuse = 1;
		int bhasNormal = 0;
		int bhasRoughness = 0;
		int bInverseRoughness = 0;
	};

	struct StaticModelPassConstants
	{
		static UINT64 s_CurrPassIndex;
		
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 invView;
		DirectX::XMFLOAT4X4 proj;
		DirectX::XMFLOAT4X4 invProj;
		DirectX::XMFLOAT4X4 viewProj;
		DirectX::XMFLOAT4X4 invViewProj;
		Vector3 eyePos;
		float pad1;
		Vector2 renderTargetSize;
		Vector2 invRenderTargetSize;
		float znear;
		float zfar;
		float totalTime;
		float deltaTime;
		Vector4 ambientLight;

		LightData lights[MaxLights];
	};

	class StaticModelFrameResource : public FrameResource
	{
	public:
		StaticModelFrameResource() { m_FrameResourceType = FrameResourceType::StaticModelFrameResource; }

		StaticModelFrameResource(RHI* rhi, StaticModelFrameResourceDesc desc)
		{ 
			Create(rhi, desc);
		}

		void Create(RHI* rhi, StaticModelFrameResourceDesc desc)
		{
			m_FrameResourceType = FrameResourceType::StaticModelFrameResource;
			allObjectsConstantBuffer = rhi->CreateConstantBuffer(desc.allObjectNum * 3 / 2, sizeof(StaticModelObjectConstants));
			allPassesConstantBuffer = rhi->CreateConstantBuffer(desc.allPassNum * 3 / 2, sizeof(StaticModelPassConstants));
			allMaterialDatasConstantBuffer = rhi->CreateConstantBuffer(desc.allMatDatasNum * 3 / 2, sizeof(StaticModelMaterialDataConstants));
			m_Rhi = rhi;
		}

		ConstantBufferView* GetObjectConstantBufferView(UINT64 objectIdx)
		{
			auto find_it = allObjectConstantsViews.find(objectIdx);
			if(find_it != allObjectConstantsViews.end())
			{
				return find_it->second;
			}
			D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
			desc.BufferLocation = allObjectsConstantBuffer->GetConstantGPUAddressByIndex(objectIdx);
			desc.SizeInBytes = allObjectsConstantBuffer->constantBufferStrideInBytes;
			allObjectConstantsViews[objectIdx] = m_Rhi->CreateConstantBufferView(&desc);
			return allObjectConstantsViews[objectIdx];
		}
		ConstantBufferView* GetPassConstantBufferView(UINT passIdx)
		{
			auto find_it = allPassConstantsViews.find(passIdx);
			if (find_it != allPassConstantsViews.end())
			{
				return find_it->second;
			}
			D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
			desc.BufferLocation = allPassesConstantBuffer->GetConstantGPUAddressByIndex(passIdx);
			desc.SizeInBytes = allPassesConstantBuffer->constantBufferStrideInBytes;
			allPassConstantsViews[passIdx] = m_Rhi->CreateConstantBufferView(&desc);
			return allPassConstantsViews[passIdx];
		}
		ConstantBufferView* GetMatDataConstantBufferView(UINT matIdx)
		{
			auto find_it = allMaterialDatasConstantsViews.find(matIdx);
			if (find_it != allMaterialDatasConstantsViews.end())
			{
				return find_it->second;
			}
			D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
			desc.BufferLocation = allMaterialDatasConstantBuffer->GetConstantGPUAddressByIndex(matIdx);
			desc.SizeInBytes = allMaterialDatasConstantBuffer->constantBufferStrideInBytes;
			allMaterialDatasConstantsViews[matIdx] = m_Rhi->CreateConstantBufferView(&desc);
			return allMaterialDatasConstantsViews[matIdx];
		}

		void UpdateObjectConstantBuffer(UINT64 objectIdx, const void* objectConstantsData)
		{
			UINT szObjectConstant = sizeof(StaticModelObjectConstants);
			allObjectsConstantBuffer->UpdateElements(m_Rhi, objectIdx, objectConstantsData, szObjectConstant);
		}

		void UpdatePassConstantBuffer(UINT passIdx, const void* passConstantsData)
		{
			UINT szPassConstant = sizeof(StaticModelPassConstants);
			allPassesConstantBuffer->UpdateElements(m_Rhi, passIdx, passConstantsData, szPassConstant);
		}

		void UpdateMatDataConstantBuffer(UINT matIdx, const void* matDataConstantsData)
		{
			UINT szMatDataConstant = sizeof(StaticModelMaterialDataConstants);
			allMaterialDatasConstantBuffer->UpdateElements(m_Rhi, matIdx, matDataConstantsData, szMatDataConstant);
		}

		std::shared_ptr<ConstantBuffer> allObjectsConstantBuffer;
		std::shared_ptr<ConstantBuffer> allPassesConstantBuffer;
		std::shared_ptr<ConstantBuffer> allMaterialDatasConstantBuffer;

		std::unordered_map<UINT64, ConstantBufferView*> allObjectConstantsViews;
		std::unordered_map<UINT, ConstantBufferView*> allPassConstantsViews;
		std::unordered_map<UINT64, ConstantBufferView*> allMaterialDatasConstantsViews;
	};

	struct StaticModelFrameResourceRenderItemInfo
	{
		// FrameResourceData
		INT64 objConstantIdx = -1;

		StaticModelObjectConstants objectConstants;
	};

}