#include "RuntimeGlobalContext.h"
#include "Core/Log/LogManager.h"
#include "Function/Render/WindowSystem.h"

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

	}

	void RuntimeGlobalContext::ShutDownSubSystems()
	{
		windowSystem->CloseAllWindows();
		windowSystem.reset();

		logManager->ShutDown();
		logManager.reset();
	}

}