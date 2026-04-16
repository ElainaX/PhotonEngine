#include "EditorUI.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

namespace photon 
{


	EditorUI::EditorUI()
	{
		m_EditorTimer.Reset();

		//m_PaintIt = std::make_shared<PaintItCaller>((g_EditorFolder / L"paint_it").generic_wstring());


		//// Create Editor For GameObject Inspector
		//m_GameObjectEditors["CommonRenderItem"] = [this](GameObject* go)
		//	{
		//		auto ri = dynamic_cast<CommonRenderItem*>(go);
		//		auto mesh = m_ResourceManager->GetMesh(ri->meshGuid);
		//		auto material = ri->material;
		//		auto shader = ri->shader;
		//		auto renderLayer = ri->renderLayer;
		//		auto frameResourceEditor = m_RenderSystem->GetRenderScene()->GetCommonRItemFrameResourceEditor(go->GameObjectId);

		//		auto wndSize = ImGui::GetContentRegionAvail();

		//		auto renderLayers = GetAllRenderLayerStringList();
		//		auto layerComboGetter = [](void* data, int n) {
		//			return ((std::string*)data)[n].c_str();
		//			};

		//		static int currRenderLayer = (int)RenderLayer::Count;
		//		currRenderLayer = (int)renderLayer;
		//		if(ImGui::Combo("RenderLayer", &currRenderLayer, layerComboGetter, renderLayers.data(), renderLayers.size()))
		//		{
		//			ri->renderLayer = (RenderLayer)currRenderLayer;
		//		}
		//		ImGui::Separator();

		//		if(ImGui::CollapsingHeader("Transform"))
		//		{
		//			ImGui::Text("Translation");
		//			if (ImGui::DragFloat("x##1", &frameResourceEditor->translation.x, 0.1f)
		//				|| ImGui::DragFloat("y##1", &frameResourceEditor->translation.y, 0.1f)
		//				|| ImGui::DragFloat("z##1", &frameResourceEditor->translation.z, 0.1f))
		//			{
		//				frameResourceEditor->bDirty = true;
		//			}
		//			ImGui::Separator();

		//			ImGui::Text("Rotation");
		//			if (ImGui::DragFloat("x##2", &frameResourceEditor->rotationXYZ.x, 0.1f)
		//				|| ImGui::DragFloat("y##2", &frameResourceEditor->rotationXYZ.y, 0.1f)
		//				|| ImGui::DragFloat("z##2", &frameResourceEditor->rotationXYZ.z, 0.1f))
		//			{
		//				frameResourceEditor->bDirty = true;
		//			}
		//			ImGui::Separator();

		//			ImGui::Text("Scale");
		//			if (ImGui::DragFloat("x##3", &frameResourceEditor->scale.x, 0.1f)
		//				|| ImGui::DragFloat("y##3", &frameResourceEditor->scale.x, 0.1f)
		//				|| ImGui::DragFloat("z##3", &frameResourceEditor->scale.z, 0.1f))
		//			{
		//				frameResourceEditor->scale.y = frameResourceEditor->scale.z = frameResourceEditor->scale.x;
		//				frameResourceEditor->bDirty = true;
		//			}
		//		}

		//		if (ImGui::CollapsingHeader("Render"))
		//		{
		//			ImGui::Text(std::format("Mesh(#{})", mesh->guid).c_str());
		//			ImGui::Text("Name");
		//			ImGui::SameLine();
		//			ImGui::Text(WString2String(mesh->name).c_str());
		//			ImGui::Separator();

		//			ImGui::Text("Shader");
		//			ImGui::Text("Path");
		//			ImGui::SameLine();
		//			ImGui::TextWrapped("%s", WString2String(shader->sourceFilepath).c_str());
		//			ImGui::Separator();


		//			ImGui::Text(std::format("Material(#{})", material->guid).c_str());
		//			static ShaderResourceView* texImageView = nullptr;
		//			texImageView = m_RenderSystem->GetRHI()->CreateShaderResourceView(material->diffuseMap, nullptr, texImageView);
		//			if(ImGui::TreeNode("Texture"))
		//			{
		//				ImGui::TextWrapped("%s", WString2String(material->diffuseMap->name).c_str());
		//				ImGui::Image(texImageView->gpuHandleInHeap.ptr, ImVec2(wndSize.x - 30, wndSize.x - 30));
		//				ImGui::TreePop();
		//			}
		//			ImGui::Separator();

		//			if(ImGui::ColorEdit4("DiffuseAlbedo", (float*)&material->matCBufferData.diffuseAlbedo))
		//			{
		//				ri->SetDirty();
		//			}
		//			if(ImGui::SliderFloat3("FresnelR0", (float*)&material->matCBufferData.fresnelR0, 0.0f, 1.0f))
		//			{
		//				ri->SetDirty();
		//			}
		//			if(ImGui::SliderFloat("Roughness", (float*)&material->matCBufferData.roughness, 0.0f, 1.0f))
		//			{
		//				ri->SetDirty();
		//			}
		//			
		//		}
		//	};
		//	m_GameObjectEditors["ModelAsset"] = [this](GameObject* go)
		//		{
		//			auto model = dynamic_cast<ModelAsset*>(go);
		//			auto frameResourceEditor = m_RenderSystem->GetRenderScene()->GetCommonRItemFrameResourceEditor(go->GameObjectId);
		//			auto resourceManager = m_RenderSystem->GetResourceManager();
		//			if (ImGui::CollapsingHeader("Transform"))
		//			{
		//				ImGui::Text("Translation");
		//				if (ImGui::DragFloat("x##1", &frameResourceEditor->translation.x, 0.1f)
		//					|| ImGui::DragFloat("y##1", &frameResourceEditor->translation.y, 0.1f)
		//					|| ImGui::DragFloat("z##1", &frameResourceEditor->translation.z, 0.1f))
		//				{
		//					frameResourceEditor->bDirty = true;
		//				}
		//				ImGui::Separator();

		//				ImGui::Text("Rotation");
		//				if (ImGui::DragFloat("x##2", &frameResourceEditor->rotationXYZ.x, 0.1f)
		//					|| ImGui::DragFloat("y##2", &frameResourceEditor->rotationXYZ.y, 0.1f)
		//					|| ImGui::DragFloat("z##2", &frameResourceEditor->rotationXYZ.z, 0.1f))
		//				{
		//					frameResourceEditor->bDirty = true;
		//				}
		//				ImGui::Separator();

		//				ImGui::Text("Scale");
		//				if (ImGui::DragFloat("x##3", &frameResourceEditor->scale.x, 0.01f)
		//					|| ImGui::DragFloat("y##3", &frameResourceEditor->scale.x, 0.01f)
		//					|| ImGui::DragFloat("z##3", &frameResourceEditor->scale.z, 0.01f))
		//				{
		//					frameResourceEditor->scale.y = frameResourceEditor->scale.z = frameResourceEditor->scale.x;
		//					frameResourceEditor->bDirty = true;
		//				}
		//			}

		//			static bool bShouldOpenModal = false;
		//			if(ImGui::CollapsingHeader("Paint It"))
		//			{
		//				if(ImGui::Button("Prompt Generate"))
		//				{
		//					ImGui::OpenPopup("PaintIt");
		//					bShouldOpenModal = true;
		//					m_RenderSystem->Stop();
		//				}
		//				ImGui::SameLine();
		//				if(ImGui::Button("Back to Raw"))
		//				{
		//					model->GlobalRebackRawDiffuseMap();
		//					model->GlobalRebackRawNormalMap();
		//					model->GlobalRebackRawSpecularMap();
		//				}
		//			}

		//			DrawPaintItModal(model, &bShouldOpenModal);


		//		};
		//m_GameObjectEditors["DirLight"] = [this](GameObject* go)
		//	{
		//		auto dirlight = dynamic_cast<DirLight*>(go);
		//		ImGui::ColorEdit4("Light Color", (float*)&dirlight->data.strength);
		//		ImGui::SliderFloat3("Light Position", (float*)&dirlight->data.position, -5.0f, 5.0f);
		//		dirlight->data.direction = -dirlight->data.position.normalisedCopy();
		//		ImGui::SliderFloat3("Light Direction", (float*)&dirlight->data.direction, -1.0f, 1.0f);
		//	};

		//m_GameObjectEditors["PointLight"] = [this](GameObject* go)
		//	{
		//		auto pointlight = dynamic_cast<PointLight*>(go);
		//		ImGui::ColorEdit4("Light Color", (float*)&pointlight->data.strength);
		//		ImGui::DragFloat("falloffStart", &pointlight->data.falloffStart);
		//		ImGui::DragFloat("falloffEnd", &pointlight->data.falloffEnd);
		//		if (pointlight->data.falloffEnd < pointlight->data.falloffStart)
		//		{
		//			pointlight->data.falloffEnd = pointlight->data.falloffStart;
		//		}
		//		ImGui::DragFloat3("Light Position", (float*)&pointlight->data.position);
		//	};

		//m_GameObjectEditors["SpotLight"] = [this](GameObject* go)
		//	{
		//		auto spotlight = dynamic_cast<SpotLight*>(go);
		//		ImGui::ColorEdit4("Light Color", (float*)&spotlight->data.strength);
		//		ImGui::DragFloat("falloffStart", &spotlight->data.falloffStart, 1.0f, 0.0f);
		//		ImGui::DragFloat("falloffEnd", &spotlight->data.falloffEnd, 1.0f, spotlight->data.falloffStart);
		//		if (spotlight->data.falloffEnd < spotlight->data.falloffStart)
		//		{
		//			spotlight->data.falloffEnd = spotlight->data.falloffStart;
		//		}
		//		ImGui::DragFloat3("Light Position", (float*)&spotlight->data.position);
		//		ImGui::SliderFloat3("Spot Direction", (float*)&spotlight->data.direction, -1.0f, 1.0f);
		//		ImGui::DragFloat("Spot Power", (float*)&spotlight->data.spotPower, 1.0f, 1.0f, 128.0f);
		//	};
	}

	void EditorUI::Initialize(const WindowUIInitInfo& initInfo)
	{
		m_WindowSystem = initInfo.windowSystem;
		m_RenderSystem = initInfo.renderSystem;
		m_ResourceManager = m_RenderSystem->GetResourceManager();
		//m_GeometryGenerator = m_RenderSystem->GetGeometryGenerator();

		m_RenderSystem->InitializeEditorUI(this);

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		// io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		m_FontTitle = io.Fonts->AddFontFromFileTTF(
			"E:\\Code\\PhotonEngine\\Engine\\Assets\\Fonts\\consolab.ttf", 16.0f);
		m_FontBody = io.Fonts->AddFontFromFileTTF(
			"E:\\Code\\PhotonEngine\\Engine\\Assets\\Fonts\\consola.ttf", 16.0f);

		ImGui::StyleColorsDark();
	}

	void EditorUI::PreRender()
	{
		m_EditorTimer.Tick();
		ShowEditorUI();
	}

	void EditorUI::ShowEditorUI()
	{
		//static bool open = true;
		//ImGui::ShowDemoWindow(&open);
		DrawEditorMenuWindowUI();
		DrawGameEngineWindowUI();
		DrawInspectorWindowUI();
		DrawProjectWindowUI();
		DrawSceneWindowUI();
	}

	void EditorUI::DrawEditorMenuWindowUI()
	{
		//ImGui::BeginMainMenuBar();

		//if(ImGui::BeginMenu("Options"))
		//{

		//	ImGui::EndMenu();
		//}

		//ImGui::EndMainMenuBar();

		//ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

		ImGui::PushFont(m_FontBody);

		static ImGuiDockNodeFlags s_dockSpaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		windowFlags |= ImGuiWindowFlags_NoBackground;

		ImGui::Begin("EditorMenuWindow", &m_bEditorMenuWindowOpen, windowFlags);
		ImGui::PopStyleVar(3);
		ImGuiIO& io = ImGui::GetIO();
		ImGuiID dockspace_id = ImGui::GetID("Main Dockspace");

		if (ImGui::DockBuilderGetNode(dockspace_id) == nullptr)
		{
			ImGui::DockBuilderRemoveNode(dockspace_id); // 清除旧的布局
			ImGui::DockBuilderAddNode(dockspace_id, ImGuiDockNodeFlags_DockSpace);
			ImGui::DockBuilderSetNodeSize(dockspace_id, ImVec2(io.DisplaySize.x, io.DisplaySize.y));
			
			ImGuiID inspectorId, inspectorOtherId;
			ImGui::DockBuilderSplitNode(dockspace_id, ImGuiDir_Right, 0.25f, &inspectorId, &inspectorOtherId);
			m_WindowUIDockIds["Inspector"] = inspectorId;

			ImGuiID projectId, projectOtherId;
			ImGui::DockBuilderSplitNode(inspectorOtherId, ImGuiDir_Down, 0.25f, &projectId, &projectOtherId);
			m_WindowUIDockIds["Project"] = projectId;
				
			ImGuiID sceneId, engineId;
			ImGui::DockBuilderSplitNode(projectOtherId, ImGuiDir_Left, 0.25f, &sceneId, &engineId);
			m_WindowUIDockIds["Scene"] = sceneId;

			m_WindowUIDockIds["GameEngine"] = engineId;

			ImGui::DockBuilderDockWindow("Inspector", inspectorId);
			ImGui::DockBuilderDockWindow("Project", projectId);
			ImGui::DockBuilderDockWindow("GameEngine", engineId);
			ImGui::DockBuilderDockWindow("Scene", sceneId);

			ImGui::DockBuilderFinish(dockspace_id);
		}

		ImGui::DockSpace(dockspace_id);

		if (ImGui::BeginMenuBar())
		{

			if (ImGui::BeginMenu("Options"))
			{
				if(ImGui::MenuItem("haha"))
				{
					
				}

				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}



		ImGui::End();
		//if (ImGui::BeginMenuBar())
		//{
		//	if (ImGui::BeginMenu("Options"))
		//	{

		//	}

		//	ImGui::EndMenu();
		//}
		
		ImGui::PopFont();

	}

	void EditorUI::DrawGameEngineWindowUI()
	{
		auto windowFlags = ImGuiWindowFlags_None;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("GameEngine", &m_bGameEngineWindowOpen, windowFlags);

		ImGui::PopStyleVar(3);

		// 这一块区域是你真正想显示场景画面的 client 区域
		ImVec2 avail = ImGui::GetContentRegionAvail();
		Vector2i viewportSize = {
			std::max(1, static_cast<int>(avail.x)),
			std::max(1, static_cast<int>(avail.y))
		};

		// 让 WindowSystem 知道当前 editor 视口大小
		if (viewportSize != m_LastGameEngineViewportSize)
		{
			m_LastGameEngineViewportSize = viewportSize;
			m_WindowSystem->SetViewportSize(viewportSize);
		}

		// 鼠标悬停 / 聚焦时，把输入焦点交给 render viewport
		const bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_RootAndChildWindows);
		const bool focused = ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows);
		m_WindowSystem->SetFocusOnRenderWindow(hovered || focused);

		// 尝试显示场景输出
		TextureHandle sceneColor = m_RenderSystem->GetSceneColorHandle();
		if (sceneColor.handle.IsValid())
		{
			auto* rm = m_RenderSystem->GetResourceManager();
			auto* imguiSystem = m_RenderSystem->GetImGuiSystem();

			const TextureRenderResource* sceneColorRR =
				rm ? rm->GetTextureRenderResource(sceneColor) : nullptr;

			if (imguiSystem && sceneColorRR && sceneColorRR->srv.IsValid())
			{
				// sceneColor 改变了，就重新注册一遍 ImGui texture id
				if (!m_GameEngineTexId || !(sceneColor == m_GameEngineBoundSceneColor))
				{
					if (m_GameEngineTexId)
					{
						imguiSystem->UnregisterExternalSrv(m_GameEngineTexId);
						m_GameEngineTexId = 0;
					}

					m_GameEngineTexId =
						imguiSystem->RegisterExternalSrv(
							m_RenderSystem->GetDescriptorSystem(),
							sceneColorRR->srv);

					m_GameEngineBoundSceneColor = sceneColor;
				}

				if (m_GameEngineTexId)
				{
					ImGui::Image(m_GameEngineTexId, avail);
				}
				else
				{
					ImGui::SetCursorPos(ImVec2(20, 20));
					ImGui::TextUnformatted("Game viewport texture registration failed.");
				}
			}
			else
			{
				ImGui::SetCursorPos(ImVec2(20, 20));
				ImGui::TextUnformatted("SceneColor SRV is not ready.");
			}
		}
		else
		{
			ImGui::SetCursorPos(ImVec2(20, 20));
			ImGui::TextUnformatted("SceneColor is invalid.");
		}

		ImGui::End();
	}

	void EditorUI::DrawInspectorWindowUI()
	{
		//ImGui::Begin("Inspector", &m_bGameEngineWindowOpen);

		//if(m_SelectedGameObject)
		//{
		//	if(m_GameObjectEditors.find(m_SelectedGameObject->GetGameObjectType()) != m_GameObjectEditors.end())
		//	{
		//		m_GameObjectEditors[m_SelectedGameObject->GetGameObjectType()](m_SelectedGameObject);
		//	}
		//	else 
		//	{
		//		assert(0);
		//	}
		//}

		//ImVec2 currSize = ImGui::GetContentRegionAvail();
		//{
		//	ShaderResourceView* srv = m_RenderSystem->GetPipelineCsmMgrSRV();
		//	ImGui::Image((ImTextureID)(srv->gpuHandleInHeap.ptr), currSize);
		//}

		//ImGui::End();
	}
	bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size = size_arg;
		size.x -= style.FramePadding.x * 2;

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ImGui::ItemSize(bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		// Render
		const float circleStart = size.x * 0.7f;
		const float circleEnd = size.x;
		const float circleWidth = circleEnd - circleStart;

		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
		window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);

		const float t = g.Time;
		const float r = size.y / 2;
		const float speed = 1.5f;

		const float a = speed * 0;
		const float b = speed * 0.333f;
		const float c = speed * 0.666f;

		const float o1 = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
		const float o2 = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
		const float o3 = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
		window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);

		return true;
	}

	bool Spinner(const char* label, float radius, int thickness, const ImU32& color) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window->SkipItems)
			return false;

		ImGuiContext& g = *GImGui;
		const ImGuiStyle& style = g.Style;
		const ImGuiID id = window->GetID(label);

		ImVec2 pos = window->DC.CursorPos;
		ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

		const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
		ImGui::ItemSize(bb, style.FramePadding.y);
		if (!ImGui::ItemAdd(bb, id))
			return false;

		// Render
		window->DrawList->PathClear();

		int num_segments = 30;
		int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

		const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
		const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

		const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

		for (int i = 0; i < num_segments; i++) {
			const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
			window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
				centre.y + ImSin(a + g.Time * 8) * radius));
		}

		window->DrawList->PathStroke(color, false, thickness);

		return true;
	}

	void EditorUI::DrawProjectWindowUI()
	{
		ImGui::Begin("Project", &m_bGameEngineWindowOpen);

		{
			
			const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
			const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);

			Spinner("##spinner", 15, 6, col);
			BufferingBar("##buffer_bar", 0.7f, ImVec2(400, 6), bg, col);
		}

		ImGui::End();
	}

	void EditorUI::DrawSceneWindowUI()
	{
		ImGui::Begin("Scene", &m_bGameEngineWindowOpen);

		//{
		//	auto renderScene = m_RenderSystem->GetRenderScene();
		//	auto ritems = renderScene->GetCommonRenderItems(nullptr);
		//	auto& dirlights = renderScene->directionalLights;
		//	auto& pointLights = renderScene->pointLights;
		//	auto& spotLights = renderScene->spotLights;
		//	auto& models = renderScene->GetModelRenderItems();

		//	for(auto& ri : ritems)
		//	{
		//		ImGui::PushID(ri->GameObjectId);
		//		if (ImGui::Selectable(ri->GetGameObjectType().c_str(), m_SelectedGameObject == ri))
		//		{
		//			m_SelectedGameObject = ri;
		//		}
		//		ImGui::PopID();
		//	}

		//	for(auto& keyval : models)
		//	{
		//		auto model = keyval.first;
		//		auto ritems = keyval.second;
		//		ImGui::PushID(model->GameObjectId);

		//		ImGuiTreeNodeFlags nodeFlag = ImGuiTreeNodeFlags_OpenOnArrow;

		//		if(m_SelectedGameObject == model.get())
		//		{
		//			nodeFlag |= ImGuiTreeNodeFlags_Selected;
		//		}


		//		if(ImGui::TreeNodeEx(model->GetGameObjectType().c_str(), nodeFlag))
		//		{
		//			if (ImGui::IsItemClicked())
		//			{
		//				m_SelectedGameObject = model.get();
		//			}
		//			for(auto& ri : ritems)
		//			{
		//				ImGui::PushID(ri->GameObjectId);
		//				if (ImGui::Selectable(ri->GetGameObjectType().c_str(), m_SelectedGameObject == ri.get()))
		//				{
		//					m_SelectedGameObject = ri.get();
		//				}
		//				ImGui::PopID();
		//			}

		//			ImGui::TreePop();
		//		}


		//		ImGui::PopID();
		//	}

		//	for (auto& light : dirlights)
		//	{
		//		ImGui::PushID(light.GameObjectId);
		//		if (ImGui::Selectable(light.GetGameObjectType().c_str(), m_SelectedGameObject == &light))
		//		{
		//			if (m_SelectedGameObject != &light)
		//			{
		//				m_SelectedGameObject = &light;
		//			}
		//		}
		//		ImGui::PopID();
		//	}
		//	for (auto& light : pointLights)
		//	{
		//		ImGui::PushID(light.GameObjectId);
		//		if (ImGui::Selectable(light.GetGameObjectType().c_str(), m_SelectedGameObject == &light))
		//		{
		//			if (m_SelectedGameObject != &light)
		//			{
		//				m_SelectedGameObject = &light;
		//			}
		//		}
		//		ImGui::PopID();
		//	}

		//	for (auto& light : spotLights)
		//	{
		//		ImGui::PushID(light.GameObjectId);
		//		if (ImGui::Selectable(light.GetGameObjectType().c_str(), m_SelectedGameObject == &light))
		//		{
		//			if (m_SelectedGameObject != &light)
		//			{
		//				m_SelectedGameObject = &light;
		//			}
		//		}
		//		ImGui::PopID();
		//	}



		//}

		ImGui::End();
	}

	void EditorUI::DrawPaintItModal(ModelAsset* model, bool* bOpen)
	{
		/*if (!(*bOpen))
			return;
		static bool bInGeneratingMode = false;
		static bool bSubstiDiffuseMap = true;
		static bool bSubstiSpecularMap = false;
		static bool bSubstiNormalMap = false;
		static char posPromptString[256] = "";
		static char negPromptString[256] = "";
		static float timeElapsed = 0.0f;
		static int progress = 0.0f;
		static float timeSwitched = 0.0f;


		RenderSystem* renderSystem = g_RuntimeGlobalContext.renderSystem.get();
		ResourceManager* resourceManager = renderSystem->GetResourceManager();
		

		ImGui::SetNextWindowSize(ImVec2(700, 300));
		
		ImGui::BeginPopupModal("PaintIt");

		if(!bInGeneratingMode)
		{
			ImGui::InputText("Positive Prompt", posPromptString, sizeof(posPromptString));
			ImGui::InputText("Negative Prompt (can omit)", negPromptString, sizeof(negPromptString));

			ImGui::Checkbox("diffuse", &bSubstiDiffuseMap);
			ImGui::SameLine();
			ImGui::Checkbox("normal", &bSubstiNormalMap);
			ImGui::SameLine();
			ImGui::Checkbox("specular", &bSubstiSpecularMap);
			if(ImGui::Button("Begin Generate"))
			{
				auto modelFolder = Path::GetFileFolder(model->name);
				auto folderName = Path::GetPathSegments(modelFolder).back();
				std::wstring posPromptWstr = String2WString(posPromptString);
				std::wstring negPromptWstr = String2WString(negPromptString);

				auto finalPosPrompt = std::format(L"a mesh of {}, {}", folderName, posPromptWstr);
				std::wstring modelPath = model->name;
				std::wstring outputPath = std::format(L"{}/{}_{}", modelFolder.generic_wstring(), posPromptWstr, negPromptWstr);
				m_PaintIt->FillKeyArgs(modelPath, finalPosPrompt, outputPath, negPromptWstr);
				m_PaintIt->ExecuteCommand();
				bInGeneratingMode = true;
			}
			ImGui::SameLine();
			if(ImGui::Button("Cancel"))
			{
				bSubstiDiffuseMap = true;
				bSubstiNormalMap = false;
				bSubstiSpecularMap = false;
				bInGeneratingMode = false;
				*bOpen = false;

				ImGui::CloseCurrentPopup();
				renderSystem->ReStart();
			}
		}
		else 
		{
			timeElapsed += m_EditorTimer.DeltaTime();
			timeSwitched += m_EditorTimer.DeltaTime();
			if(!m_PaintIt->isOver())
			{
				if(timeElapsed >= 1.0f)
				{
					progress = m_PaintIt->GetProgress();
					timeElapsed -= 1.0f;
				}

				if(timeSwitched > 0.1f)
				{
					timeSwitched -= 0.1f;
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
				
				const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
				const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);

				Spinner("##spinner", 10, 6, col);
				BufferingBar("##buffer_bar", progress / 100.0f, ImVec2(400, 15), bg, col);
				ImGui::SameLine();
				ImGui::Text(std::format("{}%%", progress).c_str());
			}
			else
			{
				ChangeModelToPaintIt(model, bSubstiDiffuseMap, bSubstiNormalMap, bSubstiSpecularMap);
				
				timeElapsed = 0.0f;
				bSubstiDiffuseMap = true;
				bSubstiNormalMap = false;
				bSubstiSpecularMap = false;
				bInGeneratingMode = false;
				*bOpen = false;

				ImGui::CloseCurrentPopup();
				renderSystem->ReStart();
			}
		}

		ImGui::EndPopup();
*/


		/*static std::string path = "";
		if (ImGui::Button("OK"))
		{
			path = buffer;
			auto tex = resourceManager->LoadTexture2D(String2WString(path));
			if (tex)
			{
				model->GlobalSwitchDiffuseMap(tex);
			}
			else
			{
				ImGui::OpenPopup("Info");

			}
		}
		if (ImGui::BeginPopupModal("Info"))
		{
			ImGui::Text(("Can't Open File: " + path).c_str());
			if (ImGui::Button("Close"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}*/
	}

	void EditorUI::ChangeModelToPaintIt(ModelAsset* model, bool bDiffuse, bool bNormal, bool bSpecular)
	{
		//ResourceManager* manager = g_RuntimeGlobalContext.renderSystem->GetResourceManager();

		//auto diffuseMap = m_PaintIt->GetOutputDiffuseMapPath();
		//auto normalMap = m_PaintIt->GetOutputNormalMapPath();
		//auto specularMap = m_PaintIt->GetOutputSpecularMapPath();
		//
		//if (bDiffuse)
		//{
		//	auto texDiffuse = manager->LoadTexture2D(diffuseMap);
		//	model->GlobalSwitchDiffuseMap(texDiffuse);
		//}
		//if(bNormal)
		//{
		//	auto texNormal = manager->LoadTexture2D(normalMap);
		//	model->GlobalSwitchNormalMap(texNormal);
		//}
		//if (bSpecular)
		//{
		//	auto texSpecular = manager->LoadTexture2D(specularMap);
		//	model->GlobalSwitchSpecularMap(texSpecular);
		//}

	}

}