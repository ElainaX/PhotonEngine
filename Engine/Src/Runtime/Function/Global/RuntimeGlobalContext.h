#pragma once

#include <memory>

namespace photon
{
	class LogManager;

	class RuntimeGlobalContext
	{
	public:
		void StartSubSystems();
		void ShutDownSubSystems();

	public:
		std::shared_ptr<LogManager> logManager;
	};

	extern RuntimeGlobalContext g_RuntimeGlobalContext;
} // end photon namespace