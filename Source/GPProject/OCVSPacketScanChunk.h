#pragma once
#include "GPProject.h"
//#include <opencv2/opencv.hpp>
#include <cstdint>
#include <vector>

#include "OCVSPacket.h"

class OCVSPacketScanChunk :
	public OCVSPacket
{
public:
	//OCVSPacketScanChunk(uint32_t index, cv::RotatedRect rect);

	OCVSPacketScanChunk(std::vector<char> &begin, int offset);

	~OCVSPacketScanChunk();

	void Pack(std::vector<char> &buff) override;

	size_t GetPackedSize() const override;

	// TODO: Getters
	float c1_x;
	float c1_y;
	float c2_x;
	float c2_y;

private:
	uint32_t index;
};
