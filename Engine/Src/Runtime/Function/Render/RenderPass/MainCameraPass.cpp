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

	void photon::MainCameraPass::PrepareContext(RenderResourceData* data)
	{


		MainPassRenderResourceData* renderResource = dynamic_cast<MainPassRenderResourceData*>(data);
		EG_FrameContext* frame = renderResource->frame;
		PassBlackboard* bb = renderResource->bb;

		//auto renderCamera = frame->uniforms.mainCamera;
		//auto viewMat = frame->uniforms.view;
		//auto viewDet = XMMatrixDeterminant(viewMat);
		//auto invViewMat = XMMatrixInverse(&viewDet, viewMat);
		//auto projMat = renderCamera->GetProjMatrix();
		//auto projDet = XMMatrixDeterminant(projMat);
		//auto invProjMat = XMMatrixInverse(&projDet, projMat);
		//auto viewProj = XMMatrixMultiply(viewMat, projMat);
		//auto invViewProj = XMMatrixMultiply(invProjMat, invViewMat);
		//Vector3 eyePos = renderCamera->pos;
		//Vector2 renderTargetSize = Vector2((float)m_WindowSystem->GetViewportSize().x, 
		//	(float)m_WindowSystem->GetViewportSize().y);
		//Vector2 invRenderTargetSize = 1.0f / renderTargetSize;
		//float znear = renderCamera->znear;
		//float zfar = renderCamera->zfar;
		float totalTime = frame->uniforms.timer->TotalTime();
		float deltaTime = frame->uniforms.timer->DeltaTime();

		//XMStoreFloat4x4(&m_PassConstants.view, frame->uniforms.view);
		//XMStoreFloat4x4(&m_PassConstants.invView, invViewMat);
		//XMStoreFloat4x4(&m_PassConstants.proj, projMat);
		//XMStoreFloat4x4(&m_PassConstants.invProj, invProjMat);
		//XMStoreFloat4x4(&m_PassConstants.viewProj, viewProj);
		//XMStoreFloat4x4(&m_PassConstants.invViewProj, invViewProj);
		m_PassConstants.view = frame->uniforms.view;
		m_PassConstants.proj = frame->uniforms.proj;
		m_PassConstants.viewProj = frame->uniforms.viewProj;
		m_PassConstants.invView = frame->uniforms.invView;
		m_PassConstants.invProj = frame->uniforms.invProj;
		m_PassConstants.invViewProj = frame->uniforms.invViewProj;
		m_PassConstants.eyePos = frame->uniforms.camPosWS;
		m_PassConstants.renderTargetSize = Vector2{ (float)frame->uniforms.viewportSize.x, (float)frame->uniforms.viewportSize.y };
		m_PassConstants.invRenderTargetSize = frame->uniforms.invViewportSize;
		m_PassConstants.znear = frame->uniforms.znear; 
		m_PassConstants.zfar = frame->uniforms.zfar;
		m_PassConstants.totalTime = totalTime;
		m_PassConstants.deltaTime = deltaTime;
		m_PassConstants.ambientLight = { 0.001, 0.001, 0.001, 1.0 };



		int index = 0;
		std::vector<CommonRenderItem*> lightRenderItems;

		StaticModelFrameResource* frameResource = (StaticModelFrameResource*)m_Rhi->GetCurrFrameResource(FrameResourceType::StaticModelFrameResource);
		
		// 这里是为了渲染Debug用的灯光模型
		for (int i = 0; i < frame->uniforms.dirLights.size(); ++i)
		{
			frame->uniforms.dirLights[i].data.direction.normalise();
			m_PassConstants.lights[index] = frame->uniforms.dirLights[i].data;
			lightRenderItems.push_back(CreateLightRenderItem(&frame->uniforms.dirLights[i].data));
			frameResource->UpdateObjectConstantBuffer(lightRenderItems.back()->objConstantIdx, &lightRenderItems.back()->objectConstants);
			index++;
		}
		for (int i = 0; i < frame->uniforms.pointLights.size(); ++i)
		{
			frame->uniforms.pointLights[i].data.direction.normalise();
			m_PassConstants.lights[index] = frame->uniforms.pointLights[i].data;
			lightRenderItems.push_back(CreateLightRenderItem(&frame->uniforms.pointLights[i].data));
			frameResource->UpdateObjectConstantBuffer(lightRenderItems.back()->objConstantIdx, &lightRenderItems.back()->objectConstants);
			index++;
		}
		for (int i = 0; i < frame->uniforms.spotLights.size(); ++i)
		{
			frame->uniforms.spotLights[i].data.direction.normalise();
			m_PassConstants.lights[index] = frame->uniforms.spotLights[i].data;
			lightRenderItems.push_back(CreateLightRenderItem(&frame->uniforms.spotLights[i].data));
			frameResource->UpdateObjectConstantBuffer(lightRenderItems.back()->objConstantIdx, &lightRenderItems.back()->objectConstants);
			index++;
		}

		m_PassConstants.shadowMapSize = Vector2(ShadowMapSize, ShadowMapSize);
		m_PassConstants.invShadowMapSize = Vector2(1.0f / ShadowMapSize, 1.0f / ShadowMapSize);
		auto csmMgr = bb->Get<CascadedShadowManager>("csm_mgr");
		auto spliters = bb->Get<std::pair<std::vector<float>, std::vector<float>>>("csm_spliters");
		auto& spliter_dis = spliters->first;
		auto lightViewProjs = bb->Get<std::vector<std::tuple<DirectX::XMMATRIX, DirectX::XMMATRIX>>>("light_view_proj_matrices");
		for (int i = 0; i < lightViewProjs->size(); ++i)
		{
			auto lightViewAndProj = (*lightViewProjs)[i];
			DirectX::XMMATRIX viewProj = DirectX::XMMatrixMultiply(std::get<0>(lightViewAndProj), std::get<1>(lightViewAndProj));
			DirectX::XMStoreFloat4x4(&m_PassConstants.lightViewProjs[i], viewProj);
			m_PassConstants.gSpliters[i] = Vector4(spliter_dis[i], 0.0f, 0.0f, 0.0f);
		}

		// Update FramePassConstants
		frameResource->UpdatePassConstantBuffer(m_PassConstantsIdx, &m_PassConstants);


		bb->Set("back_buffer", std::shared_ptr<Texture2D>(frame->backBuffer));
		bb->Set("depth_stencil_buffer", std::shared_ptr<Texture2D>(frame->depthStencilBuffer));
		//m_TestRenderTargetView = m_Rhi->CreateRenderTargetView(frame->backBuffer.get(), nullptr, m_TestRenderTargetView);
		//m_TestDepthStencilView = m_Rhi->CreateDepthStencilView(frame->depthStencilBuffer.get(), nullptr, m_TestDepthStencilView);



		TestSubPassData testSubpassData;
		//TestSubpassData.renderTargetView = m_TestRenderTargetView;
		//TestSubpassData.depthStencilView = m_TestDepthStencilView;
		//TestSubpassData.renderItems = renderResource->allRenderItems;
		testSubpassData.bb = bb;
		testSubpassData.frame = frame;
		testSubpassData.shader = frame->renderlists.opaque[0]->shader;
		testSubpassData.macros = { MacroInfo{"MaxLights", std::to_string(MaxLights)},
									MacroInfo{"NumDirLights", std::to_string(frame->uniforms.dirLights.size())},
									MacroInfo{"NumPointLights", std::to_string(frame->uniforms.pointLights.size())},
									MacroInfo{"NumSpotLights", std::to_string(frame->uniforms.spotLights.size())},
									MacroInfo{"EnableNormalMap", ""},
									MacroInfo{"EnableRoughnessMap", ""} ,
									MacroInfo{"Shadow", ""},
									MacroInfo{"MaxCascadedNum", std::to_string(MaxCascadedNum)} };
		testSubpassData.passConstantIdx = m_PassConstantsIdx;

		m_TestSubpass->PrepareForData(&testSubpassData);


		DebugDrawLightPassData debugSubpassData;
		debugSubpassData.bb = bb;
		debugSubpassData.frame = frame;

		//debugSubpassData.renderTargetView = m_TestRenderTargetView;
		//debugSubpassData.depthStencilView = m_TestDepthStencilView;
		debugSubpassData.lightRenderItems = std::move(lightRenderItems);
		debugSubpassData.passConstantIdx = m_PassConstantsIdx;
		m_DebugDrawSubpass->PrepareForData(&debugSubpassData);


		DrawSkyboxSubPassData drawSkyboxData;
		drawSkyboxData.bb = bb;
		drawSkyboxData.frame = frame;
		//drawSkyboxData.cubemap = renderResource->cubemap;
		//drawSkyboxData.renderTargetView = m_TestRenderTargetView;
		//drawSkyboxData.depthStencilView = m_TestDepthStencilView;
		drawSkyboxData.skyboxRenderItem = m_InnerCommonRenderItems["Skybox"].get();
		drawSkyboxData.passConstantIdx = m_PassConstantsIdx;
		m_SkyboxSubpass->PrepareForData(&drawSkyboxData);


		UISubPassData uiSubpassData;
		uiSubpassData.bb = bb;
		uiSubpassData.frame = frame;

		//UISubpassData.depthStencilView = m_TestDepthStencilView;
		//UISubpassData.renderTargetView = m_Rhi->GetCurrBackBufferAsRenderTarget();
		m_UISubpass->PrepareForData(&uiSubpassData);
	}

	void photon::MainCameraPass::Draw(EG_FrameContext* frame, PassBlackboard* bb)
	{
		//auto [width, height] = m_WindowSystem->GetClientWidthAndHeight();
		//D3D12_RECT scissorRect = { 0, 0, width, height };
		//D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)width, (float)height, 0.0f, 1.0f};
		
		if(!m_bOnlyUI)
		{
			m_TestSubpass->Draw(frame, bb);
			m_DebugDrawSubpass->Draw(frame, bb);
			m_SkyboxSubpass->Draw(frame, bb);
		}
		m_UISubpass->Draw(frame, bb);


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
			ritem->objConstantIdx = g_objIdxHolder.GetObjIndex();
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