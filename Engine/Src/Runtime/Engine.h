#pragma once

#include "Function/Util/GameTimer.h"

namespace photon
{
	class WindowCreateInfo;

	// 资源管理方式都很简单
	class PhotonEngine
	{
	public:
		PhotonEngine() = default;
		PhotonEngine(const PhotonEngine&) = delete;

		void StartEngine(WindowCreateInfo wndCreateInfo);
		void ShutDownEngine();

		GameTimer* GetTimer();

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