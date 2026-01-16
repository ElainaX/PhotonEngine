#include "MainCameraPass.h"
#include "Function/Global/RuntimeGlobalContext.h"
#include "Function/Render/GeometryGenerator.h"
#include "Function/Render/RenderSystem.h"
#include "Function/Util/RenderUtil.h"
#include "Function/Render/Shader/ShaderFactory.h"

namespace photon 
{
	void MainCameraPass::Initialize(RHI* rhi, WindowSystem* windowSystem)
	{
		m_Rhi = rhi;
		m_WindowSystem = windowSystem;

		m_PassConstantsIdx = StaticModelPassConstants::s_CurrPassIndex++;


		// SkyBox For MainCamera
		auto skyboxShader = g_RuntimeGlobalContext.renderSystem->GetShaderFactory()->Create(L"Skybox");
		auto geometryGen = g_RuntimeGlobalContext.renderSystem->GetGeometryGenerator();

		auto boxes = geometryGen->CreateBox(0.3f, 0.3f, 0.3f, 0);
		std::vector<VertexSimple> boxVert(boxes.Vertices.size());
		for(int i = 0; i < boxVert.size(); ++i)
		{
			boxVert[i].position = boxes.Vertices[i].Position;
		}

		MeshDesc lightDesc;
		lightDesc.name = L"light";
		lightDesc.type = VertexType::VertexSimple;
		lightDesc.vertexRawData = RenderUtil::CreateD3DBlob(boxVert.data(), sizeof(VertexSimple) * boxVert.size());
		lightDesc.indexRawData = RenderUtil::CreateD3DBlob(boxes.Indices32.data(), sizeof(UINT32) * boxes.Indices32.size());
		m_LightMesh = g_RuntimeGlobalContext.renderSystem->GetResourceManager()->CreateMesh(lightDesc);

		auto skybox = geometryGen->CreateBox(1.0f, 1.0f, 1.0f, 0);
		std::vector<VertexSimple> skyboxVert(skybox.Vertices.size());
		for (int i = 0; i < skyboxVert.size(); ++i)
		{
			skyboxVert[i].position = skybox.Vertices[i].Position;
		}
		MeshDesc skyboxDesc;
		skyboxDesc.name = L"Skybox";
		skyboxDesc.type = VertexType::VertexSimple;
		skyboxDesc.vertexRawData = RenderUtil::CreateD3DBlob(skyboxVert.data(), sizeof(VertexSimple) * skyboxVert.size());
		skyboxDesc.indexRawData = RenderUtil::CreateD3DBlob(skybox.Indices32.data(), sizeof(UINT32) * skybox.Indices32.size());
		auto skyboxMesh = g_RuntimeGlobalContext.renderSystem->GetResourceManager()->CreateMesh(skyboxDesc);

		auto skyboxRenderItem = std::make_shared<CommonRenderItem>();
		skyboxRenderItem->shader = skyboxShader;
		skyboxRenderItem->meshCollection = &m_MeshCollection;
		skyboxRenderItem->meshGuid = skyboxMesh->guid;
		skyboxRenderItem->primitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		m_InnerCommonRenderItems["Skybox"] = skyboxRenderItem;

		m_MeshCollection.PushMesh(m_LightMesh);
		m_MeshCollection.PushMesh(skyboxMesh);
		m_MeshCollection.EndPush(rhi);

		m_DebugDrawLightShader = g_RuntimeGlobalContext.renderSystem->GetShaderFactory()->Create(L"DebugDrawLight");

		//m_PassConstants.color = Vector4{ 0.2, 0.5, 0.0, 1.0 };

		m_TestSubpass = std::make_shared<TestSubPass>();
		m_TestSubpass->Initialize(rhi);

		m_DebugDrawSubpass = std::make_shared<DebugDrawSubPass>();
		m_DebugDrawSubpass->Initialize(rhi);

		m_UISubpass = std::make_shared<UISubPass>();
		m_UISubpass->Initialize(rhi);

		m_SkyboxSubpass = std::make_shared<DrawSkyboxSubPass>();
		m_SkyboxSubpass->Initialize(rhi);
	}

	void MainCameraPass::PrepareContext(RenderResourceData* data)
	{


		MainPassRenderResourceData* renderResource = dynamic_cast<MainPassRenderResourceData*>(data);

		auto renderCamera = renderResource->mainCamera;
		auto viewMat = renderCamera->GetViewMatrix();
		auto viewDet = XMMatrixDeterminant(viewMat);
		auto invViewMat = XMMatrixInverse(&viewDet, viewMat);
		auto projMat = renderCamera->GetProjMatrix();
		auto projDet = XMMatrixDeterminant(projMat);
		auto invProjMat = XMMatrixInverse(&projDet, projMat);
		auto viewProj = XMMatrixMultiply(viewMat, projMat);
		auto invViewProj = XMMatrixMultiply(invProjMat, invViewMat);
		Vector3 eyePos = renderCamera->pos;
		Vector2 renderTargetSize = Vector2((float)m_WindowSystem->GetViewportSize().x, 
			(float)m_WindowSystem->GetViewportSize().y);
		Vector2 invRenderTargetSize = 1.0f / renderTargetSize;
		float znear = renderCamera->znear;
		float zfar = renderCamera->zfar;
		float totalTime = renderResource->gameTimer->TotalTime();
		float deltaTime = renderResource->gameTimer->DeltaTime();

		XMStoreFloat4x4(&m_PassConstants.view, viewMat);
		XMStoreFloat4x4(&m_PassConstants.invView, invViewMat);
		XMStoreFloat4x4(&m_PassConstants.proj, projMat);
		XMStoreFloat4x4(&m_PassConstants.invProj, invProjMat);
		XMStoreFloat4x4(&m_PassConstants.viewProj, viewProj);
		XMStoreFloat4x4(&m_PassConstants.invViewProj, invViewProj);
		m_PassConstants.eyePos = eyePos;
		m_PassConstants.renderTargetSize = renderTargetSize;
		m_PassConstants.invRenderTargetSize = invRenderTargetSize;
		m_PassConstants.znear = znear; 
		m_PassConstants.zfar = zfar;
		m_PassConstants.totalTime = totalTime;
		m_PassConstants.deltaTime = deltaTime;
		m_PassConstants.ambientLight = { 0.001, 0.001, 0.001, 1.0 };


		int index = 0;
		std::vector<CommonRenderItem*> lightRenderItems;

		StaticModelFrameResource* frameResource = (StaticModelFrameResource*)m_Rhi->GetCurrFrameResource(FrameResourceType::StaticModelFrameResource);
		
		// 这里是为了渲染Debug用的灯光模型
		for (int i = 0; i < renderResource->directionalLights.size(); ++i)
		{
			renderResource->directionalLights[i]->direction.normalise();
			m_PassConstants.lights[index] = *renderResource->directionalLights[i];
			lightRenderItems.push_back(CreateLightRenderItem(renderResource->directionalLights[i]));
			frameResource->UpdateObjectConstantBuffer(lightRenderItems.back()->objConstantIdx, &lightRenderItems.back()->objectConstants);
			index++;
		}
		for (int i = 0; i < renderResource->pointLights.size(); ++i)
		{
			renderResource->directionalLights[i]->direction.normalise();
			m_PassConstants.lights[index] = *renderResource->pointLights[i];
			lightRenderItems.push_back(CreateLightRenderItem(renderResource->pointLights[i]));
			frameResource->UpdateObjectConstantBuffer(lightRenderItems.back()->objConstantIdx, &lightRenderItems.back()->objectConstants);
			index++;
		}
		for (int i = 0; i < renderResource->spotLights.size(); ++i)
		{
			renderResource->directionalLights[i]->direction.normalise();
			m_PassConstants.lights[index] = *renderResource->spotLights[i];
			lightRenderItems.push_back(CreateLightRenderItem(renderResource->spotLights[i]));
			frameResource->UpdateObjectConstantBuffer(lightRenderItems.back()->objConstantIdx, &lightRenderItems.back()->objectConstants);
			index++;
		}

		// Update FramePassConstants
		frameResource->UpdatePassConstantBuffer(m_PassConstantsIdx, &m_PassConstants);



		m_TestRenderTargetView = m_Rhi->CreateRenderTargetView(renderResource->renderTarget.get(), nullptr, m_TestRenderTargetView);
		m_TestDepthStencilView = m_Rhi->CreateDepthStencilView(renderResource->depthStencil.get(), nullptr, m_TestDepthStencilView);

		TestSubPassData TestSubpassData;
		TestSubpassData.renderTargetView = m_TestRenderTargetView;
		TestSubpassData.depthStencilView = m_TestDepthStencilView;
		TestSubpassData.renderItems = renderResource->allRenderItems;
		TestSubpassData.shader = TestSubpassData.renderItems[0]->shader;
		TestSubpassData.macros = { MacroInfo{"MaxLights", std::to_string(MaxLights)},
									MacroInfo{"NumDirLights", std::to_string(renderResource->directionalLights.size())},
									MacroInfo{"NumPointLights", std::to_string(renderResource->pointLights.size())},
									MacroInfo{"NumSpotLights", std::to_string(renderResource->spotLights.size())},
									MacroInfo{"EnableNormalMap", ""},
									MacroInfo{"EnableRoughnessMap", ""} };
		TestSubpassData.passConstantIdx = m_PassConstantsIdx;

		m_TestSubpass->PrepareForData(&TestSubpassData);


		DebugDrawLightPassData debugSubpassData;
		debugSubpassData.renderTargetView = m_TestRenderTargetView;
		debugSubpassData.depthStencilView = m_TestDepthStencilView;
		debugSubpassData.lightRenderItems = std::move(lightRenderItems);
		debugSubpassData.passConstantIdx = m_PassConstantsIdx;
		m_DebugDrawSubpass->PrepareForData(&debugSubpassData);


		DrawSkyboxSubPassData drawSkyboxData;
		drawSkyboxData.cubemap = renderResource->cubemap;
		drawSkyboxData.renderTargetView = m_TestRenderTargetView;
		drawSkyboxData.depthStencilView = m_TestDepthStencilView;
		drawSkyboxData.skyboxRenderItem = m_InnerCommonRenderItems["Skybox"].get();
		drawSkyboxData.passConstantIdx = m_PassConstantsIdx;
		m_SkyboxSubpass->PrepareForData(&drawSkyboxData);


		UISubPassData UISubpassData;
		UISubpassData.depthStencilView = m_TestDepthStencilView;
		UISubpassData.renderTargetView = m_Rhi->GetCurrBackBufferAsRenderTarget();
		m_UISubpass->PrepareForData(&UISubpassData);
	}

	void MainCameraPass::Draw()
	{
		auto [width, height] = m_WindowSystem->GetClientWidthAndHeight();
		D3D12_RECT scissorRect = { 0, 0, width, height };
		D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f};
		
		if(!m_bOnlyUI)
		{
			m_TestSubpass->Draw(scissorRect, viewport);
			m_DebugDrawSubpass->Draw(scissorRect, viewport);
			m_SkyboxSubpass->Draw(scissorRect, viewport);
		}
		m_UISubpass->Draw(scissorRect, viewport);


		//m_Rhi->CopyTextureToSwapChain(dynamic_cast<Texture2D*>(m_TestRenderTargetView->resource));
		m_Rhi->PrepareForPresent();
		m_Rhi->Present();
	}

	void MainCameraPass::OnlyUI(bool bOnlyUI)
	{
		m_bOnlyUI = bOnlyUI;
	}

	photon::CommonRenderItem* MainCameraPass::CreateLightRenderItem(LightData* light)
	{
		using namespace DirectX;
		std::shared_ptr<CommonRenderItem> ritem;
		if(!m_LightRenderItems.contains(light))
		{
			ritem = std::make_shared<CommonRenderItem>();
			ritem->meshCollection = &m_MeshCollection;
			ritem->meshGuid = m_LightMesh->guid;
			ritem->shader = m_DebugDrawLightShader;
			ritem->objConstantIdx = StaticModelObjectConstants::s_CurrObjectIndex++;
			m_LightRenderItems[light] = ritem;
		}
		else 
		{
			ritem = m_LightRenderItems[light];
		}

		XMMATRIX worldMat = XMMatrixTranslation(light->position.x, light->position.y, light->position.z);
		XMStoreFloat4x4(&ritem->objectConstants.world, worldMat);

		return ritem.get();
	}

}