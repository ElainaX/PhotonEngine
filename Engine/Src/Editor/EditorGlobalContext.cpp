#include "EditorGlobalContext.h"

namespace photon 
{
	EditorGlobalContext g_EditorGlobalContext;


	void EditorGlobalContext::Initialize(const EditorGlobalContextInitInfo& info)
	{
		windowSystem = info.windowSystem;
		renderSystem = info.renderSystem;
		engine = info.engine;
	}

	void EditorGlobalContext::clear()
	{

	}

}