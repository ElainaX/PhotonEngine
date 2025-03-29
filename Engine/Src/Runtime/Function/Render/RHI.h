#pragma once
#include "RhiStruct.h"
#include "Resource/ResourceType.h"
#include "Resource/Texture/Texture2D.h"
#include "Resource/Texture/Buffer.h"
#include "Shader/Shader.h"
#include "DX12RHI/d3dx12.h"

#include <memory>
#include <cstdint>
#include <utility>
#include <d3d12.h>
#include <wrl.h>
#include <array>
#include <string>

namespace photon 
{
	class WindowSystem;
	class ResourceManager;
	class VertexBuffer;
	class IndexBuffer;
	enum class VertexType;
	class ConstantBuffer;
	class ConstantBufferView;
	class ShaderResourceView;
	class UnorderedAccessView;
	class DepthStencilView;
	class RenderTargetView;
	class SamplerView;
	class DXPipeline;
	class FrameResource;
	enum class FrameResourceType;
	class FrameResourceDesc;

	// 特定RHI的Init如果需要其他的信息就在这个struct中添加
	struct RHIInitInfo
	{
		std::shared_ptr<WindowSystem> window_System;
	};

	// RHI Interface
	class RHI
	{
	public:
		virtual ~RHI(){}
		
		// 关于创建的相关函数
		virtual void Initialize(RHIInitInfo initializeInfo) = 0;
		virtual void CreateSwapChain() = 0;
		virtual void ReCreateSwapChain() = 0;
		virtual void CreateFactory() = 0;
		virtual void CreateDevice() = 0;
		virtual void CreateFence() = 0;

		virtual void CreateCommandObjects() = 0;
		virtual void CreateSwapChainRenderTarget() = 0;
		virtual void CreateDescriptorHeaps() = 0;

		virtual void CreateAssetAllocator() = 0;

		virtual void CreateDebugManager() = 0;

		virtual void Clear() = 0;

		// cmdList命令
		virtual void CmdSetViewportsAndScissorRects(D3D12_RECT scissorRect, D3D12_VIEWPORT viewport) = 0;
		virtual void CmdSetPipelineState(ID3D12PipelineState* pipeline) = 0;
		virtual void CmdSetGraphicsRootSignature(ID3D12RootSignature* rootSignature) = 0;
		virtual void CmdSetRenderTargets(UINT numRenderTargetViews, const D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors, bool RTsSingleHandleToDescriptorRange, const D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilDescriptor) = 0;
		virtual void CmdSetDescriptorHeaps(const std::vector<ID3D12DescriptorHeap*>& descriptorHeaps) = 0;
		virtual void CmdSetDescriptorHeaps() = 0;
		virtual void CmdSetGraphicsRootDescriptorTable(UINT RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor) = 0;
		virtual void CmdSetGraphicsRootConstantBufferView(UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) = 0;
		virtual void CmdSetVertexBuffers(UINT startSlot, UINT numViews, const D3D12_VERTEX_BUFFER_VIEW* pViews) = 0;
		virtual void CmdSetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* pView) = 0;
		virtual void CmdSetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology) = 0;
		virtual void CmdDrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT  BaseVertexLocation,UINT StartInstanceLocation) = 0;
		virtual void CmdClearRenderTarget(RenderTargetView* view, Vector4 clearRGBA, UINT numRects = 0, const D3D12_RECT* clearRect = nullptr) = 0;
		virtual void CmdClearDepthStencil(DepthStencilView* view, D3D12_CLEAR_FLAGS ClearFlags, float depth, UINT8 stencil, UINT numRects = 0, const D3D12_RECT* clearRect = nullptr) = 0;

		// 常用功能
		virtual FrameResource* GetCurrFrameResource(FrameResourceType type) = 0;
		virtual void CreateFrameResource(FrameResourceType type, FrameResourceDesc* desc) = 0;
		virtual std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers() = 0;


		virtual void FlushCommandQueue() = 0;
		virtual void WaitForFenceValue(uint64_t fenceValue) = 0;

		virtual void CopyTextureToSwapChain(Texture2D* tex) = 0;
		virtual void PrepareForPresent() = 0;
		virtual void Present() = 0;



		virtual void BeginSingleRenderPass() = 0;
		virtual void EndSingleRenderPass() = 0;


		virtual unsigned int GetCurrBackBufferIndex() = 0;
		virtual std::shared_ptr<ResourceManager> GetResourceManager() = 0;

		virtual Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRootSignature(Shader* shader, int samplerCount = 0, const D3D12_STATIC_SAMPLER_DESC* samplerDesc = nullptr) = 0;
		virtual Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc) = 0;
		virtual std::shared_ptr<VertexBuffer> CreateVertexBuffer(VertexType type, const void* data, UINT64 sizeInBytes) = 0;
		virtual std::shared_ptr<IndexBuffer> CreateIndexBuffer(const void* data, UINT64 sizeInBytes) = 0;
		virtual std::shared_ptr<ConstantBuffer> CreateConstantBuffer(unsigned int elementCount, unsigned int singleElementSizeInBytes) = 0;
		virtual ConstantBufferView* CreateConstantBufferView(const D3D12_CONSTANT_BUFFER_VIEW_DESC* pDesc, ConstantBufferView* thisView = nullptr) = 0;
		virtual ConstantBufferView* CreateConstantBufferView() = 0;
		virtual ShaderResourceView* CreateShaderResourceView(Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* pDesc, ShaderResourceView* thisView = nullptr) = 0;
		virtual ShaderResourceView* CreateShaderResourceView() = 0;
		virtual UnorderedAccessView* CreateUnorderedAccessView(Resource* resource, Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* pDesc, UnorderedAccessView* thisView = nullptr) = 0;
		virtual UnorderedAccessView* CreateUnorderedAccessView() = 0;
		virtual DepthStencilView* CreateDepthStencilView(Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* pDesc, DepthStencilView* thisView = nullptr) = 0;
		virtual DepthStencilView* CreateDepthStencilView() = 0;
		virtual RenderTargetView* CreateRenderTargetView(Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* pDesc, RenderTargetView* thisView = nullptr) = 0;
		virtual RenderTargetView* CreateRenderTargetView() = 0;
		virtual SamplerView* CreateSampler(const D3D12_SAMPLER_DESC* pDesc, SamplerView* thisView = nullptr) = 0;
		virtual SamplerView* CreateSampler() = 0;

		

		// 资源相关函数
		virtual std::shared_ptr<Texture2D> CreateTexture2D(Texture2DDesc desc) = 0;
		virtual std::shared_ptr<Texture2D> LoadTextureFromFile(const std::wstring& filepath, std::unique_ptr<uint8_t[]>& decodedData, D3D12_SUBRESOURCE_DATA& subresource, size_t maxsize = 0) = 0;
		virtual std::shared_ptr<Buffer> CreateBuffer(BufferDesc desc) = 0;
		virtual std::shared_ptr<Buffer> CreateBuffer(BufferDesc desc, const void* data, UINT64 sizeInBytes) = 0;
		virtual void CopyDataCpuToGpu(Resource* dstResource, const void* data, UINT64 sizeInBytes) = 0;
		virtual void CopyDataCpuToGpu(Resource* dstResource, UINT64 startPosInBytes, const void* data, UINT64 sizeInBytes) = 0;
		virtual void CopyDataGpuToGpu(Resource* dstResource, Resource* srcResource) = 0;
		virtual void CopyDataGpuToGpu(Resource* dstResource, Resource* srcResource, UINT64 dstStartPosInBytes, UINT64 srcStartPosInBytes, UINT64 sizeInBytes) = 0;
		virtual void CopySubResourceDataCpuToGpu(Resource* dest, Resource* upload, UINT64 uploadOffsetInBytes, D3D12_SUBRESOURCE_DATA* resources, UINT resourcesStartIdx = 0, UINT resourcesNum = 1) = 0;

		virtual void ResourceStateTransform(Resource* resource, D3D12_RESOURCE_STATES stateAfter) = 0;

	private:
		// No Private
	};
}