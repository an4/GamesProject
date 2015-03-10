#ifdef IN_UE4
#include "GPProject.h"
#include "AssertionMacros.h"
#endif IN_UE4

#include "OCVSPacketScanHeader.h"

#ifndef IN_UE4
#include <cassert>
#endif


OCVSPacketScanHeader::OCVSPacketScanHeader(uint8_t result)
	: result(result)
	, length(0)
	, chunk_count(0)
{
}



OCVSPacketScanHeader::OCVSPacketScanHeader(uint32_t length, uint32_t chunk_count)
	: result(RESULT_SUCCESS)
	, length(length)
	, chunk_count(chunk_count)
{
}


OCVSPacketScanHeader::OCVSPacketScanHeader(const std::vector<OCVSPacket *> &scanChunks)
	: result(RESULT_SUCCESS)
	, chunk_count(scanChunks.size())
{
	length = 0;
	
	for (auto &pkt : scanChunks)
	{
		length += pkt->GetPackedSize();
	}
}


OCVSPacketScanHeader::OCVSPacketScanHeader(const std::vector<char> &packet)
{
	int i;
	// First 4 bytes is the length.
	// TODO: Cheeky reinterpret maybe???
	length = 0;
	for (i = sizeof(uint32) - 1; i >= 0; i--) {
		length = length << 8;
		length = length | packet.at(i);
	}
	const int start = sizeof(uint32) + sizeof(uint32) - 1;
	chunk_count = 0;
	for (i = start; i >= sizeof(uint32); i--) {
		chunk_count = chunk_count << 8;
		chunk_count = chunk_count | packet.at(i);
	}
	// TODO: Check!
}


OCVSPacketScanHeader::~OCVSPacketScanHeader()
{
}


void OCVSPacketScanHeader::Pack(std::vector<char> &buff)
{
	buff.clear();

	// Send the result code.
	buff.push_back(result);

	char *asBytes = reinterpret_cast<char *>(&length);

	// Send the total length
	buff.insert(buff.end(), asBytes, asBytes+sizeof(length));

	asBytes = reinterpret_cast<char *>(&chunk_count);

	// Send the chunk count
	buff.insert(buff.end(), asBytes, asBytes + sizeof(chunk_count));

#ifdef IN_UE4
	check(buff.size() == GetPackedSize());
#else
	assert(buff.size() == GetPackedSize());
#endif
}


size_t OCVSPacketScanHeader::GetPackedSize() const
{
	// Fixed length of one 8 bit field and two 32 bit fields
	return 9;
}


uint32 OCVSPacketScanHeader::GetChunkCount() const
{
	// TODO: Type crying!
	return (uint32)chunk_count;
}
