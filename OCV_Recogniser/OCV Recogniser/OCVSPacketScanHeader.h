#pragma once
#include "OCVSPacket.h"
#include <stdint.h>

class OCVSPacketScanHeader :
	public OCVSPacket
{
public:
	OCVSPacketScanHeader(uint32_t length, uint32_t chunk_count);

	OCVSPacketScanHeader(const std::vector<OCVSPacket> &scanChunks);

	~OCVSPacketScanHeader();

	void Pack(std::vector<char> &buff) override;

	size_t GetPackedSize() const override;

private:
	uint32_t length;
	uint32_t chunk_count;
};

