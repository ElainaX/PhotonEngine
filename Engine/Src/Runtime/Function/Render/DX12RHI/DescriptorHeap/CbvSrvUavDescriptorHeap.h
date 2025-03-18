#pragma once
#include "DescriptorHeap.h"

#include <unordered_map>
#include <memory>

namespace photon
{
	class CbvSrvUavDescriptorHeap : public DescriptorHeapBase
	{
	public:
		using DescriptorHeapBase::GetCPUHandleByIndex;
		using DescriptorHeapBase::GetGPUHandleByIndex;

		CbvSrvUavDescriptorHeap(ID3D12Device* device, int size)
			: DescriptorHeapBase()
		{
			m_CbvStartPos = m_SrvStartPos = m_UavStartPos = 0;
			m_Device = device;
			m_HeapSize = size;
			CreateHeap(m_HeapSize);
			m_DescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
		CbvSrvUavDescriptorHeap(ID3D12Device* device, int sizeCbv, int sizeSrv, int sizeUav)
			: DescriptorHeapBase()
		{
			m_Device = device;
			m_CbvStartPos = 0;
			m_SrvStartPos = sizeCbv;
			m_UavStartPos = m_SrvStartPos + sizeSrv;
			m_HeapSize = m_UavStartPos + sizeUav;
			CreateHeap(m_HeapSize);
			m_DescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}




		// 创建 CbvSrvUav，返回堆中的 ConstantBufferView
		ConstantBufferView* CreateConstantBufferView(const D3D12_CONSTANT_BUFFER_VIEW_DESC* pDesc, ConstantBufferView* thisView = nullptr)
		{
			if(thisView != nullptr)
			{
				m_Device->CreateConstantBufferView(pDesc, thisView->cpuHandleInHeap);
				thisView->viewDesc = pDesc ? *pDesc : D3D12_CONSTANT_BUFFER_VIEW_DESC();
				return thisView;
			}

			int index = m_CbvStartPos + m_CbvCount;
			if (index >= m_SrvStartPos)
			{
				LOG_ERROR("CbvSrvUavDescriptorHeap Overflow! Can't Create new ConstantBufferView!");
				return nullptr;
			}
			// 大概是对的
			auto cpuHandle = GetCPUHandleByIndex(index);
			auto gpuHandle = GetGPUHandleByIndex(index);
			m_Device->CreateConstantBufferView(pDesc, cpuHandle);

			auto view = std::make_shared<ConstantBufferView>();
			view->cpuHandleInHeap = cpuHandle;
			view->gpuHandleInHeap = gpuHandle;
			view->offsetCountInHeap = index;
			view->viewDesc = pDesc ? *pDesc : D3D12_CONSTANT_BUFFER_VIEW_DESC();

			m_Views.insert({ view->viewGuid, view });

			m_CbvCount++;

			return view.get();
		}
		ConstantBufferView* CreateConstantBufferView()
		{
			int index = m_CbvStartPos + m_CbvCount;
			if (index >= m_SrvStartPos)
			{
				LOG_ERROR("CbvSrvUavDescriptorHeap Overflow! Can't Create new ConstantBufferView!");
				return nullptr;
			}
			// 大概是对的
			auto cpuHandle = GetCPUHandleByIndex(index);
			auto gpuHandle = GetGPUHandleByIndex(index);

			auto view = std::make_shared<ConstantBufferView>();
			view->cpuHandleInHeap = cpuHandle;
			view->gpuHandleInHeap = gpuHandle;
			view->offsetCountInHeap = index;

			m_Views.insert({ view->viewGuid, view });
			m_CbvCount++;

			return view.get();
		}



		ShaderResourceView* CreateShaderResourceView(Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* pDesc, ShaderResourceView* thisView = nullptr)
		{
			if (thisView != nullptr)
			{
				m_Device->CreateShaderResourceView(resource->gpuResource.Get(), pDesc, thisView->cpuHandleInHeap);
				thisView->viewDesc = pDesc ? *pDesc : D3D12_SHADER_RESOURCE_VIEW_DESC();
				thisView->resource = resource;
				return thisView;
			}

			int index = m_SrvStartPos + m_SrvCount;
			if (index >= m_UavStartPos)
			{
				LOG_ERROR("CbvSrvUavDescriptorHeap Overflow! Can't Create new ShaderResourceView!");
				return nullptr;
			}
			// 大概是对的
			auto cpuHandle = GetCPUHandleByIndex(index);
			auto gpuHandle = GetGPUHandleByIndex(index);
			m_Device->CreateShaderResourceView(resource->gpuResource.Get(), pDesc, cpuHandle);

			auto view = std::make_shared<ShaderResourceView>();
			view->cpuHandleInHeap = cpuHandle;
			view->gpuHandleInHeap = gpuHandle;
			view->offsetCountInHeap = index;
			view->viewDesc = pDesc ? *pDesc : D3D12_SHADER_RESOURCE_VIEW_DESC();
			view->resource = resource;

			m_Views.insert({ view->viewGuid, view });

			m_SrvCount++;

			return view.get();
		}
		ShaderResourceView* CreateShaderResourceView()
		{
			int index = m_SrvStartPos + m_SrvCount;
			if (index >= m_UavStartPos)
			{
				LOG_ERROR("CbvSrvUavDescriptorHeap Overflow! Can't Create new ShaderResourceView!");
				return nullptr;
			}
			// 大概是对的
			auto cpuHandle = GetCPUHandleByIndex(index);
			auto gpuHandle = GetGPUHandleByIndex(index);

			auto view = std::make_shared<ShaderResourceView>();
			view->cpuHandleInHeap = cpuHandle;
			view->gpuHandleInHeap = gpuHandle;
			view->offsetCountInHeap = index;

			m_Views.insert({ view->viewGuid, view });

			m_SrvCount++;

			return view.get();
		}

		UnorderedAccessView* CreateUnorderedAccessView(Resource* resource, Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* pDesc, UnorderedAccessView* thisView = nullptr)
		{
			if (thisView != nullptr)
			{
				m_Device->CreateUnorderedAccessView(resource->gpuResource.Get(), counterResource->gpuResource.Get(), pDesc, thisView->cpuHandleInHeap);
				thisView->viewDesc = pDesc ? *pDesc : D3D12_UNORDERED_ACCESS_VIEW_DESC();
				thisView->resource = resource;
				thisView->counterResource = counterResource;
				return thisView;
			}

			int index = m_UavStartPos + m_UavCount;
			if (index >= m_HeapSize)
			{
				LOG_ERROR("CbvSrvUavDescriptorHeap Overflow! Can't Create new UnorderedAccessView!");
				return nullptr;
			}
			// 大概是对的
			auto cpuHandle = GetCPUHandleByIndex(index);
			auto gpuHandle = GetGPUHandleByIndex(index);
			m_Device->CreateUnorderedAccessView(resource->gpuResource.Get(), counterResource->gpuResource.Get(), pDesc, cpuHandle);

			auto view = std::make_shared<UnorderedAccessView>();
			view->cpuHandleInHeap = cpuHandle;
			view->gpuHandleInHeap = gpuHandle;
			view->offsetCountInHeap = index;
			view->viewDesc = pDesc ? *pDesc : D3D12_UNORDERED_ACCESS_VIEW_DESC();
			view->resource = resource;
			view->counterResource = counterResource;

			m_Views.insert({ view->viewGuid, view });

			m_UavCount++;

			return view.get();
		}
		UnorderedAccessView* CreateUnorderedAccessView()
		{
			int index = m_UavStartPos + m_UavCount;
			if (index >= m_HeapSize)
			{
				LOG_ERROR("CbvSrvUavDescriptorHeap Overflow! Can't Create new UnorderedAccessView!");
				return nullptr;
			}
			// 大概是对的
			auto cpuHandle = GetCPUHandleByIndex(index);
			auto gpuHandle = GetGPUHandleByIndex(index);

			auto view = std::make_shared<UnorderedAccessView>();
			view->cpuHandleInHeap = cpuHandle;
			view->gpuHandleInHeap = gpuHandle;
			view->offsetCountInHeap = index;

			m_Views.insert({ view->viewGuid, view });

			m_UavCount++;

			return view.get();
		}

	private:
		void CreateHeap(int size)
		{
			D3D12_DESCRIPTOR_HEAP_DESC desc;
			desc.NumDescriptors = size;
			desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			desc.NodeMask = 0;
			DX_LogIfFailed(m_Device->CreateDescriptorHeap(&desc,
				IID_PPV_ARGS(&m_Heap)));
		}

		UINT32 m_CbvCount = 0;
		UINT32 m_SrvCount = 0;
		UINT32 m_UavCount = 0;

		UINT32 m_CbvStartPos = 0;
		UINT32 m_SrvStartPos = 0;
		UINT32 m_UavStartPos = 0;

	};
}

