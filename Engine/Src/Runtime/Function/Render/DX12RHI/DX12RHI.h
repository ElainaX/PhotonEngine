#pragma once
#include <dxgi1_4.h>
#include <d3d12.h>
#include <DirectXColors.h>
#include <wrl.h>
#include <array>


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
#include "../RenderObject/RenderMeshCollection.h"
#include "../RenderObject/RenderItem.h"

namespace photon 
{
	class ResourceManager;


	struct FrameContext
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> cmdAllocator = nullptr;
		UINT64 fenceValue = 0;
	};

	struct SwapChainContent
	{
		std::shared_ptr<Texture2D> backBuffer = nullptr;
		D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptor;
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


		void FlushCommandQueue() override final;
		void WaitForFenceValue(uint64_t fenceValue) override final;
		void CreateDebugManager() override final;

		void Clear() override;

		unsigned int GetCurrBackBufferIndex() override final;


		std::shared_ptr<Texture2D> CreateTexture2D(Texture2DDesc desc) override final;
		std::shared_ptr<Buffer> CreateBuffer(BufferDesc desc) override final;
		std::shared_ptr<Buffer> CreateBuffer(BufferDesc desc, const void* data, UINT64 sizeInBytes) override final;
		void CompileShaders();

		void CopyDataCpuToGpu(Resource* dstResource, const void* data, UINT64 sizeInBytes) override;
		void CopyDataGpuToGpu(Resource* dstResource, Resource* srcResource) override;

		void CopyTextureToSwapChain(std::shared_ptr<Texture2D> tex) override;
		void Present() override;


		void BeginSingleRenderPass() override;
		void EndSingleRenderPass() override;


		void ResourceStateTransform(Resource* resource, D3D12_RESOURCE_STATES stateAfter) override;


		void PrepareForPresent() override;


		void TestRender() override;


	private:
		Texture2D* GetCurrBackBufferResource() { return m_SwapChainContents[m_CurrBackBufferIndex]->backBuffer.get(); }

		void OnWindowResize(const WindowResizeEvent& e);

	private:
		UINT64 m_FenceValue = 0;
		uint32_t m_CurrBackBufferIndex = 0;
		uint32_t m_CurrFrameResourceIndex = 0;
		Vector4 m_ClearColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		// temp resource
		// delete when has more class
		uint32_t m_RtvDescriptorSize = 0;
		uint32_t m_DsvDescriptorSize = 0;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DsvHeap;
		Microsoft::WRL::ComPtr<ID3DBlob> m_VertexShaderBlob;
		Microsoft::WRL::ComPtr<ID3DBlob> m_PixelShaderBlob;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_RootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_PipelineState;
		std::shared_ptr<Texture2D> m_RenderTex;
		std::shared_ptr<Texture2D> m_DepthStencilTex;
		std::shared_ptr<ResourceManager> m_ResourceManager;
		std::shared_ptr<RenderMeshCollection> m_RenderMeshCollection;
		OpaqueRenderItem m_RenderItem;
		



		Microsoft::WRL::ComPtr<IDXGIFactory4> m_Factory;
		Microsoft::WRL::ComPtr <IDXGIAdapter3> m_Adapter;
		Microsoft::WRL::ComPtr<ID3D12Device3> m_Device;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_SwapChain;
		std::array<std::shared_ptr<SwapChainContent>, g_SwapChainCount> m_SwapChainContents;
		HANDLE m_SwapChainWaitableObject;


		Microsoft::WRL::ComPtr<ID3D12Fence1> m_Fence; 
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CmdQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_MainCmdAllocator;
		std::array<std::shared_ptr<FrameContext>, g_FrameResourceCount> m_FrameContexts;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> m_MainCmdList;

		std::shared_ptr<WindowSystem> m_WindowSystem;

	};
}