#ifndef TECH_LOGGER_H
#define TECH_LOGGER_H

#include <tech/format.h>
#include <tech/utils.h>


#define LOG(format, ...) Tech::logMessage(__FILENAME__, __LINE__, format, ##__VA_ARGS__)


namespace Tech {


using LogMessageHandler = void(*)(const String&, int, const String&);


void setLogMessageHandler(LogMessageHandler handler);
void logMessage(const char* fileName, int line, const String& message);


template<typename ...Args>
void logMessage(const char* fileName, int line, const char* format, const Args&... args)
{
	logMessage(fileName, line, Formatter::format(format, args...));
}


} // namespace Tech


#endif // TECH_LOGGER_H
