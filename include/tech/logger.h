#ifndef TECH_LOGGER_H
#define TECH_LOGGER_H

#include <cstdio>

#define LOG(format, ...) fprintf(stderr, format "\n", ##__VA_ARGS__)


/*
#include <vector>

#ifndef NDEBUG
#define LOG(format, ...) Tech::Logger::log(format "\n", ##__VA_ARGS__)

namespace Tech {


class Logger {
public:
	Logger();
	~Logger();

	static void log(const char* format, ...);

private:
	int fd_;
	std::vector<char> buffer_;
};


} // namespace Tech


#else
#define LOG(format, ...)
#endif //  NDEBUG
*/

#endif // TECH_LOGGER_H
