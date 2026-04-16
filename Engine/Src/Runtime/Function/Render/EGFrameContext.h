#pragma once
#include "Function/Util/GameTimer.h"
#include "RenderCamera.h"
#include "Core/Math/MathFunction.h"
#include <DirectXMath.h>
#include <vector>

#include "RenderItem.h"
#include "Resource/DXResourceHeader.h"
#include "Function/Render/DX12RHI/GpuResource.h"
#include "Function/Render/DX12RHI/DescriptorHeap/Descriptor.h"

namespace photon
{
	class DX12RHI;
	class ResourceManager;
	class GpuResourceManager;
	class DescriptorSystem;
	class FrameAllocatorSystem;
	class CommandContextManager;
	class PipelineStateCache;
	class RootSignatureCache;
	class DX12CommandContext;
	class RenderScene;

	struct FrameUniforms
	{
		GameTimer* timer = nullptr;

		RenderCamera* mainCamera = nullptr;

		DirectX::XMFLOAT4X4 view = Identity4x4();
		DirectX::XMFLOAT4X4 proj = Identity4x4();
		DirectX::XMFLOAT4X4 viewProj = Identity4x4();
		DirectX::XMFLOAT4X4 invView = Identity4x4();
		DirectX::XMFLOAT4X4 invProj = Identity4x4();
		DirectX::XMFLOAT4X4 invViewProj = Identity4x4();
		DirectX::XMFLOAT4X4 prevViewProj = Identity4x4();

		Vector3 camPosWS = {};
		float znear = 0.1f;
		float zfar = 1000.0f;

		Vector2i viewportSize = {};
		Vector2 invViewportSize = {};

		std::vector<DirLight> dirLights;
		std::vector<PointLight> pointLights;
		std::vector<SpotLight> spotLights;

		DirLight* mainDirLight = nullptr;
		PointLight* mainPointLight = nullptr;
	};

	struct FrameServices
	{
		DX12RHI* rhi = nullptr;
		ResourceManager* resourceManager = nullptr;
		GpuResourceManager* gpuResManager = nullptr;
		DescriptorSystem* descriptorSystem = nullptr;
		FrameAllocatorSystem* frameAllocator = nullptr;
		CommandContextManager* cmdCtxMgr = nullptr;
		PipelineStateCache* pipelineCache = nullptr;
		RootSignatureCache* rootSignatureCache = nullptr;
		DX12CommandContext* graphicsCmd = nullptr;
	};

	struct FrameRenderLists
	{
		std::vector<const RenderItem*> all;
		std::vector<const RenderItem*> opaque;
		std::vector<const RenderItem*> transparent;
		std::vector<const RenderItem*> shadowCasters;
	};

	struct FrameRenderTargets
	{
		TextureHandle sceneColor = {};
		TextureHandle sceneDepth = {};
	};

	struct EG_FrameContext
	{
		uint32_t frameIndex = 0;

		RenderScene* renderScene = nullptr;

		FrameUniforms uniforms = {};
		FrameServices services = {};
		FrameRenderLists renderlists = {};
		FrameRenderTargets targets = {};

		//std::shared_ptr<DXTextureCube> skybox;
		TextureHandle skybox = {};
	};
}