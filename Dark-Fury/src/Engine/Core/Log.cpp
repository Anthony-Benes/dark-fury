#include "engpch.h"
#include "Engine/Core/Log.hpp"

#ifdef ENG_PLATFORM_WINDOWS
#include <Windows.h>
auto hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
#define SetColor(color)		SetConsoleTextAttribute(hStdOut, color)
#define ClearColor()		SetConsoleTextAttribute(hStdOut, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY)
#else
#define SetColor(color)		printf("\033[1;36m");//printf("\033[1;92m");printf("\033[1;33m");printf("\033[1;31m");printf("\033[1;4;31m");
#define Clear()				printf("\033[0m");
#endif

namespace Engine {

	Logger::Logger(const char* name) {
		_logName = name;
	}

	void Logger::trace(const char* frmt, ...) {
		va_list args;
		va_start(args, frmt);
		ClearColor();
		printf("[%s] ", _logName);
		vprintf(frmt, args);
		printf("\n");
		va_end(args);
	}

	void Logger::info(const char* frmt, ...) {
		va_list args;
		va_start(args, frmt);
		SetColor(FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		printf("[%s] ", _logName);
		vprintf(frmt, args);
		ClearColor();
		printf("\n");
		va_end(args);
	}

	void Logger::warn(const char* frmt, ...) {
		va_list args;
		va_start(args, frmt);
		SetColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
		printf("[%s] ", _logName);
		vprintf(frmt, args);
		ClearColor();
		printf("\n");
		va_end(args);
	}

	void Logger::error(const char* frmt, ...) {
		va_list args;
		va_start(args, frmt);
		SetColor(FOREGROUND_RED | FOREGROUND_INTENSITY);
		printf("[%s] ", _logName);
		vprintf(frmt, args);
		ClearColor();
		printf("\n");
		va_end(args);
	}

	void Logger::critical(const char* frmt, ...) {
		va_list args;
		va_start(args, frmt);
		SetColor(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
		printf("[%s] ", _logName);
		vprintf(frmt, args);
		ClearColor();
		printf("\n");
		va_end(args);
	}

	std::shared_ptr<Logger> Log::s_CoreLogger;
	std::shared_ptr<Logger> Log::s_ClientLogger;

	void Log::Init() {
		s_CoreLogger = std::make_shared<Logger>(Logger("Engine"));
		s_ClientLogger = std::make_shared<Logger>(Logger("App"));
	}
};