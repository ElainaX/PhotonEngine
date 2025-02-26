#include "RuntimeGlobalContext.h"
#include "Core/Log/LogManager.h"

namespace photon
{
	RuntimeGlobalContext g_RuntimeGlobalContext;

	void RuntimeGlobalContext::StartSubSystems()
	{
		logManager = std::make_shared<LogManager>();
		logManager->StartUp();
		
	}

	void RuntimeGlobalContext::ShutDownSubSystems()
	{
		logManager->ShutDown();
		logManager.reset();
	}

}