#include "Engine.h"
#include "Test/SandBox.h"
#include "Function/Global/RuntimeGlobalContext.h"
#include "Function/Render/WindowSystem.h"
#include "Function/Render/RenderSystem.h"

#include <cassert>

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

	void PhotonEngine::TickOneFrame(GameTimer& timer)
	{
		TickLogical(timer);
		TickRenderer(timer);
	}

	void PhotonEngine::TickLogical(GameTimer& timer)
	{
		g_RuntimeGlobalContext.windowSystem->PollEvents();
	}

	void PhotonEngine::TickRenderer(GameTimer& timer)
	{
		g_RuntimeGlobalContext.renderSystem->Tick(timer);
	}

	void PhotonEngine::Run()
	{
		//// Run Code
		SandBox sandBox;
		sandBox.Run();
		m_Timer.Reset();
		assert(g_RuntimeGlobalContext.windowSystem);
		while(!g_RuntimeGlobalContext.windowSystem->ShouldClose())
		{
			m_Timer.Tick();
			TickOneFrame(m_Timer);

		}
	}

	PhotonEngine::~PhotonEngine()
	{
		// Do Nothing
	}

}