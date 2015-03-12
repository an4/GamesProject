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

	void Pack(std::vector<char> &buff) const override;

	size_t GetPackedSize() const override;

	// TODO: Getters 
	float centre_x;
	float centre_y;
	float rotation;
	float scale_x;
	float scale_y;

private:
	uint32_t index;
};
