#include <tech/logger.h>

/*
#ifndef NDEBUG
namespace Tech {


static Logger logger;


Logger::Logger() :
	buffer_(1024)
{
	fd_ = socket(AF_UNIX, SOCK_STREAM, 0);
	if(fd_ < 0)
		return;

	sockaddr_un address;
	memset(&address, 0, sizeof(sockaddr_un));
	address.sun_family = AF_UNIX;
	snprintf(address.sun_path, 108, LOG_SOCK_PATH);
	if(connect(fd_, reinterpret_cast<sockaddr*>(&address), sizeof(sockaddr_un)) != 0) {
		close(fd_);
		fd_ = -1;
	}
}


Logger::~Logger()
{
	if(fd_ >= 0)
		close(fd_);
}


void Logger::log(const char* format, ...)
{
	if(logger.fd_) {
		char* buffer = logger.buffer_.data();
		va_list args;

		va_start(args, format);
		uint count = std::vsnprintf(buffer, logger.buffer_.size(), format, args) + 1;
		va_end(args);

		send(logger.fd_, buffer, count, 0);
	}
}


} // namespace Tech


#endif // NDEBUG
*/
