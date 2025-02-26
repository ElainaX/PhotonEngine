#pragma once

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

		void Run();

		~PhotonEngine();
	};
}