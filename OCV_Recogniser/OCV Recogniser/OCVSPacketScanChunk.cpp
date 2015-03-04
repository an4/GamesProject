#include <cassert>

#include "OCVSPacketScanChunk.h"


OCVSPacketScanChunk::OCVSPacketScanChunk(uint32_t index, cv::RotatedRect rect)
	: index(index)
	, centre_x(rect.center.x)
	, centre_y(rect.center.y)
	, rotation(rect.angle)
	, scale_x(rect.size.width)
	, scale_y(rect.size.height)
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

	// Send the centre
	asBytes = reinterpret_cast<char *>(&centre_x);
	buff.insert(buff.end(), asBytes, asBytes + sizeof(centre_x));
	asBytes = reinterpret_cast<char *>(&centre_y);
	buff.insert(buff.end(), asBytes, asBytes + sizeof(centre_y));

	// Send rotation
	asBytes = reinterpret_cast<char *>(&rotation);
	buff.insert(buff.end(), asBytes, asBytes + sizeof(rotation));

	// Send the scale
	asBytes = reinterpret_cast<char *>(&scale_x);
	buff.insert(buff.end(), asBytes, asBytes + sizeof(scale_x));
	asBytes = reinterpret_cast<char *>(&scale_y);
	buff.insert(buff.end(), asBytes, asBytes + sizeof(scale_y));

	assert(buff.size() == GetPackedSize());
}


size_t OCVSPacketScanChunk::GetPackedSize() const
{
	// Fixed length of one 32 bit field, 5 32 bit floats
	return 24;
}
