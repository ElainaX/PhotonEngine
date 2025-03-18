#pragma once
#include "DescriptorHeap.h"

#include <unordered_map>
#include <memory>

namespace photon
{
	class SamplerDescriptorHeap : public DescriptorHeapBase
	{
	public:
		using DescriptorHeapBase::GetCPUHandleByIndex;
		using DescriptorHeapBase::GetGPUHandleByIndex;

		SamplerDescriptorHeap(ID3D12Device* device, int size)
			: DescriptorHeapBase()
		{
			m_Device = device;
			m_HeapSize = size;
			CreateHeap(m_HeapSize);
			m_DescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
		}




		// 创建 CbvSrvUav，返回堆中的 Index
		SamplerView* CreateSampler(const D3D12_SAMPLER_DESC* pDesc, SamplerView* thisView = nullptr)
		{
			if(thisView != nullptr)
			{
				m_Device->CreateSampler(pDesc, thisView->cpuHandleInHeap);
				thisView->viewDesc = pDesc ? *pDesc : D3D12_SAMPLER_DESC();
				return thisView;
			}

			int index = m_SamplerCount;
			if (index >= m_HeapSize)
			{
				LOG_ERROR("SamplerDescriptorHeap Overflow! Can't Create new SamplerView!");
				return nullptr;
			}
			// 大概是对的
			auto cpuHandle = GetCPUHandleByIndex(index);
			auto gpuHandle = GetGPUHandleByIndex(index);
			m_Device->CreateSampler(pDesc, cpuHandle);

			auto view = std::make_shared<SamplerView>();
			view->cpuHandleInHeap = cpuHandle;
			view->gpuHandleInHeap = gpuHandle;
			view->offsetCountInHeap = index;
			view->viewDesc = pDesc ? *pDesc : D3D12_SAMPLER_DESC();

			m_Views.insert({ view->viewGuid, view });

			m_SamplerCount++;

			return view.get();
		}

		SamplerView* CreateSampler()
		{
			int index = m_SamplerCount;
			if (index >= m_HeapSize)
			{
				LOG_ERROR("SamplerDescriptorHeap Overflow! Can't Create new SamplerView!");
				return nullptr;
			}
			// 大概是对的
			auto cpuHandle = GetCPUHandleByIndex(index);
			auto gpuHandle = GetGPUHandleByIndex(index);

			auto view = std::make_shared<SamplerView>();
			view->cpuHandleInHeap = cpuHandle;
			view->gpuHandleInHeap = gpuHandle;
			view->offsetCountInHeap = index;

			m_Views.insert({ view->viewGuid, view });

			m_SamplerCount++;

			return view.get();
		}

	private:
		void CreateHeap(int size)
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.NumDescriptors = size;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 0;
			DX_LogIfFailed(m_Device->CreateDescriptorHeap(&desc,
				IID_PPV_ARGS(&m_Heap)));
		}

		UINT32 m_SamplerCount = 0;

	};
}

