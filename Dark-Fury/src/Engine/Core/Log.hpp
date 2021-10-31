#pragma once

#include "Engine/Core/Base.hpp"

#define MAX_LOG_SIZE (1024 * sizeof(char))

namespace Engine {

	class Logger
	{
	public:
		Logger(const char* name);
		~Logger() {};
		void trace(const char* frmt, ...);
		void info(const char* frmt, ...);
		void warn(const char* frmt, ...);
		void error(const char* frmt, ...);
		void critical(const char* frmt, ...);

	private:
		const char* _logName;
	};

	class Log
	{
	public:
		static void Init();

		static std::shared_ptr<Logger>& GetCoreLogger() { return s_CoreLogger; }
		static std::shared_ptr<Logger>& GetClientLogger() { return s_ClientLogger; }
	private:
		static std::shared_ptr<Logger> s_CoreLogger;
		static std::shared_ptr<Logger> s_ClientLogger;
	};

};

// Core log macros
#define ENG_CORE_TRACE(...)    ::Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ENG_CORE_INFO(...)     ::Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ENG_CORE_WARN(...)     ::Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ENG_CORE_ERROR(...)    ::Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ENG_CORE_CRITICAL(...) ::Engine::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define ENG_TRACE(...)         ::Engine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ENG_INFO(...)          ::Engine::Log::GetClientLogger()->info(__VA_ARGS__)
#define ENG_WARN(...)          ::Engine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ENG_ERROR(...)         ::Engine::Log::GetClientLogger()->error(__VA_ARGS__)
#define ENG_CRITICAL(...)      ::Engine::Log::GetClientLogger()->critical(__VA_ARGS__)