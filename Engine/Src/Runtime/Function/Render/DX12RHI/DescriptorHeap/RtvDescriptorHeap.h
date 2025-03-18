#pragma once
#include "DescriptorHeap.h"

#include <unordered_map>
#include <memory>

namespace photon
{
	class RtvDescriptorHeap : public DescriptorHeapBase
	{
	public:
		using DescriptorHeapBase::GetCPUHandleByIndex;

		RtvDescriptorHeap(ID3D12Device* device, int size)
			: DescriptorHeapBase()
		{
			m_Device = device;
			m_HeapSize = size;
			CreateHeap(m_HeapSize);
			m_DescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}




		// 创建 CbvSrvUav，返回堆中的 Index
		RenderTargetView* CreateRenderTargetView(Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* pDesc, RenderTargetView* thisView = nullptr)
		{
			if(thisView != nullptr)
			{
				m_Device->CreateRenderTargetView(resource->gpuResource.Get(), pDesc, thisView->cpuHandleInHeap);
				thisView->viewDesc = pDesc ? *pDesc : D3D12_RENDER_TARGET_VIEW_DESC();
				thisView->resource = resource;

				return thisView;
			}

			int index = m_RtvCount;
			if (index >= m_HeapSize)
			{
				LOG_ERROR("RtvDescriptorHeap Overflow! Can't Create new RenderTargetView!");
				return nullptr;
			}
			// 大概是对的
			auto cpuHandle = GetCPUHandleByIndex(index);
			m_Device->CreateRenderTargetView(resource->gpuResource.Get(), pDesc, cpuHandle);

			auto view = std::make_shared<RenderTargetView>();
			view->cpuHandleInHeap = cpuHandle;
			view->offsetCountInHeap = index;
			view->viewDesc = pDesc ? *pDesc : D3D12_RENDER_TARGET_VIEW_DESC();
			view->resource = resource;

			m_Views.insert({ view->viewGuid, view });

			m_RtvCount++;

			return view.get();
		}

		// 创建 CbvSrvUav，返回堆中的 Index
		RenderTargetView* CreateRenderTargetView()
		{
			int index = m_RtvCount;
			if (index >= m_HeapSize)
			{
				LOG_ERROR("RtvDescriptorHeap Overflow! Can't Create new RenderTargetView!");
				return nullptr;
			}
			// 大概是对的
			auto cpuHandle = GetCPUHandleByIndex(index);

			auto view = std::make_shared<RenderTargetView>();
			view->cpuHandleInHeap = cpuHandle;
			view->offsetCountInHeap = index;

			m_Views.insert({ view->viewGuid, view });

			m_RtvCount++;

			return view.get();
		}

	private:
		void CreateHeap(int size)
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.NumDescriptors = size;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			desc.NodeMask = 0;
			DX_LogIfFailed(m_Device->CreateDescriptorHeap(&desc,
				IID_PPV_ARGS(&m_Heap)));
		}

		UINT32 m_RtvCount = 0;

	};
}

