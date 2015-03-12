
#include "GPProject.h"
#include "OCVSPacketScanReq.h"

OCVSPacketScanReq::OCVSPacketScanReq()
{
}


OCVSPacketScanReq::~OCVSPacketScanReq()
{
}


void OCVSPacketScanReq::Pack(std::vector<char> &buff) const
{
	// Simply a single byte, 0x53
	buff.clear();
	buff.push_back(content);
}


size_t OCVSPacketScanReq::GetPackedSize() const
{
	// Fixed length of a single byte
	return 1;
}
