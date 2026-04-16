#pragma once

#include <d3d12.h>
#include <cstdint>
#include <wrl/client.h>

#include "GpuResourceManager.h"
#include "Resource/DXResourceHeader.h"

namespace photon
{
	// 这里只提供native接口
	class DX12CommandContext
	{
	public:
		bool Initialize(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type);
		void Shutdown();

		void Reset(ID3D12CommandAllocator* allocator, ID3D12PipelineState* initialPSO = nullptr);
		void Close();

		ID3D12GraphicsCommandList* GetNativeCommandList() const { return m_commandList.Get(); }
		D3D12_COMMAND_LIST_TYPE GetType() const { return m_type; }

	public:

		void ResourceStateTransform(DXResource* resource, D3D12_RESOURCE_STATES stateAfter);
		void BatchResourceStateTransform(const std::vector<DXResource*>& resources, const std::vector<D3D12_RESOURCE_STATES> batchStatesAfter);

		void SetViewportsAndScissorRects(D3D12_RECT scissorRect, D3D12_VIEWPORT viewport);
		void SetPipelineState(ID3D12PipelineState* pipeline);
		void SetGraphicsRootSignature(ID3D12RootSignature* rootSignature);
		void SetRenderTargets(UINT numRenderTargetViews, const D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors, bool RTsSingleHandleToDescriptorRange, const D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilDescriptor) ;
		void SetDescriptorHeaps(const std::vector<ID3D12DescriptorHeap*>& descriptorHeaps) ;
		void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor) ;
		void SetGraphicsRootConstantBufferView(UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) ;
		void SetVertexBuffers(UINT startSlot, UINT numViews, const D3D12_VERTEX_BUFFER_VIEW* pViews) ;
		void SetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* pView) ;
		void SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology) ;
		void DrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation) ;
		void ClearRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE view, Vector4 clearRGBA, UINT numRects = 0, const D3D12_RECT* clearRect = nullptr);
		void ClearDepthStencil(D3D12_CPU_DESCRIPTOR_HANDLE view, D3D12_CLEAR_FLAGS ClearFlags, float depth, UINT8 stencil, UINT numRects = 0, const D3D12_RECT* clearRect = nullptr);

		void CopyGpuResource(DXResource* dstResource, DXResource* srcResource) ;
		void CopyCpuDataToGpuDest(DXResource* dest, DXResource* upload, UINT64 uploadOffsetInBytes, const D3D12_SUBRESOURCE_DATA* resources, UINT resourcesStartIdx = 0, UINT resourcesNum = 1) ;
		void CopyBufferCpuToUpload(DXResource* dstResource, const void* data, UINT64 sizeInBytes) ;
		void CopyBufferCpuToUpload(DXResource* dstResource, UINT64 startPosInBytes, const void* data, UINT64 sizeInBytes) ;
		void CopyBufferRegion(DXResource* dstResource, DXResource* srcResource, UINT64 dstStartPosInBytes, UINT64 srcStartPosInBytes, UINT64 sizeInBytes) ;
		void CopyTextureRegion(DXResource* dest, DXResource* src, UINT32 destArrayIndex, Vector3i destResourceCoords = {0, 0, 0},
			UINT32 srcArrayIndex = 0, Vector3i srcResourceCoordsStart = { 0, 0, 0 }, Vector3i srcResourceCoordsEnd = {-1, -1, -1}) ;



		void Dispatch(uint32_t groupX, uint32_t groupY, uint32_t groupZ);

	private:
		D3D12_COMMAND_LIST_TYPE m_type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> m_commandList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_bootstrapAllocator;
	};
}


