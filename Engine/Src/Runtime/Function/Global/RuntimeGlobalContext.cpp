#include "RuntimeGlobalContext.h"
#include "Core/Log/LogManager.h"
#include "Function/Render/WindowSystem.h"
#include "Function/Render/RenderSystem.h"
#include "Function/Input/InputSystem.h"

namespace photon
{
	RuntimeGlobalContext g_RuntimeGlobalContext;

	void RuntimeGlobalContext::StartSubSystems(WindowCreateInfo wndCreateInfo)
	{
		logManager = std::make_shared<LogManager>();
		logManager->StartUp();
		
		windowSystem = std::make_shared<WindowSystem>();
		windowSystem->Initialize(wndCreateInfo);

		renderSystem = std::make_shared<RenderSystem>();
		RenderSystemInitInfo rsInitInfo;
		rsInitInfo.windowSystem = windowSystem;
		renderSystem->Initialize(rsInitInfo);

		inputSystem = std::make_shared<InputSystem>();
		inputSystem->Initialize();
	}

	void RuntimeGlobalContext::ShutDownSubSystems()
	{
		inputSystem.reset();

		windowSystem->CloseAllWindows();
		windowSystem.reset();

		logManager->ShutDown();
		logManager.reset();
	}

}