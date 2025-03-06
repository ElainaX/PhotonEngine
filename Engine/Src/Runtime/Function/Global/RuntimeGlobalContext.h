#pragma once

#include <memory>

namespace photon
{
	class LogManager;
	class WindowSystem;

	class RuntimeGlobalContext
	{
	public:
		void StartSubSystems();
		void ShutDownSubSystems();

	public:
		std::shared_ptr<LogManager> logManager;

		std::shared_ptr<WindowSystem> windowSystem;
	};

	extern RuntimeGlobalContext g_RuntimeGlobalContext;
} // end photon namespace