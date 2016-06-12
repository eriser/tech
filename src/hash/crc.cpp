#include <tech/hash/crc.h>


namespace Tech {


// Dallas / Maxim
static const u8 kCrc8        = 0x31;

// ITU X.25/T.30
static const u16 kCrc16      = 0x8005;

// CRC-CCITT
static const u16 kCrc16Ccitt = 0x1021;

// ISO 3309
static const u32 kCrc32      = 0x04C11DB7;

// ISO 3309
static const u64 kCrc64      = 0x000000000000001B;


static Box<u8> crc8Table;
static Box<u16> crc16Table;
static Box<u16> crc16CcittTable;
static Box<u32> crc32Table;
static Box<u64> crc64Table;


u8 crc8(const void* data, size_t length)
{
	if(!crc8Table)
		crc8Table.reset(generateReverseCrcTable<u8>(kCrc8));

	return calculateReverseCrc<u8>(data, length, crc8Table.get(), 0xFF);
}


u8 crc8Zero(const void* data, size_t length)
{
	if(!crc8Table)
		crc8Table.reset(generateReverseCrcTable<u8>(kCrc8));

	return calculateReverseCrc<u8>(data, length, crc8Table.get(), 0x00);
}


u16 crc16(const void* data, size_t length)
{
	if(!crc16Table)
		crc16Table.reset(generateReverseCrcTable<u16>(kCrc16));

	return calculateReverseCrc<u16>(data, length, crc16Table.get(), 0xFFFF);
}


u16 crc16Ccitt(const void* data, size_t length)
{
	if(!crc16CcittTable)
		crc16CcittTable.reset(generateReverseCrcTable<u16>(kCrc16Ccitt));

	return ~calculateReverseCrc<u16>(data, length, crc16CcittTable.get(), 0xFFFF);
}


u32 crc32(const void* data, size_t length)
{
	if(!crc32Table)
		crc32Table.reset(generateReverseCrcTable<u32>(kCrc32));

	return calculateReverseCrc<u32>(data, length, crc32Table.get(), 0xFFFFFFFF);
}


u64 crc64(const void* data, size_t length)
{
	if(!crc64Table)
		crc64Table.reset(generateReverseCrcTable<u64>(kCrc64));

	return calculateReverseCrc<u64>(data, length, crc64Table.get(), 0xFFFFFFFFFFFFFFFF);
}


}
