#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace photon
{
	class LogManager
	{
	public:
		LogManager() = default;
		~LogManager() = default;

		void StartUp();
		void ShutDown();


		template<typename T>
		void LogFatal(const T& msg)
		{
			m_logger->critical(msg);
		}

		template<typename T>
		void LogError(const T& msg)
		{
			m_logger->error(msg);
		}

		template<typename T>
		void LogWarn(const T& msg)
		{
			m_logger->warn(msg);
		}

		template<typename T>
		void LogInfo(const T& msg)
		{
			m_logger->info(msg);
		}

		template<typename T>
		void Log(const T& msg)
		{
			m_logger->debug(msg);
		}

	private:
		std::shared_ptr<spdlog::logger> m_logger;
	};


}