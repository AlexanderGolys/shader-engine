#include "macros.hpp"
#include "logging.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <cstdarg>

namespace logging {

	// Opaque logger handles
	void* Logger::engine_logger = nullptr;
	void* Logger::external_logger = nullptr;
	void* Logger::pure_logger = nullptr;
	unordered_map<string, long> Logger::time_points = unordered_map<string, long>();

	void Logger::init() {
		engine_logger = (void*)spdlog::stdout_color_mt("Engine").get();

		external_logger = (void*)spdlog::stdout_color_mt("External").get();
		pure_logger = (void*)spdlog::stdout_color_mt("Pure").get();
	}

	void* Logger::getEngineLogger() { return engine_logger; }
	void* Logger::getExternalLogger() { return external_logger; }
	void* Logger::getPureLogger() { return pure_logger; }

	static spdlog::logger* get_logger(void* handle) {
		return static_cast<spdlog::logger*>(handle);
	}

	long Logger::measureTimeDifference(const string &name) {
		if (!time_points.contains(name))
		{
			log_warn("No time point with name " + name + " found!");
			return -1.0;
		}
		long now = getTimePoint();
		long diff = now - time_points[name];
		time_points.erase(name);
		return diff;
	}

	long Logger::getTimePoint() {
		auto tm = spdlog::log_clock::now();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(tm.time_since_epoch()).count();
		return ms;
	}

	void Logger::setTimePoint(const string &name) {
		time_points[name] = getTimePoint();
	}

	void log_info(const char* fmt, ...) {
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, sizeof(buf), fmt, args);
		va_end(args);
		get_logger(Logger::engine_logger)->info(buf);
	}
	void log_error(const char* fmt, ...) {
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, sizeof(buf), fmt, args);
		va_end(args);
		get_logger(Logger::engine_logger)->error(buf);
	}
	void log_warn(const char* fmt, ...) {
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, sizeof(buf), fmt, args);
		va_end(args);
		get_logger(Logger::engine_logger)->warn(buf);
	}

	void log_info(const string& msg) {
		log_info("%s", msg.c_str());
	}
	void log_error(const string& msg) {
		log_error("%s", msg.c_str());
	}
	void log_warn(const string& msg) {
		log_warn("%s", msg.c_str());
	}

} // namespace logging
