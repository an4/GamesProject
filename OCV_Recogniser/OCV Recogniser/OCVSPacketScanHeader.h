#pragma once
#include "OCVSPacket.h"
#include <stdint.h>

class OCVSPacketScanHeader :
	public OCVSPacket
{
public:
	OCVSPacketScanHeader(uint8_t result);

	OCVSPacketScanHeader(uint32_t length, uint32_t chunk_count);

	OCVSPacketScanHeader(const std::vector<OCVSPacket *> &scanChunks);

	~OCVSPacketScanHeader();

	void Pack(std::vector<char> &buff) override;

	size_t GetPackedSize() const override;

	// TODO: An enum would be nice here... but conversion to/from is annoying
	static const uint8_t RESULT_SUCCESS = 0x00;
	static const uint8_t RESULT_FAILURE_UNKNOWN = 0xFF;
private:
	uint8_t result;
	uint32_t length;
	uint32_t chunk_count;
};

