#pragma once
#include <opencv2/opencv.hpp>
#include <cstdint>

#include "OCVSPacket.h"

class OCVSPacketScanChunk :
	public OCVSPacket
{
public:
	//OCVSPacketScanChunk(uint32_t index, cv::RotatedRect rect);
	OCVSPacketScanChunk(uint32_t index, cv::Point2f *corners);

	~OCVSPacketScanChunk();

	void Pack(std::vector<char> &buff) override;

	size_t GetPackedSize() const override;

private:
	uint32_t index;
	float c1_x;
	float c1_y;
	float c2_x;
	float c2_y;
};
