
#include "GPProject.h"
#include "OCVSPacketScanHeader.h"
//#include <cassert>

OCVSPacketScanHeader::OCVSPacketScanHeader(uint32_t length, uint32_t chunk_count)
	: length(length)
	, chunk_count(chunk_count)
{
}


OCVSPacketScanHeader::OCVSPacketScanHeader(const std::vector<OCVSPacket> &scanChunks)
	: chunk_count(scanChunks.size())
{
	length = 0;

	//for each (const auto &pkt in scanChunks)
	//{
	//	length += pkt.GetPackedSize();
	//}
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

	char *asBytes = reinterpret_cast<char *>(&length);

	// Send the total length
	buff.insert(buff.end(), asBytes, asBytes+sizeof(length));

	asBytes = reinterpret_cast<char *>(&chunk_count);

	// Send the chunk count
	buff.insert(buff.end(), asBytes, asBytes + sizeof(chunk_count));

	//assert(buff.size() == GetPackedSize());
}


size_t OCVSPacketScanHeader::GetPackedSize() const
{
	// Fixed length of two 32 bit fields
	return 8;
}


uint32 OCVSPacketScanHeader::GetChunkCount() const
{
	// TODO: Type crying!
	return (uint32)chunk_count;
}
