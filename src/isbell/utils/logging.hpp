#pragma once

#include "macros.hpp"


namespace logging {
	#ifdef LOG
	#undef LOG
	#endif
	#ifdef ERROR
	#undef ERROR
	#endif
	#ifdef WARN
	#undef WARN
	#endif

	class Logger {
	public:

		static void* engine_logger;
		static void* pure_logger;
		static void* test_logger;
		static unordered_map<string, long> time_points;

		static void init();
		static void* getEngineLogger();
		static void* getPureLogger();
		static void* getTestLogger();
		static long getTimePoint();
		static void setTimePoint(const string& name);
		static long measureTimeDifference(const string& name);

	};

	void log_info(const char* fmt, ...);
	void log_error(const char* fmt, ...);
	void log_warn(const char* fmt, ...);
	void log_info(const string& msg);
	void log_error(const string& msg);
	void log_warn(const string& msg);

	void log_test_ok(const char* fmt, ...);
	void log_test_fail(const char* fmt, ...);
	void log_test_ok(const string& msg);
	void log_test_fail(const string& msg);

	void log_info_pure(const char* fmt, ...);
	void log_error_pure(const char* fmt, ...);
	void log_warn_pure(const char* fmt, ...);
	void log_info_pure(const string& msg);
	void log_error_pure(const string& msg);
	void log_warn_pure(const string& msg);

	#define LOG(...) ::logging::log_info(__VA_ARGS__)
	#define LOG_ERROR(...) ::logging::log_error(__VA_ARGS__)
	#define LOG_WARN(...) ::logging::log_warn(__VA_ARGS__)

	#define LOG_TEST_OK(...) ::logging::log_test_ok(__VA_ARGS__)
	#define LOG_TEST_FAIL(...) ::logging::log_test_fail(__VA_ARGS__)

	#define LOG_PURE(...) ::logging::log_info_pure(__VA_ARGS__)
	#define LOG_ERROR_PURE(...) ::logging::log_error_pure(__VA_ARGS__)
	#define LOG_WARN_PURE(...) ::logging::log_warn_pure(__VA_ARGS__)

	#define START_TIMER(name) ::logging::Logger::setTimePoint(name);
	#define STOP_TIMER(name) ::logging::Logger::measureTimeDifference(name);
}
