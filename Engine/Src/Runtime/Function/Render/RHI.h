 #pragma once
#include "Resource/DXResourceHeader.h"
#include "Shader/Shader.h"
#include "DX12RHI/d3dx12.h"
#include "Core/Math/Vector3i.h"

#include <memory>
#include <cstdint>
#include <utility>
#include <d3d12.h>
#include <wrl.h>
#include <array>
#include <string>

#include "DX12RHI/FrameSyncSystem.h"

 namespace photon 
{
	class DescriptorSystem;
	class WindowSystem;
	class ResourceManager;
	class VertexBuffer;
	class IndexBuffer;
	class ConstantBuffer;
	class ConstantBufferView;
	class ShaderResourceView;
	class UnorderedAccessView;
	class DepthStencilView;
	class RenderTargetView;
	class SamplerView;
	class DXPipelineState;
	class FrameResource;
	enum class FrameResourceType;
	class FrameResourceDesc;
	class GpuResourceManager;

	enum class QueueType : uint8_t
	{
		Graphics,
		Compute,
		Copy
	};

	// 特定RHI的Init如果需要其他的信息就在这个struct中添加
	struct RHIInitInfo
	{
		WindowSystem* windowSystem;
		FrameSyncSystem* frameSyncSystem;
	};

	// RHI Interface
	class RHI
	{
	public:
		virtual ~RHI() = default;
		
		// 关于创建的相关函数
		virtual bool Initialize(const RHIInitInfo& initializeInfo) = 0;
		virtual void Shutdown() = 0;
		//virtual void InitializeImGui() = 0;

		//virtual void PrepareForPresent() = 0;
		virtual void Present() = 0;
		virtual void ResizeSwapChain(uint32_t width, uint32_t height) = 0;

		virtual uint64_t GetCompletedFenceValue(QueueType queueType) const = 0;
		virtual uint64_t SignalQueue(QueueType queueType) = 0;
		virtual void WaitForFenceValue(QueueType queueType, uint64_t fenceValue) = 0;


	/*	virtual void CreateSwapChain() = 0;
		virtual void ReCreateSwapChain() = 0;
		virtual void CreateFactory() = 0;
		virtual void CreateDevice() = 0;
		virtual void CreateFence() = 0;

		virtual void CreateCommandObjects() = 0;
		virtual void CreateSwapChainRenderTarget() = 0;
		virtual void CreateDescriptorHeaps() = 0;

		virtual void CreateAssetAllocator() = 0;

		virtual void CreateDebugManager() = 0;*/


		//// cmdList命令
		//virtual void CmdSetViewportsAndScissorRects(D3D12_RECT scissorRect, D3D12_VIEWPORT viewport) = 0;
		//virtual void CmdSetPipelineState(ID3D12PipelineState* pipeline) = 0;
		//virtual void CmdSetGraphicsRootSignature(ID3D12RootSignature* rootSignature) = 0;
		//virtual void CmdSetRenderTargets(UINT numRenderTargetViews, const D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors, bool RTsSingleHandleToDescriptorRange, const D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilDescriptor) = 0;
		//virtual void CmdSetDescriptorHeaps(const std::vector<ID3D12DescriptorHeap*>& descriptorHeaps) = 0;
		//virtual void CmdSetDescriptorHeaps() = 0;
		//virtual void CmdSetGraphicsRootDescriptorTable(UINT RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor) = 0;
		//virtual void CmdSetGraphicsRootConstantBufferView(UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) = 0;
		//virtual void CmdSetVertexBuffers(UINT startSlot, UINT numViews, const D3D12_VERTEX_BUFFER_VIEW* pViews) = 0;
		//virtual void CmdSetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* pView) = 0;
		//virtual void CmdSetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology) = 0;
		//virtual void CmdDrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT  BaseVertexLocation,UINT StartInstanceLocation) = 0;
		//virtual void CmdClearRenderTarget(RenderTargetView* view, Vector4 clearRGBA, UINT numRects = 0, const D3D12_RECT* clearRect = nullptr) = 0;
		//virtual void CmdClearDepthStencil(DepthStencilView* view, D3D12_CLEAR_FLAGS ClearFlags, float depth, UINT8 stencil, UINT numRects = 0, const D3D12_RECT* clearRect = nullptr) = 0;
		//virtual void CmdDrawImGui() = 0;

		// 常用功能
		//virtual FrameResource* GetCurrFrameResource(FrameResourceType type) = 0;
		//virtual void CreateFrameResource(FrameResourceType type, FrameResourceDesc* desc) = 0;
		//virtual std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers() = 0;
		//virtual ID3D12Fence1* GetDXFence() = 0;
		//virtual uint64_t GetResourceSizeInBytes(ID3D12Resource* pResource) = 0;


		//virtual void FlushCommandQueue() = 0;

		//virtual void CmdCopyTextureToSwapChain(DXTexture2D* tex) = 0;



		//virtual void BeginSingleRenderPass() = 0;
		//virtual void EndSingleRenderPass() = 0;


		//virtual unsigned int GetCurrBackBufferIndex() = 0;
		//virtual std::shared_ptr<ResourceManager> GetResourceManager() = 0;

		//virtual Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRootSignature(Shader* shader, int samplerCount = 0, const D3D12_STATIC_SAMPLER_DESC* samplerDesc = nullptr) = 0;
		//virtual Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc) = 0;
		//virtual std::shared_ptr<VertexBuffer> CreateVertexBuffer(VertexType type, const void* data, UINT64 sizeInBytes) = 0;
		//virtual std::shared_ptr<IndexBuffer> CreateIndexBuffer(const void* data, UINT64 sizeInBytes) = 0;
		//virtual std::shared_ptr<ConstantBuffer> CreateConstantBuffer(unsigned int elementCount, unsigned int singleElementSizeInBytes) = 0;
		//virtual ConstantBufferView* CreateConstantBufferView(const D3D12_CONSTANT_BUFFER_VIEW_DESC* pDesc, ConstantBufferView* thisView = nullptr) = 0;
		//virtual ConstantBufferView* CreateConstantBufferView() = 0;
		//virtual ShaderResourceView* CreateShaderResourceView(DXResource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* pDesc, ShaderResourceView* thisView = nullptr) = 0;
		//virtual ShaderResourceView* CreateShaderResourceView() = 0;
		//virtual UnorderedAccessView* CreateUnorderedAccessView(DXResource* resource, DXResource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* pDesc, UnorderedAccessView* thisView = nullptr) = 0;
		//virtual UnorderedAccessView* CreateUnorderedAccessView() = 0;
		//virtual DepthStencilView* CreateDepthStencilView(DXResource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* pDesc, DepthStencilView* thisView = nullptr) = 0;
		//virtual DepthStencilView* CreateDepthStencilView() = 0;
		//virtual RenderTargetView* CreateRenderTargetView(DXResource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* pDesc, RenderTargetView* thisView = nullptr) = 0;
		//virtual RenderTargetView* CreateRenderTargetView() = 0;
		//virtual SamplerView* CreateSampler(const D3D12_SAMPLER_DESC* pDesc, SamplerView* thisView = nullptr) = 0;
		//virtual SamplerView* CreateSampler() = 0;

		//

		//// 资源相关函数
		//virtual std::shared_ptr<DXTexture2DArray> CreateTexture2DArray(DXTexture2DArrayDesc desc) = 0;
		//virtual std::shared_ptr<DXTextureCube> CreateCubemap(DXTextureCubeDesc desc) = 0;
		//virtual std::shared_ptr<DXTexture2D> CreateTexture2D(DXTexture2DDesc desc) = 0;
		//virtual std::shared_ptr<DXTexture2D> LoadTextureFromFile(const std::wstring& filepath, std::unique_ptr<uint8_t[]>& decodedData, D3D12_SUBRESOURCE_DATA& subresource, size_t maxsize = 0, bool bForceLoadSRGB = false) = 0;
		//virtual std::shared_ptr<DXBuffer> CreateBuffer(DXBufferDesc desc) = 0;
		//virtual std::shared_ptr<DXBuffer> CreateBuffer(DXBufferDesc desc, const void* data, UINT64 sizeInBytes) = 0;
		//virtual void CopyDataCpuToGpu(DXResource* dstResource, const void* data, UINT64 sizeInBytes) = 0;
		//virtual void CopyDataCpuToGpu(DXResource* dstResource, UINT64 startPosInBytes, const void* data, UINT64 sizeInBytes) = 0;
		//virtual void CmdCopyDataGpuToGpu(DXResource* dstResource, DXResource* srcResource) = 0;
		//virtual void CmdCopyDataGpuToGpu(DXResource* dstResource, DXResource* srcResource, UINT64 dstStartPosInBytes, UINT64 srcStartPosInBytes, UINT64 sizeInBytes) = 0;
		//virtual void CmdCopySubResourceDataCpuToGpu(DXResource* dest, DXResource* upload, UINT64 uploadOffsetInBytes, D3D12_SUBRESOURCE_DATA* resources, UINT resourcesStartIdx = 0, UINT resourcesNum = 1) = 0;
		//virtual void CmdCopyTextureSubRegionGpuToGpu(DXResource* dest, DXResource* src, UINT32 destArrayIndex, Vector3i destResourceCoords = {0, 0, 0},
		//	UINT32 srcArrayIndex = 0, Vector3i srcResourceCoordsStart = { 0, 0, 0 }, Vector3i srcResourceCoordsEnd = {-1, -1, -1}) = 0;
		////virtual void CopyTexturesToCubemap(ID3D12Resource* cubemap, DXResource* upload, const std::array<std::shared_ptr<DXTexture2D>, 6>& textures) = 0;
		//virtual void CmdResourceStateTransform(DXResource* resource, D3D12_RESOURCE_STATES stateAfter) = 0;

		//virtual std::shared_ptr<DXTexture2D> GetCurrBackBufferResource() = 0;
		//virtual RenderTargetView* GetCurrBackBufferAsRenderTarget() = 0;
		//virtual ShaderResourceView* GetCurrBackBufferAsShaderResource(const D3D12_SHADER_RESOURCE_VIEW_DESC* pDesc) = 0;

	private:
		// No Private
	};
}
