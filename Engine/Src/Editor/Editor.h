#pragma once

#include "Engine.h"
#include "EditorUI.h"
#include "Function/Render/WindowSystem.h"
#include "Function/Render/RenderSystem.h"

#include <memory>

namespace photon 
{

	class PhotonEditor 
	{
	public:
		
		void Initialize(PhotonEngine* engine);

		void Run();
		void clear();

	private:
		PhotonEngine* m_Engine;
		std::shared_ptr<EditorUI> m_EditorUI;
	};


}