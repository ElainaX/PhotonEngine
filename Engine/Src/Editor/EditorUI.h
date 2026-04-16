#pragma once
#include "Function/UI/WindowUI.h"
#include "ImGui/imgui.h"
#include "Function/Util/GameTimer.h"

#include <unordered_map>
#include <string>
#include <functional>

#include "Function/Render/GeometryGenerator.h"

namespace photon 
{
	class EditorUI : public WindowUI
	{
	public:
		EditorUI();
		~EditorUI() = default;

		void Initialize(const WindowUIInitInfo& initInfo) override final;
		void PreRender() override final;

		void ShowEditorUI();

		void DrawEditorMenuWindowUI();
		void DrawGameEngineWindowUI();
		void DrawInspectorWindowUI();
		void DrawProjectWindowUI();
		void DrawSceneWindowUI();


		void DrawPaintItModal(ModelAsset* model, bool* bOpen);
		void ChangeModelToPaintIt(ModelAsset* model, bool bDiffuse, bool bNormal, bool bSpecular);

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

		GameTimer m_EditorTimer;

	private:
		ImTextureID m_GameEngineTexId = 0;
		TextureHandle m_GameEngineBoundSceneColor = {};
		Vector2i m_LastGameEngineViewportSize = {};

		//std::shared_ptr<PaintItCaller> m_PaintIt;
	};


}
