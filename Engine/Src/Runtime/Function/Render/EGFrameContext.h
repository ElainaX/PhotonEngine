#pragma once 
#include "Function/Util/GameTimer.h"
#include "RenderCamera.h"
#include "Core/Math/Vector4.h"
#include <DirectXMath.h>
#include <memory>

#include "Function/Render/Shader/ShaderFactory.h"
#include "Function/Render/GeometryGenerator.h"
#include "ResourceManager.h"
#include "RenderObject/RenderItem.h"
#include "Resource/Texture/Cubemap.h"

namespace photon
{
struct FrameUniforms
{
	GameTimer* timer;

	// main camera data
	RenderCamera* mainCamera;
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 proj;
	DirectX::XMFLOAT4X4 viewProj;
	DirectX::XMFLOAT4X4 invView;
	DirectX::XMFLOAT4X4 invProj;
	DirectX::XMFLOAT4X4 invViewProj;
	DirectX::XMFLOAT4X4 prevViewProj;

	Vector3 camPosWS;
	float znear, zfar;
	Vector2i viewportSize;
	Vector2 invViewportSize;


	// light
	std::vector<DirLight> dirLights;
	std::vector<PointLight> pointLights;
	std::vector<SpotLight> spotLights;

	DirLight* mainDirLight;
	PointLight* mainPointLight;


	StaticModelFrameResource* staticFrameResource;
};


struct FrameServices
{
	ShaderFactory* shaderFactory;
	GeometryGenerator* geoGen;
	ResourceManager* resMgr;
	RHI* rhi;
	RenderMeshCollection* innerMeshCollection;
};


struct FrameRenderLists
{
	std::vector<CommonRenderItem*> allRitems;
	std::vector<CommonRenderItem*> opaque;
	std::vector<CommonRenderItem*> transparent;
	std::vector<CommonRenderItem*> shadowCasters;
	std::unordered_map<FrameResource*, std::vector<std::shared_ptr<CommonRenderItem>>> innerRitems;
};


// EG 就是Engine的意思，与DX12中的FrameContext区别开来
struct EG_FrameContext
{
	FrameUniforms uniforms;
	FrameServices services;
	FrameRenderLists renderlists;

	// frame outputs/inputs
	std::shared_ptr<Texture2D> backBuffer;
	std::shared_ptr<Texture2D> depthStencilBuffer;
	std::shared_ptr<Cubemap> skybox;
};

}