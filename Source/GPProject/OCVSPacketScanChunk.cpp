//#include <cassert>
#include "GPProject.h"
#include "OCVSPacketScanChunk.h"


//OCVSPacketScanChunk::OCVSPacketScanChunk(uint32_t index, cv::RotatedRect rect)
//	: index(index)
//	, centre_x(rect.center.x)
//	, centre_y(rect.center.y)
//	, rotation(rect.angle)
//	, scale_x(rect.size.width)
//	, scale_y(rect.size.height)
//{
//}

// This is not the way to pass an iterator. TODO: NOPE NOPE NOPE
OCVSPacketScanChunk::OCVSPacketScanChunk(std::vector<char> &begin, int offset)
{
	// Index, 5 floats.
	std::vector<char>::iterator it = begin.begin();

	// skip to this packet
	it += offset;

	// Get index
	uint32_t *theint;
	theint = reinterpret_cast<uint32_t *>(&(*it));
	index = *theint;

	it += sizeof(uint32_t);

	// Get pointer and interpret as float.
	float *thefloat;
	thefloat = reinterpret_cast<float *>(&(*it));
	centre_x = *thefloat;

	it += sizeof(float);

	thefloat = reinterpret_cast<float *>(&(*it));
	centre_y = *thefloat;

	it += sizeof(float);

	thefloat = reinterpret_cast<float *>(&(*it));
	rotation = *thefloat;

	it += sizeof(float);

	thefloat = reinterpret_cast<float *>(&(*it));
	scale_x = *thefloat;

	it += sizeof(float);

	thefloat = reinterpret_cast<float *>(&(*it));
	scale_y = *thefloat;
}

OCVSPacketScanChunk::~OCVSPacketScanChunk()
{
}


void OCVSPacketScanChunk::Pack(std::vector<char> &buff) const
{
	buff.clear();

	// Send the index
	const char *asBytes = reinterpret_cast<const char *>(&index);
	buff.insert(buff.end(), asBytes, asBytes + sizeof(index));

	// Send the centre
	asBytes = reinterpret_cast<const char *>(&centre_x);
	buff.insert(buff.end(), asBytes, asBytes + sizeof(centre_x));
	asBytes = reinterpret_cast<const char *>(&centre_y);
	buff.insert(buff.end(), asBytes, asBytes + sizeof(centre_y));

	// Send rotation
	asBytes = reinterpret_cast<const char *>(&rotation);
	buff.insert(buff.end(), asBytes, asBytes + sizeof(rotation));

	// Send the scale
	asBytes = reinterpret_cast<const char *>(&scale_x);
	buff.insert(buff.end(), asBytes, asBytes + sizeof(scale_x));
	asBytes = reinterpret_cast<const char *>(&scale_y);
	buff.insert(buff.end(), asBytes, asBytes + sizeof(scale_y));

	//assert(buff.size() == GetPackedSize());
}


size_t OCVSPacketScanChunk::GetPackedSize() const
{
	// Fixed length of one 32 bit field, 5 32 bit floats
	return 24;
}

