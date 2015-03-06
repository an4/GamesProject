#pragma once
#include "GPProject.h"
#include "OCVSPacket.h"
#include <stdint.h>

class OCVSPacketScanHeader :
	public OCVSPacket
{
public:
	OCVSPacketScanHeader(uint32_t length, uint32_t chunk_count);

	OCVSPacketScanHeader(const std::vector<OCVSPacket> &scanChunks);

	OCVSPacketScanHeader(const std::vector<char> &packet);

	~OCVSPacketScanHeader();

	void Pack(std::vector<char> &buff) override;

	size_t GetPackedSize() const override;

	// _t versions... polluting UE or not?
	uint32 GetChunkCount() const;

private:
	uint32_t length;
	uint32_t chunk_count;
};

