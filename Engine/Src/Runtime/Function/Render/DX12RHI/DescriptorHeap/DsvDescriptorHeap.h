#pragma once
#include "DescriptorHeap.h"

#include <unordered_map>
#include <memory>

namespace photon
{
	class DsvDescriptorHeap : public DescriptorHeapBase
	{
	public:
		using DescriptorHeapBase::GetCPUHandleByIndex;

		DsvDescriptorHeap(ID3D12Device* device, int size)
			: DescriptorHeapBase()
		{
			m_Device = device;
			m_HeapSize = size;
			CreateHeap(m_HeapSize);
			m_DescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		}




		// 创建 CbvSrvUav，返回堆中的 Index
		DepthStencilView* CreateDepthStencilView(Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* pDesc, DepthStencilView* thisView = nullptr)
		{
			if(thisView != nullptr)
			{
				m_Device->CreateDepthStencilView(resource->gpuResource.Get(), pDesc, thisView->cpuHandleInHeap);
				thisView->viewDesc = pDesc ? *pDesc : D3D12_DEPTH_STENCIL_VIEW_DESC();
				thisView->resource = resource;
				return thisView;
			}

			int index = m_DsvCount;
			if (index >= m_HeapSize)
			{
				LOG_ERROR("DsvDescriptorHeap Overflow! Can't Create new DepthStencilView!");
				return nullptr;
			}
			// 大概是对的
			auto cpuHandle = GetCPUHandleByIndex(index);
			m_Device->CreateDepthStencilView(resource->gpuResource.Get(), pDesc, cpuHandle);

			auto view = std::make_shared<DepthStencilView>();
			view->cpuHandleInHeap = cpuHandle;
			view->offsetCountInHeap = index;
			view->viewDesc = pDesc ? *pDesc : D3D12_DEPTH_STENCIL_VIEW_DESC();
			view->resource = resource;

			m_Views.insert({ view->viewGuid, view });

			m_DsvCount++;

			return view.get();
		}

		// 创建 CbvSrvUav，返回堆中的 Index
		DepthStencilView* CreateDepthStencilView()
		{
			int index = m_DsvCount;
			if (index >= m_HeapSize)
			{
				LOG_ERROR("DsvDescriptorHeap Overflow! Can't Create new DepthStencilView!");
				return nullptr;
			}
			// 大概是对的
			auto cpuHandle = GetCPUHandleByIndex(index);

			auto view = std::make_shared<DepthStencilView>();
			view->cpuHandleInHeap = cpuHandle;
			view->offsetCountInHeap = index;

			m_Views.insert({ view->viewGuid, view });

			m_DsvCount++;

			return view.get();
		}

	private:
		void CreateHeap(int size)
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.NumDescriptors = size;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 0;
			DX_LogIfFailed(m_Device->CreateDescriptorHeap(&desc,
				IID_PPV_ARGS(&m_Heap)));
		}

		UINT32 m_DsvCount = 0;

	};
}

