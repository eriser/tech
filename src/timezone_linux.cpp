#include "timezone_linux.h"

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <tech/utils.h>


namespace Tech {


namespace {

struct Header {
	u32  magic;
	char version;
	u8   reserved[15];
	u32  isGmtCount;
	u32  isStdCount;
	u32  leapCount;
	u32  timeCount;
	u32  typeCount;
	u32  charCount;
} __attribute__ ((packed));


struct Info {
	i32 gmtOffset;
	u8  isDst;
	u8  abbrIndex;
} __attribute__ ((packed));

}


const ByteArray TimeZoneImpl::tzDir_ = TimeZoneImpl::tzDir();
ByteArray TimeZoneImpl::local_;


TimeZoneImpl::TimeZoneImpl(const ByteArray& name) :
	Implementation<TimeZone>(nullptr)
{
	ByteArray tzFile;
	if(name == "localtime") {
		tzFile = "/etc/localtime";
	}
	else {
		tzFile = tzDir_ + name;
	}

	if(parseTimeZoneFile(tzFile)) {
		name_ = name;
	}
	else {
		parseTimeZoneFile(tzDir_ + "UTC");
		name_ = "UTC";
	}
}


ByteArray TimeZoneImpl::name() const
{
	return name_;
}


Duration TimeZoneImpl::fromUtc(const Duration& utc) const
{
	auto it = std::upper_bound(transitions_.begin(), transitions_.end(), utc,
	        Comparator());

	const Transition* transition;

	if(it == transitions_.end()) {
		transition = &transitions_.back();
	}
	else {
		transition = &(*it);
	}

	return utc + transition->utcOffset;
}


Duration TimeZoneImpl::toUtc(const Duration& local, bool isDst) const
{
	// FIXME
	return Duration();
}


TimeZone TimeZoneImpl::local()
{
	if(local_.isEmpty()) {
		// Check the TZ environment variable for a timezone name
		const char* value = getenv("TZ");
		if(value)
			local_ = value;

		if(local_.isEmpty()) {
			// TZ is empty, try to read local timezone from the /etc/timezone file
			local_ = readTimeZone();

			if(local_.isEmpty()) {
				// Local timezone still not found, try to analyze the /etc/localtime file
				local_ = readLocalTime();
				if(local_.isEmpty()) {
					// All attempts failed, select UTC as our local timezone
					local_ = "UTC";
				}
			}
		}
	}

	return TimeZone(local_);
}


TimeZone TimeZoneImpl::utc()
{
	return TimeZone("UTC");
}


ByteArrayList TimeZoneImpl::availableTimeZones()
{
	ByteArrayList result;

	scanDirectory(tzDir_, tzDir_.length(), &result);
	return result;
}


bool TimeZoneImpl::parseTimeZoneFile(const ByteArray& path)
{
	auto readHeader = [] (FILE* f, Header* header) -> bool {
		if(fread(header, sizeof(Header), 1, f) != 1)
			return false;

		header->magic      = fromBigEndian(header->magic);
		header->isGmtCount = fromBigEndian(header->isGmtCount);
		header->isStdCount = fromBigEndian(header->isStdCount);
		header->leapCount  = fromBigEndian(header->leapCount);
		header->timeCount  = fromBigEndian(header->timeCount);
		header->typeCount  = fromBigEndian(header->typeCount);
		header->charCount  = fromBigEndian(header->charCount);

		return header->magic == MAKE_MAGIC('T', 'Z', 'i', 'f');
	};


	FILE* f = fopen(path, "r");
	Header header;

	if(!readHeader(f, &header)) {
		fclose(f);
		return false;
	}

	size_t timeSize = sizeof(i32);

	size_t dataSize = (sizeof(i32) + sizeof(u8)) * header.timeCount +
			sizeof(Info) * header.typeCount + header.charCount +
			sizeof(i32) * 2 * header.leapCount + sizeof(u8) * header.isStdCount +
			sizeof(u8) * header.isGmtCount;

	if(sizeof(time_t) == 8 && header.version == '2') {
		fseek(f, dataSize, SEEK_CUR);
		dataSize += sizeof(i32) * (header.timeCount + header.leapCount);
		timeSize = sizeof(i64);

		if(!readHeader(f, &header)) {
			fclose(f);
			return false;
		}
	}

	u8 data[dataSize];
	if(fread(data, dataSize, 1, f) != 1) {
		fclose(f);
		return false;
	}

	transitions_.resize(header.timeCount);

	u8* timePos = data;
	u8* indexPos = timePos + timeSize * header.timeCount;
	u8* infoPos  = indexPos + header.timeCount;

	for(u32 i = 0; i < header.timeCount; ++i) {
		Transition& transition = transitions_[i];

		u8* pos = timePos + timeSize * i;
		if(timeSize == sizeof(i64)) {
			i64 value = fromBigEndian(*reinterpret_cast<i64*>(pos));
			transition.timestamp = Duration(value * 1000ll);
		}
		else {
			i32 value = fromBigEndian(*reinterpret_cast<i32*>(pos));
			transition.timestamp = Duration(value * 1000ll);
		}

		Info* info = reinterpret_cast<Info*>(infoPos + sizeof(Info) * indexPos[i]);
		i32 offset = fromBigEndian(info->gmtOffset);
		transition.utcOffset = Duration(offset * 1000ll);

		transition.isDst = info->isDst;
	}

	fclose(f);
	return true;
}


ByteArray TimeZoneImpl::tzDir()
{
	const char* value = getenv("TZDIR");
	if(value) {
		ByteArray result = value;
		if(!result.endsWith('/'))
			result += '/';

		return result;
	}

	return "/usr/share/zoneinfo/";
}


void TimeZoneImpl::scanDirectory(const ByteArray& path, uint prefixLength,
		ByteArrayList* result)
{
	DIR* dir = opendir(path);
	dirent* ent;
	struct stat st;

	while((ent = readdir(dir)) != nullptr) {
		ByteArray name = path + ent->d_name;

		if(name.endsWith('.') || name.endsWith("/posix") || name.endsWith("/posixrules")
		   		|| name.endsWith("/right"))
			continue;

		if(stat(name, &st) == -1)
			continue;

		if(S_ISDIR(st.st_mode)) {
			scanDirectory(name + '/', prefixLength, result);
		}
		else if(S_ISREG(st.st_mode)) {
			FILE* f = fopen(name, "rb");
			if(!f)
				continue;

			u32 magic;
			if(fread(&magic, sizeof(u32), 1, f) != 1) {
				fclose(f);
				continue;
			}

			if(fromBigEndian(magic) == MAKE_MAGIC('T', 'Z', 'i', 'f'))
				result->push_back(name.middleRef(prefixLength));

			fclose(f);
		}
	}
}


ByteArray TimeZoneImpl::readTimeZone()
{
	FILE* f;
	char* line = nullptr;
	size_t len = 0;
	ByteArray result;

	f = fopen("/etc/timzone", "r");
	if(f) {
		while(getline(&line, &len, f) != -1) {
			result = ByteArray(line).trimmed();
			if(!result.startsWith('#'))
				break;

			result.clear();
			continue;
		}

		if(line)
			free(line);

		fclose(f);
	}

	return result;
}


ByteArray TimeZoneImpl::readLocalTime()
{
	struct stat st;

	if(lstat("/etc/localtime", &st) != -1) {
		if(S_ISLNK(st.st_mode)) {
			char buffer[PATH_MAX];
			ssize_t length = readlink("/etc/localtime", buffer, PATH_MAX);
			if(length < 1)
				return ByteArray();

			ByteArray fileName(buffer, length);
			if(!fileName.startsWith(tzDir_))
				return ByteArray();

			return fileName.middleRef(tzDir_.length());
		}
		else if(S_ISREG(st.st_mode)) {
			// The /etc/localtime is a regular file. We can't find its proper name, so we
			// just return a placeholder here.
			return "localtime";
		}
	}

	return ByteArray();
}


} // namespace Tech
