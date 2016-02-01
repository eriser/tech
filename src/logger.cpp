#include <tech/logger.h>


namespace Tech {


static LogMessageHandler logMessageHandler = nullptr;


void setLogMessageHandler(const LogMessageHandler& handler)
{
	logMessageHandler = handler;
}


void logMessage(const char* fileName, int line, const String& message)
{
	logMessageHandler(fileName, line, message);
}


} // namespace Tech
