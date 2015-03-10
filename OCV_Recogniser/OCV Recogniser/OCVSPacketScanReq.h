#pragma once
#include "OCVSPacket.h"

class OCVSPacketScanReq :
	public OCVSPacket
{
public:
	OCVSPacketScanReq();

	~OCVSPacketScanReq();

	void Pack(std::vector<char> &buff) override;

	size_t GetPackedSize() const override;

private:
	const unsigned char content = 0x53;
};

