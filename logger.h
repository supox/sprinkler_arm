#pragma once

class Logger
{
public:
	enum LoggerLevel {
		DUMP,
		NORMAL,
		WARNING,
		ERROR
	};

	static void AddLine(const char* message, const LoggerLevel level);

	private:
		Logger(){}
};
