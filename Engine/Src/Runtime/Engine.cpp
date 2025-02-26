#include "Engine.h"
#include "Test/SandBox.h"
#include "Test/RuntimeGlobalContext.h"


namespace photon
{
	void PhotonEngine::StartEngine()
	{
		// Startup SubSystem
		g_RuntimeGlobalContext.StartSubSystems();
	}

	void PhotonEngine::ShutDownEngine()
	{
		// Shutdown SubSystem
		g_RuntimeGlobalContext.ShutDownSubSystems();
	}

	void PhotonEngine::Run()
	{
		// Run Code
		SandBox sandBox;
		sandBox.Run();
	}

	PhotonEngine::~PhotonEngine()
	{
		// Do Nothing
	}

}