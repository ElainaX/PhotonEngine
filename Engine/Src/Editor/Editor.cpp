#include "Editor.h"
#include "EditorGlobalContext.h"
#include "Function/Global/RuntimeGlobalContext.h"

namespace photon 
{



	void PhotonEditor::Initialize(PhotonEngine* engine)
	{
		m_Engine = engine;

		EditorGlobalContextInitInfo ctxInitInfo = {};
		ctxInitInfo.engine = engine;
		ctxInitInfo.windowSystem = g_RuntimeGlobalContext.windowSystem.get();
		ctxInitInfo.renderSystem = g_RuntimeGlobalContext.renderSystem.get();
		g_EditorGlobalContext.Initialize(ctxInitInfo);

		WindowUIInitInfo uiInitInfo = {};
		uiInitInfo.windowSystem = g_RuntimeGlobalContext.windowSystem;
		uiInitInfo.renderSystem = g_RuntimeGlobalContext.renderSystem;
		m_EditorUI = std::make_shared<EditorUI>();
		m_EditorUI->Initialize(uiInitInfo);

	}

	void PhotonEditor::Run()
	{
		//// Run Code
		auto timer = m_Engine->GetTimer();
		timer->Reset();
		assert(g_RuntimeGlobalContext.windowSystem);
		while (!g_RuntimeGlobalContext.windowSystem->ShouldClose())
		{
			timer->Tick();
			m_Engine->TickOneFrame(*timer);
		}
	}

	void PhotonEditor::clear()
	{
		m_EditorUI.reset();
	}

}