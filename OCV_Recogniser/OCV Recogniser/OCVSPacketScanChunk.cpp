#include <cassert>

#include "OCVSPacketScanChunk.h"

//
//OCVSPacketScanChunk::OCVSPacketScanChunk(uint32_t index, cv::RotatedRect rect)
//	: index(index)
//	, c1_x(rect.center.x)
//	, c1_y(rect.center.y)
//	, c2_x(rect.angle)
//	, c2_y(rect.size.width)
//{
//	rect.points
//}

OCVSPacketScanChunk::OCVSPacketScanChunk(uint32_t index, cv::Point2f *corners)
	: index(index)
	, c1_x(corners[0].x)
	, c1_y(corners[0].y)
	, c2_x(corners[2].x)
	, c2_y(corners[2].y)
{
}

OCVSPacketScanChunk::~OCVSPacketScanChunk()
{
}


void OCVSPacketScanChunk::Pack(std::vector<char> &buff)
{
	buff.clear();

	// Send the index
	char *asBytes = reinterpret_cast<char *>(&index);
	buff.insert(buff.end(), asBytes, asBytes + sizeof(index));

	// Send corner 1
	asBytes = reinterpret_cast<char *>(&c1_x);
	buff.insert(buff.end(), asBytes, asBytes + sizeof(c1_x));
	asBytes = reinterpret_cast<char *>(&c1_y);
	buff.insert(buff.end(), asBytes, asBytes + sizeof(c1_y));

	// Send corner 2
	asBytes = reinterpret_cast<char *>(&c2_x);
	buff.insert(buff.end(), asBytes, asBytes + sizeof(c2_x));
	asBytes = reinterpret_cast<char *>(&c2_y);
	buff.insert(buff.end(), asBytes, asBytes + sizeof(c2_y));

	assert(buff.size() == GetPackedSize());
}


size_t OCVSPacketScanChunk::GetPackedSize() const
{
	// Fixed length of one 32 bit field, 4 32 bit floats
	return 20;
}
