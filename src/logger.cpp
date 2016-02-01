#include <tech/logger.h>


namespace Tech {


static LogMessageHandler logMessageHandler;


void setLogMessageHandler(LogMessageHandler handler)
{
	logMessageHandler = handler;
}


void logMessage(const char* file, int line, const String& message)
{
	if(logMessageHandler)
		logMessageHandler(message);
}


} // namespace Tech


