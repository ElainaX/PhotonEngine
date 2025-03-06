#pragma once

#include "Function/Util/GameTimer.h"

namespace photon
{
	// 资源管理方式都很简单
	class PhotonEngine
	{
	public:
		PhotonEngine() = default;
		PhotonEngine(const PhotonEngine&) = delete;

		void StartEngine();
		void ShutDownEngine();

	public:
		void TickOneFrame(GameTimer& timer);

		void TickLogical(GameTimer& timer);
		void TickRenderer(GameTimer& timer);

		void Run();

		~PhotonEngine();

	private:
		GameTimer m_Timer;
	};
}