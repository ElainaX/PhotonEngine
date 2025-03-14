#include "RuntimeGlobalContext.h"
#include "Core/Log/LogManager.h"
#include "Function/Render/WindowSystem.h"
#include "Function/Render/RenderSystem.h"

namespace photon
{
	RuntimeGlobalContext g_RuntimeGlobalContext;

	void RuntimeGlobalContext::StartSubSystems()
	{
		logManager = std::make_shared<LogManager>();
		logManager->StartUp();
		
		windowSystem = std::make_shared<WindowSystem>();
		WindowCreateInfo wndCreateInfo;
		windowSystem->Initialize(wndCreateInfo);

		renderSystem = std::make_shared<RenderSystem>();
		RenderSystemInitInfo rsInitInfo;
		rsInitInfo.windowSystem = windowSystem;
		renderSystem->Initialize(rsInitInfo);
	}

	void RuntimeGlobalContext::ShutDownSubSystems()
	{
		windowSystem->CloseAllWindows();
		windowSystem.reset();

		logManager->ShutDown();
		logManager.reset();
	}

}