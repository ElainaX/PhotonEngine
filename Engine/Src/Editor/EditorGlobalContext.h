#pragma once

#include "Engine.h"
#include "Function/Render/RenderSystem.h"
#include "Function/Render/WindowSystem.h"

namespace photon 
{
	struct EditorGlobalContextInitInfo
	{
		WindowSystem* windowSystem = nullptr;
		RenderSystem* renderSystem = nullptr;
		PhotonEngine* engine	   = nullptr;
	};

	class EditorGlobalContext
	{
	public:

		void Initialize(const EditorGlobalContextInitInfo& info);
		void clear();

		WindowSystem* windowSystem;
		RenderSystem* renderSystem;
		PhotonEngine* engine;
	};

	extern EditorGlobalContext g_EditorGlobalContext;
}