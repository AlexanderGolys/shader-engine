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
		static void* external_logger;
		static void* pure_logger;
		static unordered_map<string, long> time_points;

		static void init();
		static void* getEngineLogger();
		static void* getExternalLogger();
		static void* getPureLogger();
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

	#define LOG(...) ::logging::log_info(__VA_ARGS__)
	#define LOG_ERROR(...) ::logging::log_error(__VA_ARGS__)
	#define LOG_WARN(...) ::logging::log_warn(__VA_ARGS__)

	#define APP_LOG(...) ::logging::log_info(__VA_ARGS__)
	#define APP_LOG_ERROR(...) ::logging::log_error(__VA_ARGS__)
	#define APP_LOG_WARN(...) ::logging::log_warn(__VA_ARGS__)

	#define PURE_LOG(...) ::logging::log_info(__VA_ARGS__)
	#define PURE_LOG_ERROR(...) ::logging::log_error(__VA_ARGS__)
	#define PURE_LOG_WARN(...) ::logging::log_warn(__VA_ARGS__)

	#define START_TIMER(name) ::logging::Logger::setTimePoint(name);
	#define STOP_TIMER(name) ::logging::Logger::measureTimeDifference(name);
}
