#pragma once
#include "Function/Render/RenderSystem.h"
#include "Function/Render/WindowSystem.h"


namespace photon 
{
	struct WindowUIInitInfo
	{
		std::shared_ptr<WindowSystem> windowSystem;
		std::shared_ptr<RenderSystem> renderSystem;
	};

	class WindowUI
	{
	public:
		virtual void Initialize(const WindowUIInitInfo& initInfo) = 0;
		virtual void PreRender() = 0;
	};


}