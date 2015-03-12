#pragma once
#include "GPProject.h"
#include "OCVSPacket.h"

class OCVSPacketScanReq :
	public OCVSPacket
{
public:
	OCVSPacketScanReq();

	~OCVSPacketScanReq();

	void Pack(std::vector<char> &buff) const override;

	size_t GetPackedSize() const override;

private:
	const unsigned char content = 0x53;
};

