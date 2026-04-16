#include "ImGuiSystem.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_win32.h"
#include "ImGui/imgui_impl_dx12.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace photon
{
	bool ImGuiSystem::Initialize(DX12RHI* rhi, WindowSystem* windowSystem)
	{
		if (!rhi || !windowSystem)
			return false;

		m_rhi = rhi;
		m_windowSystem = windowSystem;

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
		heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		heapDesc.NumDescriptors = m_capacity;
		heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		heapDesc.NodeMask = 0;

		HRESULT hr = m_rhi->GetDevice()->CreateDescriptorHeap(
			&heapDesc, IID_PPV_ARGS(m_srvHeap.GetAddressOf()));
		if (FAILED(hr))
			return false;

		m_descriptorSize =
			m_rhi->GetDevice()->GetDescriptorHandleIncrementSize(
				D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		m_freeList.clear();
		m_freeList.reserve(m_capacity);
		for (uint32_t i = 0; i < m_capacity; ++i)
		{
			m_freeList.push_back(m_capacity - 1 - i);
		}

		ImGui_ImplWin32_Init(m_windowSystem->GetHwnd());

		m_windowSystem->RegisterBeforeAllEventCallBack(
			[](Win32WndProcInfo& procInfo, bool& bContinue)
			{
				if (ImGui_ImplWin32_WndProcHandler(
					procInfo.hwnd, procInfo.msg, procInfo.wparam, procInfo.lparam))
				{
					bContinue = false;
				}
			});

		ImGui_ImplDX12_InitInfo initInfo = {};
		initInfo.Device = m_rhi->GetDevice();
		initInfo.CommandQueue = m_rhi->GetGraphicsQueue();
		initInfo.NumFramesInFlight = FrameSyncSystem::kMaxFramesInFlight;
		initInfo.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		initInfo.DSVFormat = DXGI_FORMAT_UNKNOWN;
		initInfo.SrvDescriptorHeap = m_srvHeap.Get();
		initInfo.SrvDescriptorAllocFn = &ImGuiSystem::ImGuiSrvAlloc;
		initInfo.SrvDescriptorFreeFn = &ImGuiSystem::ImGuiSrvFree;
		initInfo.UserData = this;

		ImGui_ImplDX12_Init(&initInfo);

		return true;
	}

	void ImGuiSystem::Shutdown()
	{
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		m_srvHeap.Reset();
		m_freeList.clear();
		m_rhi = nullptr;
		m_windowSystem = nullptr;
	}

	void ImGuiSystem::BeginFrame()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiSystem::EndFrame()
	{
		ImGui::Render();
	}

	ImTextureID ImGuiSystem::RegisterExternalSrv(DescriptorSystem* descriptorSystem, DescriptorHandle srv)
	{
		if (!descriptorSystem || !srv.IsValid())
			return 0;

		uint32_t idx = AllocateSrvIndex();

		D3D12_CPU_DESCRIPTOR_HANDLE dst = GetCpuHandle(idx);
		descriptorSystem->CopyToCpuHandle(srv, dst);

		D3D12_GPU_DESCRIPTOR_HANDLE gpu = GetGpuHandle(idx);
		ImTextureID texId = static_cast<ImTextureID>(gpu.ptr);

		m_externalTexSlots[texId] = idx;
		return texId;
	}

	void ImGuiSystem::UnregisterExternalSrv(ImTextureID texId)
	{
		auto it = m_externalTexSlots.find(texId);
		if (it == m_externalTexSlots.end())
			return;

		FreeSrvIndex(it->second);
		m_externalTexSlots.erase(it);
	}

	void ImGuiSystem::Render(DX12CommandContext& cmd)
	{
		ID3D12DescriptorHeap* heaps[] = { m_srvHeap.Get() };
		cmd.SetDescriptorHeaps({ heaps[0] });

		ImGui_ImplDX12_RenderDrawData(
			ImGui::GetDrawData(),
			cmd.GetNativeCommandList());

		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}

	uint32_t ImGuiSystem::AllocateSrvIndex()
	{
		PHOTON_ASSERT(!m_freeList.empty(), "ImGuiSystem SRV heap exhausted.");
		uint32_t idx = m_freeList.back();
		m_freeList.pop_back();
		return idx;
	}

	void ImGuiSystem::FreeSrvIndex(uint32_t index)
	{
		PHOTON_ASSERT(index < m_capacity, "Invalid ImGui SRV heap index.");
		m_freeList.push_back(index);
	}

	D3D12_CPU_DESCRIPTOR_HANDLE ImGuiSystem::GetCpuHandle(uint32_t index) const
	{
		auto base = m_srvHeap->GetCPUDescriptorHandleForHeapStart();
		base.ptr += static_cast<SIZE_T>(index) * m_descriptorSize;
		return base;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE ImGuiSystem::GetGpuHandle(uint32_t index) const
	{
		auto base = m_srvHeap->GetGPUDescriptorHandleForHeapStart();
		base.ptr += static_cast<UINT64>(index) * m_descriptorSize;
		return base;
	}

	void ImGuiSystem::ImGuiSrvAlloc(
		ImGui_ImplDX12_InitInfo* info,
		D3D12_CPU_DESCRIPTOR_HANDLE* outCpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE* outGpuHandle)
	{
		auto* self = static_cast<ImGuiSystem*>(info->UserData);
		uint32_t idx = self->AllocateSrvIndex();
		*outCpuHandle = self->GetCpuHandle(idx);
		*outGpuHandle = self->GetGpuHandle(idx);
	}

	void ImGuiSystem::ImGuiSrvFree(
		ImGui_ImplDX12_InitInfo* info,
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
	{
		auto* self = static_cast<ImGuiSystem*>(info->UserData);
		auto base = self->m_srvHeap->GetCPUDescriptorHandleForHeapStart();
		uint32_t idx = static_cast<uint32_t>(
			(cpuHandle.ptr - base.ptr) / self->m_descriptorSize);
		self->FreeSrvIndex(idx);
	}
}