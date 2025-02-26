#include "LogManager.h"

#include <spdlog/async.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace photon 
{

	void LogManager::StartUp()
	{
		auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		consoleSink->set_level(spdlog::level::trace);
		consoleSink->set_pattern("[%^%l%$] %v");

		const spdlog::sinks_init_list sinkList = { consoleSink };
		spdlog::init_thread_pool(8192, 1);

		m_logger = std::make_shared<spdlog::async_logger>("Logger",
			sinkList.begin(), sinkList.end(),
			spdlog::thread_pool(),
			spdlog::async_overflow_policy::block);

		m_logger->set_level(spdlog::level::trace);

		spdlog::register_logger(m_logger);
	}

	void LogManager::ShutDown()
	{
		m_logger->flush();
		spdlog::drop_all();
	}

}