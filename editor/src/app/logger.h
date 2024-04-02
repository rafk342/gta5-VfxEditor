#pragma once

#include <format>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"

#define mlogger(arg) mlogger_(arg)
inline void mlogger_(const std::string& str)
{
	static std::shared_ptr<spdlog::logger> logger = spdlog::basic_logger_mt("log", "__TceLog.log", std::ios_base::trunc);
	logger->info(str);
	logger->flush();
}

inline void logPtrBytes(u8* address, size_t len)
{
	std::string res;

	for (int i = 0; i < len; ++i)
	{
		res += std::format("{:02X} ", address[i]);
	}
	mlogger(res);
}

