#include "GPProject.h"
#include "OCVSPacketAck.h"

OCVSPacketAck::OCVSPacketAck()
{
}


OCVSPacketAck::~OCVSPacketAck()
{
}


void OCVSPacketAck::Pack(std::vector<char> &buff)
{
	// Simply a single byte, 0xFF
	buff.clear();
	buff.push_back(content);
}


size_t OCVSPacketAck::GetPackedSize() const
{
	// Fixed length of a single byte
	return 1;
}
