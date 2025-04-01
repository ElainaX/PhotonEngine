#pragma once
#include "Function/UI/WindowUI.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx12.h"

#include <unordered_map>
#include <string>
#include <functional>

namespace photon 
{
	class EditorUI : public WindowUI
	{
	public:
		EditorUI();

		void Initialize(const WindowUIInitInfo& initInfo) override final;
		void PreRender() override final;

		void ShowEditorUI();

		void DrawEditorMenuWindowUI();
		void DrawGameEngineWindowUI();
		void DrawInspectorWindowUI();
		void DrawProjectWindowUI();
		void DrawSceneWindowUI();

	private:
		using GameObjectInspectorItem = std::function<void(GameObject*)>;

		std::unordered_map<std::string, ImGuiID> m_WindowUIDockIds;
		std::unordered_map<std::string, GameObjectInspectorItem> m_GameObjectEditors;

		std::shared_ptr<RenderSystem> m_RenderSystem;
		std::shared_ptr<WindowSystem> m_WindowSystem;
		ResourceManager* m_ResourceManager;
		GeometryGenerator* m_GeometryGenerator;

		GameObject* m_SelectedGameObject = nullptr;
	
		bool m_bEditorMenuWindowOpen = true;
		bool m_bGameEngineWindowOpen = true;
		bool m_bSceneWindowOpen = true;
		bool m_bInspectorWindowOpen = true;
		bool m_bProjectWindowOpen = true;
		bool m_bDemoWindowOpen = true;

		ImFont* m_FontBody;
		ImFont* m_FontTitle;
	};


}