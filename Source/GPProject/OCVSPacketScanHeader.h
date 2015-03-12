#pragma once

#ifdef IN_UE4
#include "GPProject.h"
#endif

#include "OCVSPacket.h"
#include <stdint.h>

class OCVSPacketScanHeader :
	public OCVSPacket
{
public:
	OCVSPacketScanHeader(uint8_t result);

	OCVSPacketScanHeader(uint32_t length, uint32_t chunk_count);

	OCVSPacketScanHeader(const std::vector<OCVSPacket *> &scanChunks);

	OCVSPacketScanHeader(const std::vector<char> &packet);

	~OCVSPacketScanHeader();

	void Pack(std::vector<char> &buff) const override;

	size_t GetPackedSize() const override;

	// _t versions... polluting UE or not?
	uint32_t GetChunkCount() const;

	// TODO: An enum would be nice here... but conversion to/from is annoying
	static const uint8_t RESULT_SUCCESS = 0x00;
	static const uint8_t RESULT_FAILURE_UNKNOWN = 0xFF;
private:
	uint8_t result;
	uint32_t length;
	uint32_t chunk_count;
};

