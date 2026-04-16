#pragma once
#include <vector>
#include <map>
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <ImGui/imgui_impl_dx12.h>

#include "Function/Render/DX12RHI/DX12RHI.h"
#include "Function/Render/WindowSystem.h"
#include "Function/Render/DX12RHI/DX12CommandContext.h"
#include "Function/Render/DX12RHI/DescriptorHeap/Descriptor.h"
#include "Function/Render/DX12RHI/DescriptorHeap/DescriptorSystem.h"

namespace photon
{
	class ImGuiSystem
	{
	public:
		bool Initialize(DX12RHI* rhi, WindowSystem* windowSystem);
		void Shutdown();

		void BeginFrame();
		void EndFrame();

		void Render(DX12CommandContext& cmd);

		ID3D12DescriptorHeap* GetSrvHeap() const { return m_srvHeap.Get(); }

		// 把外部 persistent SRV 拷到 ImGui 自己的 heap 里，返回可给 ImGui::Image 用的 ImTextureID
		ImTextureID RegisterExternalSrv(DescriptorSystem* descriptorSystem, DescriptorHandle srv);
		void UnregisterExternalSrv(ImTextureID texId);

	private:
		uint32_t AllocateSrvIndex();
		void FreeSrvIndex(uint32_t index);

		D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32_t index) const;
		D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(uint32_t index) const;

		static void ImGuiSrvAlloc(
			ImGui_ImplDX12_InitInfo* info,
			D3D12_CPU_DESCRIPTOR_HANDLE* outCpuHandle,
			D3D12_GPU_DESCRIPTOR_HANDLE* outGpuHandle);

		static void ImGuiSrvFree(
			ImGui_ImplDX12_InitInfo* info,
			D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
			D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle);

	private:
		DX12RHI* m_rhi = nullptr;
		WindowSystem* m_windowSystem = nullptr;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvHeap;

		uint32_t m_descriptorSize = 0;
		uint32_t m_capacity = 64;
		std::vector<uint32_t> m_freeList;

		// 记录外部贴图注册到 ImGui heap 后占用的 slot
		std::map<ImTextureID, uint32_t> m_externalTexSlots;
	};
}