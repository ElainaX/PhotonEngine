#pragma once

#include <memory>

namespace photon
{
	class LogManager;
	class WindowSystem;
	class RenderSystem;
	class InputSystem;
	class WindowCreateInfo;

	class RuntimeGlobalContext
	{
	public:
		void StartSubSystems(WindowCreateInfo wndCreateInfo);
		void ShutDownSubSystems();

	public:
		std::shared_ptr<LogManager> logManager;
		std::shared_ptr<WindowSystem> windowSystem;
		std::shared_ptr<RenderSystem> renderSystem;
		std::shared_ptr<InputSystem> inputSystem;
	};

	extern RuntimeGlobalContext g_RuntimeGlobalContext;
} // end photon namespace