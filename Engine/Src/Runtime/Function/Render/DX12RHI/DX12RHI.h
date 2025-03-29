#pragma once
#include <dxgi1_4.h>
#include <d3d12.h>
#include <DirectXColors.h>
#include <wrl.h>
#include <array>
#include <unordered_map>



#include "d3dx12.h"
#include "Function/Render/RHI.h"
#include "DX12Define.h"
#include "Function/Render/WindowSystem.h"
#include "Resource/Texture/Texture2D.h"
#include "Core/Math/Vector4.h"
#include "DX12Resource/VertexType.h"
#include "DX12Resource/VertexBuffer.h"
#include "DX12Resource/IndexBuffer.h"
#include "DX12Resource/VertexLayout.h"
#include "../RenderObject/RenderItem.h"
#include "DescriptorHeap/CbvSrvUavDescriptorHeap.h"
#include "DescriptorHeap/DsvDescriptorHeap.h"
#include "DescriptorHeap/RtvDescriptorHeap.h"
#include "DescriptorHeap/SamplerDescriptorHeap.h"
#include "DXPipeline/DXGraphicsPipeline.h"
#include "FrameResource/FrameResource.h"
#include "FrameResource/StaticModelFrameResource.h"

namespace photon 
{
	class ResourceManager;


	struct FrameContext
	{
		//std::shared_ptr<Buffer> bigConstantBuffer = nullptr;
		//std::shared_ptr<Buffer> bigUploadConstantBuffer = nullptr;
		std::unordered_map<FrameResourceType, std::shared_ptr<FrameResource>> frameResources;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator = nullptr;
		UINT64 fenceValue = 0;
	};

	struct SwapChainContent
	{
		std::shared_ptr<Texture2D> backBuffer = nullptr;
		RenderTargetView* view = nullptr;
	};

	class DX12RHI : public RHI 
	{
	public:
		virtual ~DX12RHI() override;

		void Initialize(RHIInitInfo initializeInfo) override final;
		void CreateSwapChain() override final;
		void ReCreateSwapChain() override final;
		void CreateFactory() override final;
		void CreateDevice() override final;
		void CreateFence() override final;


		void CreateCommandObjects() override final;
		void CreateSwapChainRenderTarget() override final;
		void CreateDescriptorHeaps() override final;
		void CreateAssetAllocator() override final;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateRootSignature(Shader* shader, int samplerCount = 0, const D3D12_STATIC_SAMPLER_DESC* samplerDesc = nullptr) override;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc) override;
		std::shared_ptr<VertexBuffer> CreateVertexBuffer(VertexType type, const void* data, UINT64 sizeInBytes) override;
		std::shared_ptr<IndexBuffer> CreateIndexBuffer(const void* data, UINT64 sizeInBytes) override;
		std::shared_ptr<ConstantBuffer> CreateConstantBuffer(unsigned int elementCount, unsigned int singleElementSizeInBytes) override;
		ConstantBufferView* CreateConstantBufferView(const D3D12_CONSTANT_BUFFER_VIEW_DESC* pDesc, ConstantBufferView* thisView = nullptr) override;
		ConstantBufferView* CreateConstantBufferView() override;
		ShaderResourceView* CreateShaderResourceView(Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC* pDesc, ShaderResourceView* thisView = nullptr) override;
		ShaderResourceView* CreateShaderResourceView() override;
		UnorderedAccessView* CreateUnorderedAccessView(Resource* resource, Resource* counterResource, const D3D12_UNORDERED_ACCESS_VIEW_DESC* pDesc, UnorderedAccessView* thisView = nullptr)override;
		UnorderedAccessView* CreateUnorderedAccessView()override;
		DepthStencilView* CreateDepthStencilView(Resource* resource, const D3D12_DEPTH_STENCIL_VIEW_DESC* pDesc, DepthStencilView* thisView = nullptr)override;
		DepthStencilView* CreateDepthStencilView()override;
		RenderTargetView* CreateRenderTargetView(Resource* resource, const D3D12_RENDER_TARGET_VIEW_DESC* pDesc, RenderTargetView* thisView = nullptr)override;
		RenderTargetView* CreateRenderTargetView()override;
		SamplerView* CreateSampler(const D3D12_SAMPLER_DESC* pDesc, SamplerView* thisView = nullptr) override;
		SamplerView* CreateSampler() override;

		void FlushCommandQueue() override final;
		void WaitForFenceValue(uint64_t fenceValue) override final;
		void CreateDebugManager() override final;

		void Clear() override;

		unsigned int GetCurrBackBufferIndex() override final;
		std::shared_ptr<ResourceManager> GetResourceManager() override;


		std::shared_ptr<Texture2D> CreateTexture2D(Texture2DDesc desc) override final;
		std::shared_ptr<Buffer> CreateBuffer(BufferDesc desc) override final;
		std::shared_ptr<Buffer> CreateBuffer(BufferDesc desc, const void* data, UINT64 sizeInBytes) override final;
		std::shared_ptr<Texture2D> LoadTextureFromFile(const std::wstring& filepath, std::unique_ptr<uint8_t[]>& decodedData, D3D12_SUBRESOURCE_DATA& subresource, size_t maxsize = 0) override;


		void CopyDataCpuToGpu(Resource* dstResource, const void* data, UINT64 sizeInBytes) override;
		void CopyDataCpuToGpu(Resource* dstResource, UINT64 startPosInBytes, const void* data, UINT64 sizeInBytes) override;
		void CopyDataGpuToGpu(Resource* dstResource, Resource* srcResource) override;
		void CopyDataGpuToGpu(Resource* dstResource, Resource* srcResource, UINT64 dstStartPosInBytes, UINT64 srcStartPosInBytes, UINT64 sizeInBytes) override;
		void CopySubResourceDataCpuToGpu(Resource* dest, Resource* upload, UINT64 uploadOffsetInBytes, D3D12_SUBRESOURCE_DATA* resources, UINT resourcesStartIdx = 0, UINT resourcesNum = 1) override;

		void CopyTextureToSwapChain(Texture2D* tex) override;
		void Present() override;


		void BeginSingleRenderPass() override;
		void EndSingleRenderPass() override;


		void ResourceStateTransform(Resource* resource, D3D12_RESOURCE_STATES stateAfter) override;
		void PrepareForPresent() override;



		void CmdSetViewportsAndScissorRects(D3D12_RECT scissorRect, D3D12_VIEWPORT viewport) override;
		void CmdSetPipelineState(ID3D12PipelineState* pipeline) override;
		void CmdSetGraphicsRootSignature(ID3D12RootSignature* rootSignature) override;
		void CmdSetRenderTargets(UINT numRenderTargetViews, const D3D12_CPU_DESCRIPTOR_HANDLE* pRenderTargetDescriptors, bool RTsSingleHandleToDescriptorRange, const D3D12_CPU_DESCRIPTOR_HANDLE* pDepthStencilDescriptor) override;
		void CmdSetDescriptorHeaps(const std::vector<ID3D12DescriptorHeap*>& descriptorHeaps) override;
		void CmdSetDescriptorHeaps() override;
		void CmdSetGraphicsRootDescriptorTable(UINT RootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE BaseDescriptor) override;
		void CmdSetGraphicsRootConstantBufferView(UINT RootParameterIndex, D3D12_GPU_VIRTUAL_ADDRESS BufferLocation) override;
		void CmdSetVertexBuffers(UINT startSlot, UINT numViews, const D3D12_VERTEX_BUFFER_VIEW* pViews) override;
		void CmdSetIndexBuffer(const D3D12_INDEX_BUFFER_VIEW* pView) override;
		void CmdSetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY PrimitiveTopology) override;
		void CmdDrawIndexedInstanced(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation) override;
		void CmdClearRenderTarget(RenderTargetView* view, Vector4 clearRGBA, UINT numRects = 0, const D3D12_RECT* clearRect = nullptr) override;
		void CmdClearDepthStencil(DepthStencilView* view, D3D12_CLEAR_FLAGS ClearFlags, float depth, UINT8 stencil, UINT numRects = 0, const D3D12_RECT* clearRect = nullptr) override;


		FrameResource* GetCurrFrameResource(FrameResourceType type) override;
		void CreateFrameResource(FrameResourceType type, FrameResourceDesc* desc) override;
		std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers() override;







	private:
		Texture2D* GetCurrBackBufferResource() { return m_SwapChainContents[m_CurrBackBufferIndex].backBuffer.get(); }
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> GetCurrFrameContextCmdAllocator() { return m_FrameContexts[m_CurrFrameContextIndex]->cmdAllocator; }
		void OnWindowResize(const WindowResizeEvent& e);

	private:
		UINT64 m_FenceValue = 0;
		uint32_t m_CurrBackBufferIndex = 0;
		uint32_t m_CurrFrameContextIndex = 0;
		Vector4 m_ClearColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		std::shared_ptr<RtvDescriptorHeap> m_RtvHeap;
		std::shared_ptr<DsvDescriptorHeap> m_DsvHeap;
		std::shared_ptr<CbvSrvUavDescriptorHeap> m_CbvUavSrvHeap;
		std::shared_ptr<SamplerDescriptorHeap> m_SamplerHeap;

		std::shared_ptr<ResourceManager> m_ResourceManager;

		FrameContext* m_CurrFrameContext = nullptr;

		// temp resource
		// delete when has more class
		//uint32_t m_RtvDescriptorSize = 0;
		//uint32_t m_DsvDescriptorSize = 0;
		//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
		//Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvHeap;



		Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory;
		Microsoft::WRL::ComPtr <IDXGIAdapter3> m_Adapter;
		Microsoft::WRL::ComPtr<ID3D12Device3> m_Device;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_SwapChain;
		std::array<SwapChainContent, g_SwapChainCount> m_SwapChainContents;
		HANDLE m_SwapChainWaitableObject;


		Microsoft::WRL::ComPtr<ID3D12Fence1> m_Fence; 
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CmdQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_MainCmdAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> m_MainCmdList;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> m_FrameResourceCmdList;
		ID3D12GraphicsCommandList6* m_CurrCmdList;
		std::array<std::shared_ptr<FrameContext>, g_FrameContextCount> m_FrameContexts;

		std::shared_ptr<WindowSystem> m_WindowSystem;

	};
}