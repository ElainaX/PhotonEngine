#pragma once

#include "../DX12Define.h"
#include "../d3dx12.h"
#include "Macro.h"
#include "Descriptor.h"

#include <d3d12.h>
#include <wrl.h>
#include <map>

namespace photon 
{
	class DescriptorHeapBase
	{
	public:
		virtual ~DescriptorHeapBase() {};

	public:
		int GetDescriptorSize() const { return m_DescriptorSize; }
		int GetHeapSize() const { return m_HeapSize; }
		ID3D12DescriptorHeap* GetDXHeapPtr() { return m_Heap.Get(); }
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleByIndex(int index = 0)
		{
			CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_Heap->GetCPUDescriptorHandleForHeapStart());
			handle.Offset(index, m_DescriptorSize);
			return handle;
		}
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandleByIndex(int index = 0)
		{
			CD3DX12_GPU_DESCRIPTOR_HANDLE handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(m_Heap->GetGPUDescriptorHandleForHeapStart());
			handle.Offset(index, m_DescriptorSize);
			return handle;
		}
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandleByViewGuid(int guid)
		{
			auto find_it = m_Views.find(guid);
			if(find_it != m_Views.end())
			{
				return find_it->second->cpuHandleInHeap;
			}
			LOG_ERROR("Not Have Such guid!");
			return GetCPUHandleByIndex();
		}
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandleByViewGuid(int guid)
		{
			auto find_it = m_Views.find(guid);
			if (find_it != m_Views.end())
			{
				return find_it->second->gpuHandleInHeap;
			}
			LOG_ERROR("Not Have Such guid!");
			return GetGPUHandleByIndex();
		}

	protected:
		int m_DescriptorSize;
		int m_HeapSize;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_Heap;
		std::unordered_map<UINT64, std::shared_ptr<ViewBase>> m_Views;
		ID3D12Device* m_Device;
	};
}

