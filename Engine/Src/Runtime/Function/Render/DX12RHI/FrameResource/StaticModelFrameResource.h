#pragma once

#include "FrameResource.h"
#include "../DX12Resource/ConstantBuffer.h"
#include "Function/Render/RHI.h"

#include <memory>
#include <unordered_map>

namespace photon 
{
	struct StaticModelFrameResourceDesc : public FrameResourceDesc
	{
		UINT allObjectNum = 0;
		UINT allPassNum = 0;
	};

	struct StaticModelObjectConstants
	{
		static UINT64 s_CurrObjectIndex;

		Vector4 color;
	};

	struct StaticModelPassConstants
	{
		static UINT64 s_CurrPassIndex;
		Vector4 color;
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

		std::shared_ptr<ConstantBuffer> allObjectsConstantBuffer;
		std::shared_ptr<ConstantBuffer> allPassesConstantBuffer;

		std::unordered_map<UINT64, ConstantBufferView*> allObjectConstantsViews;
		std::unordered_map<UINT, ConstantBufferView*> allPassConstantsViews;
	};

	struct StaticModelFrameResourceRenderItemInfo
	{
		// FrameResourceData
		INT64 objConstantIdx = -1;

		StaticModelObjectConstants objectConstants;
	};

}