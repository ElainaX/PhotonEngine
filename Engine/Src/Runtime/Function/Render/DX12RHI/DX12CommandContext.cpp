#include "DX12CommandContext.h"

#include <cmath>
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx12.h>

#include "d3dx12.h"


namespace photon
{
	bool DX12CommandContext::Initialize(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type)
	{
		m_type = type;
		DX_LogIfFailed(device->CreateCommandAllocator(m_type, IID_PPV_ARGS(&m_bootstrapAllocator)));
		DX_LogIfFailed(device->CreateCommandList(0, m_type, m_bootstrapAllocator.Get() , nullptr, IID_PPV_ARGS(&m_commandList)));
		DX_LogIfFailed(m_commandList->Close());
		return true;
	}

	void DX12CommandContext::Shutdown()
	{}

	void DX12CommandContext::Reset(ID3D12CommandAllocator* allocator, ID3D12PipelineState* initialPSO)
	{
		m_commandList->Reset(allocator, initialPSO);
	}

	void DX12CommandContext::Close()
	{
		m_commandList->Close();
	}

	void DX12CommandContext::ResourceStateTransform(DXResource* resource, D3D12_RESOURCE_STATES stateAfter)
	{
		D3D12_RESOURCE_STATES stateBefore = resource->state;
		if (stateBefore == stateAfter)
			return;
		
		auto resourceTrans = CD3DX12_RESOURCE_BARRIER::Transition(resource->gpuResource.Get(), stateBefore, stateAfter);
		m_commandList->ResourceBarrier(1, &resourceTrans);

		resource->state = stateAfter;
	}

	void DX12CommandContext::BatchResourceStateTransform(const std::vector<DXResource*>& resources,
		const std::vector<D3D12_RESOURCE_STATES> batchStatesAfter)
	{
		if (resources.size() != batchStatesAfter.size())
		{
			LOG_ERROR("Resource数量和State数量不匹配");
		}

		int minsize = std::min<int>(resources.size(), batchStatesAfter.size());
		std::vector<CD3DX12_RESOURCE_BARRIER> barriers(minsize);
		for (int i = 0; i < minsize; ++i)
		{
			D3D12_RESOURCE_STATES stateBefore = resources[i]->state;
			barriers[i] = CD3DX12_RESOURCE_BARRIER::Transition(resources[i]->gpuResource.Get(), stateBefore, batchStatesAfter[i]);
			resources[i]->state = batchStatesAfter[i];
		}
		m_commandList->ResourceBarrier(minsize, &barriers[0]);
	}


	void DX12CommandContext::SetViewportsAndScissorRects(D3D12_RECT scissorRect, D3D12_VIEWPORT viewport)
	{
		m_commandList->RSSetViewports(1, &viewport);
		m_commandList->RSSetScissorRects(1, &scissorRect);
	}

	void DX12CommandContext::SetPipelineState(ID3D12PipelineState* pipeline)
	{
		m_commandList->SetPipelineState(pipeline);
	}

	void DX12CommandContext::SetGraphicsRootSignature(ID3D12RootSignature* rootSignature)
	{
		m_commandList->SetGraphicsRootSignature(rootSignature);
	}

	void DX12CommandContext::SetRenderTargets(UINT numRenderTargetViews, const D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors, bool RTsSingleHandleToDescriptorRange, const D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilDescriptor)
	{
		m_commandList->OMSetRenderTargets(numRenderTargetViews, pRenderTargetDescriptors, RTsSingleHandleToDescriptorRange, pDepthStencilDescriptor);
	}

	void DX12CommandContext::SetDescriptorHeaps(const std::vector<ID3D12DescriptorHeap*>& descriptorHeaps)
	{
		m_commandList->SetDescriptorHeaps(descriptorHeaps.size(), &descriptorHeaps[0]);
	}

	void DX12CommandContext::SetGraphicsRootDescriptorTable(UINT RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor)
	{
		m_commandList->SetGraphicsRootDescriptorTable(RootParameterIndex, BaseDescriptor);
	}

	void DX12CommandContext::SetGraphicsRootConstantBufferView(UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation)
	{
		m_commandList->SetGraphicsRootConstantBufferView(RootParameterIndex, BufferLocation);
	}

	void DX12CommandContext::SetVertexBuffers(UINT startSlot, UINT numViews, const D3D12_VERTEX_BUFFER_VIEW* pViews)
	{
		m_commandList->IASetVertexBuffers(startSlot, numViews, pViews);
	}

	void DX12CommandContext::SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* pView)
	{
		m_commandList->IASetIndexBuffer(pView);
	}

	void DX12CommandContext::SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology)
	{
		m_commandList->IASetPrimitiveTopology(PrimitiveTopology);
	}

	void DX12CommandContext::DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation)
	{
		m_commandList->DrawIndexedInstanced(IndexCountPerInstance, InstanceCount, StartIndexLocation, BaseVertexLocation, StartInstanceLocation);
	}

	void DX12CommandContext::ClearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE view, Vector4 clearRGBA, UINT numRects,
	                                           const D3D12_RECT* clearRect)
	{
		m_commandList->ClearRenderTargetView(view, &clearRGBA[0], numRects, clearRect);
	}

	void DX12CommandContext::ClearDepthStencil(D3D12_CPU_DESCRIPTOR_HANDLE view, D3D12_CLEAR_FLAGS ClearFlags, float depth,
		UINT8 stencil, UINT numRects, const D3D12_RECT* clearRect)
	{
		m_commandList->ClearDepthStencilView(view, ClearFlags, depth, stencil, numRects, clearRect);
	}


	// 外界需要自行确保两个Resource的维度相同，并且处在可供拷贝或复制的heap中
	void DX12CommandContext::CopyGpuResource(DXResource* dstResource, DXResource* srcResource)
	{
		m_commandList->CopyResource(dstResource->gpuResource.Get(), srcResource->gpuResource.Get());
	}


	void DX12CommandContext::CopyCpuDataToGpuDest(DXResource* dest, DXResource* upload, UINT64 uploadOffsetInBytes, const D3D12_SUBRESOURCE_DATA* resources, UINT resourcesStartIdx /*= 0*/, UINT resourcesNum /*= 1*/)
	{
		UpdateSubresources(m_commandList.Get(), dest->gpuResource.Get(), upload->gpuResource.Get(), uploadOffsetInBytes, resourcesStartIdx, resourcesNum, resources);
	}

	void DX12CommandContext::CopyBufferCpuToUpload(DXResource* dstResource, const void* data, UINT64 sizeInBytes)
	{
		auto dst = dstResource->gpuResource.Get();
		void* mappedData = nullptr;
		dst->Map(0, nullptr, &mappedData);
		CopyMemory(mappedData, data, sizeInBytes);
		dst->Unmap(0, nullptr);
	}

	void DX12CommandContext::CopyBufferCpuToUpload(DXResource* dstResource, UINT64 startPosInBytes, const void* data, UINT64 sizeInBytes)
	{
		auto dst = dstResource->gpuResource.Get();
		void* mappedData = nullptr;
		dst->Map(0, nullptr, &mappedData);
		CopyMemory((char*)mappedData + startPosInBytes, data, sizeInBytes);
		dst->Unmap(0, nullptr);
	}

	void DX12CommandContext::CopyBufferRegion(DXResource* dstResource, DXResource* srcResource, UINT64 dstStartPosInBytes, UINT64 srcStartPosInBytes, UINT64 sizeInBytes)
	{
		m_commandList->CopyBufferRegion(dstResource->gpuResource.Get(), dstStartPosInBytes, srcResource->gpuResource.Get(), srcStartPosInBytes, sizeInBytes);
	}



	void DX12CommandContext::CopyTextureRegion(DXResource* dest, DXResource* src, UINT32 destArrayIndex, Vector3i destResourceCoords, UINT32 srcArrayIndex /*= 0*/, Vector3i srcResourceCoordsStart /*=*/, Vector3i srcResourceCoordsEnd /*= */)
	{
		D3D12_TEXTURE_COPY_LOCATION destLoc;
		destLoc.pResource = dest->gpuResource.Get();
		destLoc.SubresourceIndex = destArrayIndex;
		destLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		D3D12_TEXTURE_COPY_LOCATION srcLoc;
		srcLoc.pResource = src->gpuResource.Get();
		srcLoc.SubresourceIndex = srcArrayIndex;
		srcLoc.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		D3D12_BOX srcBox;
		srcBox.left = srcResourceCoordsStart.x;
		srcBox.top = srcResourceCoordsStart.y;
		srcBox.front = srcResourceCoordsStart.z;

		srcBox.right = srcResourceCoordsEnd.x == -1 ? src->dxDesc.Width : srcResourceCoordsEnd.x;
		srcBox.bottom = srcResourceCoordsEnd.y == -1 ? src->dxDesc.Height : srcResourceCoordsEnd.y;
		srcBox.back = srcResourceCoordsEnd.z == -1 ? src->dxDesc.DepthOrArraySize : srcResourceCoordsEnd.z;


		m_commandList->CopyTextureRegion(&destLoc, destResourceCoords.x, destResourceCoords.y, destResourceCoords.z, &srcLoc, &srcBox);
	}

	void DX12CommandContext::Dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ)
	{
		m_commandList->Dispatch(groupX, groupY, groupZ);
	}
}
