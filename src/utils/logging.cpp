#include "macros.hpp"
#include "logging.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <cstdarg>

namespace logging {

	// Opaque logger handles
	void* Logger::engine_logger = nullptr;
	void* Logger::pure_logger = nullptr;
	void* Logger::test_logger = nullptr;
	unordered_map<string, long> Logger::time_points = unordered_map<string, long>();

	void Logger::init() {
		engine_logger = (void*)spdlog::stdout_color_mt("Engine", spdlog::color_mode::always).get();
		pure_logger = (void*)spdlog::stdout_color_mt("Pure").get();
		test_logger = (void*)spdlog::stdout_color_mt("Tests").get();
	}

	void* Logger::getEngineLogger() {
		return engine_logger;
	}
	void* Logger::getPureLogger() {
		return pure_logger;
	}
	void* Logger::getTestLogger() {
		return test_logger;
	}

	static spdlog::logger* get_logger(void* handle) {
		auto logger_ = static_cast<spdlog::logger*>(handle);
		logger_->set_pattern("[%H:%M:%S.%e] [%^E%$: %l] %s %v");
		return logger_;
	}

	static spdlog::logger* get_test_logger(void* handle) {
		auto logger_ = static_cast<spdlog::logger*>(handle);
		logger_->set_pattern("[%H:%M:%S.%e] [%^T%$] %^%s %v%$");
		return logger_;
	}

	static spdlog::logger* get_pure_logger(void* handle) {
		auto logger_ = static_cast<spdlog::logger*>(handle);
		logger_->set_pattern("%s %v");
		return logger_;
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
	void log_test_ok(const char* fmt, ...) {
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, sizeof(buf), fmt, args);
		va_end(args);
		get_test_logger(Logger::test_logger)->info(buf);
	}
	void log_test_fail(const char* fmt, ...) {
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, sizeof(buf), fmt, args);
		va_end(args);
		get_test_logger(Logger::test_logger)->error(buf);
	}

	void log_test_ok(const string &msg) {
		log_test_ok("%s", msg.c_str());
	}

	void log_test_fail(const string &msg) {
		log_test_fail("%s", msg.c_str());
	}

	void log_info_pure(const char* fmt, ...) {
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, sizeof(buf), fmt, args);
		va_end(args);
		get_pure_logger(Logger::pure_logger)->info(buf);
	}
	void log_error_pure(const char* fmt, ...) {
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, sizeof(buf), fmt, args);
		va_end(args);
		get_pure_logger(Logger::pure_logger)->error(buf);
	}
	void log_warn_pure(const char* fmt, ...) {
		char buf[1024];
		va_list args;
		va_start(args, fmt);
		vsnprintf(buf, sizeof(buf), fmt, args);
		va_end(args);
		get_pure_logger(Logger::pure_logger)->warn(buf);
	}

	void log_info_pure(const string& msg) {
		log_info_pure("%s", msg.c_str());
	}
	void log_error_pure(const string& msg) {
		log_error_pure("%s", msg.c_str());
	}
	void log_warn_pure(const string& msg) {
		log_warn_pure("%s", msg.c_str());
	}
} // namespace logging
