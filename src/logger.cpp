#include <tech/logger.h>


namespace Tech {


static LogMessageHandler logMessageHandler = nullptr;


void setLogMessageHandler(LogMessageHandler handler)
{
	logMessageHandler = handler;
}


void logMessage(const char* fileName, int line, const String& message)
{
	if(logMessageHandler)
		logMessageHandler(fileName, line, message);
}


} // namespace Tech


