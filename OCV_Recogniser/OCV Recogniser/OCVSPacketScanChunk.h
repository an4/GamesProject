#pragma once
#include <opencv2/opencv.hpp>
#include <cstdint>

#include "OCVSPacket.h"

class OCVSPacketScanChunk :
	public OCVSPacket
{
public:
	OCVSPacketScanChunk(uint32_t index, cv::RotatedRect rect);

	~OCVSPacketScanChunk();

	void Pack(std::vector<char> &buff) override;

	size_t GetPackedSize() const override;

private:
	uint32_t index;
	float centre_x;
	float centre_y;
	float rotation;
	float scale_x;
	float scale_y;
};
