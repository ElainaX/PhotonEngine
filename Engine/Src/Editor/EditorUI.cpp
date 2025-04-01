#include "EditorUI.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imgui_impl_dx12.h"
#include "ImGui/imgui_impl_win32.h"

#include <format>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace photon 
{


	EditorUI::EditorUI()
	{
		// Create Editor For GameObject Inspector
		m_GameObjectEditors["CommonRenderItem"] = [this](GameObject* go)
			{
				auto ri = dynamic_cast<CommonRenderItem*>(go);
				auto mesh = m_ResourceManager->GetMesh(ri->meshGuid);
				auto material = ri->material;
				auto shader = ri->shader;
				auto renderLayer = ri->renderLayer;
				auto frameResourceEditor = m_RenderSystem->GetRenderScene()->GetCommonRItemFrameResourceEditor(go->GameObjectId);

				auto wndSize = ImGui::GetContentRegionAvail();

				auto renderLayers = GetAllRenderLayerStringList();
				auto layerComboGetter = [](void* data, int n) {
					return ((std::string*)data)[n].c_str();
					};

				static int currRenderLayer = (int)RenderLayer::Count;
				currRenderLayer = (int)renderLayer;
				if(ImGui::Combo("RenderLayer", &currRenderLayer, layerComboGetter, renderLayers.data(), renderLayers.size()))
				{
					ri->renderLayer = (RenderLayer)currRenderLayer;
				}
				ImGui::Separator();

				if(ImGui::CollapsingHeader("Transform"))
				{
					ImGui::Text("Translation");
					if (ImGui::DragFloat("x##1", &frameResourceEditor->translation.x, 0.1f)
						|| ImGui::DragFloat("y##1", &frameResourceEditor->translation.y, 0.1f)
						|| ImGui::DragFloat("z##1", &frameResourceEditor->translation.z, 0.1f))
					{
						ri->SetDirty();
						frameResourceEditor->bDirty = true;
					}
					ImGui::Separator();

					ImGui::Text("Rotation");
					if (ImGui::DragFloat("x##2", &frameResourceEditor->rotationXYZ.x, 0.1f)
						|| ImGui::DragFloat("y##2", &frameResourceEditor->rotationXYZ.y, 0.1f)
						|| ImGui::DragFloat("z##2", &frameResourceEditor->rotationXYZ.z, 0.1f))
					{
						ri->SetDirty();
						frameResourceEditor->bDirty = true;
					}
					ImGui::Separator();

					ImGui::Text("Scale");
					if (ImGui::DragFloat("x##3", &frameResourceEditor->scale.x, 0.1f)
						|| ImGui::DragFloat("y##3", &frameResourceEditor->scale.x, 0.1f)
						|| ImGui::DragFloat("z##3", &frameResourceEditor->scale.z, 0.1f))
					{
						ri->SetDirty();
						frameResourceEditor->scale.y = frameResourceEditor->scale.z = frameResourceEditor->scale.x;
						frameResourceEditor->bDirty = true;
					}
				}

				if (ImGui::CollapsingHeader("Render"))
				{
					ImGui::Text(std::format("Mesh(#{})", mesh->guid).c_str());
					ImGui::Text("Name");
					ImGui::SameLine();
					ImGui::Text(WString2String(mesh->name).c_str());
					ImGui::Separator();

					ImGui::Text("Shader");
					ImGui::Text("Path");
					ImGui::SameLine();
					ImGui::TextWrapped("%s", WString2String(shader->sourceFilepath).c_str());
					ImGui::Separator();


					ImGui::Text(std::format("Material(#{})", material->guid).c_str());
					static ShaderResourceView* texImageView = nullptr;
					texImageView = m_RenderSystem->GetRHI()->CreateShaderResourceView(material->diffuseMap, nullptr, texImageView);
					if(ImGui::TreeNode("Texture"))
					{
						ImGui::TextWrapped("%s", WString2String(material->diffuseMap->name).c_str());
						ImGui::Image(texImageView->gpuHandleInHeap.ptr, ImVec2(wndSize.x - 30, wndSize.x - 30));
						ImGui::TreePop();
					}
					ImGui::Separator();

					if(ImGui::ColorEdit4("DiffuseAlbedo", (float*)&material->matCBufferData.diffuseAlbedo))
					{
						ri->SetDirty();
					}
					if(ImGui::SliderFloat3("FresnelR0", (float*)&material->matCBufferData.fresnelR0, 0.0f, 1.0f))
					{
						ri->SetDirty();
					}
					if(ImGui::SliderFloat("Roughness", (float*)&material->matCBufferData.roughness, 0.0f, 1.0f))
					{
						ri->SetDirty();
					}
					
				}
			};
		m_GameObjectEditors["DirLight"] = [this](GameObject* go)
			{
				auto dirlight = dynamic_cast<DirLight*>(go);
				ImGui::ColorEdit4("Light Color", (float*)&dirlight->data.strength);
				ImGui::SliderFloat3("Light Direction", (float*)&dirlight->data.direction, -1.0f, 1.0f);
			};
	}

	void EditorUI::Initialize(const WindowUIInitInfo& initInfo)
	{
		m_WindowSystem = initInfo.windowSystem;
		m_RenderSystem = initInfo.renderSystem;
		m_ResourceManager = m_RenderSystem->GetResourceManager();
		m_GeometryGenerator = m_RenderSystem->GetGeometryGenerator();

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;

		m_FontTitle = io.Fonts->AddFontFromFileTTF("E:\\Code\\PhotonEngine\\Engine\\Assets\\Fonts\\consolab.ttf", 16.0f);
		m_FontBody = io.Fonts->AddFontFromFileTTF("E:\\Code\\PhotonEngine\\Engine\\Assets\\Fonts\\consola.ttf", 16.0f);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsClassic();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		//ImGuiStyle& style = ImGui::GetStyle();
		//if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		//{
		//	style.WindowRounding = 0.0f;
		//	style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		//}

		ImGui_ImplWin32_Init(m_WindowSystem->GetHwnd());
		m_WindowSystem->RegisterBeforeAllEventCallBack([](Win32WndProcInfo& procInfo, bool& bContinue) {
			if (ImGui_ImplWin32_WndProcHandler(procInfo.hwnd, procInfo.msg, procInfo.wparam, procInfo.lparam))
			{
				bContinue = false;
			}
		});

		m_RenderSystem->InitializeEditorUI(this);
	}

	void EditorUI::PreRender()
	{
		ImGui_ImplDX12_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		ShowEditorUI();


		ImGui::Render();
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
		static Vector2i clientSize = {0, 0};

		auto windowFlags = ImGuiWindowFlags_None;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("GameEngine", &m_bGameEngineWindowOpen, windowFlags);

		ImGui::PopStyleVar(3);

		ImVec2 currSize = ImGui::GetContentRegionAvail();
		if(clientSize.x != currSize.x || clientSize .y != currSize.y)
		{
			clientSize.x = currSize.x;
			clientSize.y = currSize.y;
			m_WindowSystem->SetViewportSize(clientSize);
			m_RenderSystem->ReCreateRenderTargetTexAndDepthStencilTex(clientSize);
		}

		{
			ShaderResourceView* srv = m_RenderSystem->GetFinalOutputShaderResourceView();
			ImGui::Image((ImTextureID)(srv->gpuHandleInHeap.ptr), currSize);
		}

		ImGui::End();
	}

	void EditorUI::DrawInspectorWindowUI()
	{
		ImGui::Begin("Inspector", &m_bGameEngineWindowOpen);

		if(m_SelectedGameObject)
		{

			if(m_SelectedGameObject->GetGameObjectType() == "CommonRenderItem")
			{
				m_GameObjectEditors["CommonRenderItem"](m_SelectedGameObject);
			}
			else if(m_SelectedGameObject->GetGameObjectType() == "DirLight")
			{
				m_GameObjectEditors["DirLight"](m_SelectedGameObject);
			}
			else 
			{
				assert(0);
			}
		}

		ImGui::End();
	}

	void EditorUI::DrawProjectWindowUI()
	{
		ImGui::Begin("Project", &m_bGameEngineWindowOpen);

		{


		}

		ImGui::End();
	}

	void EditorUI::DrawSceneWindowUI()
	{
		ImGui::Begin("Scene", &m_bGameEngineWindowOpen);

		{
			auto renderScene = m_RenderSystem->GetRenderScene();
			auto ritems = renderScene->GetCommonRenderItems(nullptr);
			auto& dirlights = renderScene->directionalLights;

			for(auto& ri : ritems)
			{
				ImGui::PushID(ri->GameObjectId);
				if(ImGui::Selectable(ri->GetGameObjectType().c_str(), m_SelectedGameObject == ri))
				{
					if (m_SelectedGameObject != ri)
					{
						m_SelectedGameObject = ri;
					}
				}
				ImGui::PopID();
			}

			for (auto& light : dirlights)
			{
				ImGui::PushID(light.GameObjectId);
				if (ImGui::Selectable(light.GetGameObjectType().c_str(), m_SelectedGameObject == &light))
				{
					if (m_SelectedGameObject != &light)
					{
						m_SelectedGameObject = &light;
					}
				}
				ImGui::PopID();
			}


		}

		ImGui::End();
	}

}