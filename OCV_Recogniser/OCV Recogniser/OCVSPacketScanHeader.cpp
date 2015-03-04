#include "OCVSPacketScanHeader.h"
#include <cassert>


OCVSPacketScanHeader::OCVSPacketScanHeader(uint32_t length, uint32_t chunk_count)
	: length(length)
	, chunk_count(chunk_count)
{
}


OCVSPacketScanHeader::OCVSPacketScanHeader(const std::vector<OCVSPacket> &scanChunks)
	: chunk_count(scanChunks.size())
{
	length = 0;

	for each (const auto &pkt in scanChunks)
	{
		length += pkt.GetPackedSize();
	}
}


OCVSPacketScanHeader::~OCVSPacketScanHeader()
{
}


void OCVSPacketScanHeader::Pack(std::vector<char> &buff)
{
	buff.clear();

	char *asBytes = reinterpret_cast<char *>(&length);

	// Send the total length
	buff.insert(buff.end(), asBytes, asBytes+sizeof(length));

	asBytes = reinterpret_cast<char *>(&chunk_count);

	// Send the chunk count
	buff.insert(buff.end(), asBytes, asBytes + sizeof(chunk_count));

	assert(buff.size() == GetPackedSize());
}


size_t OCVSPacketScanHeader::GetPackedSize() const
{
	// Fixed length of two 32 bit fields
	return 8;
}
