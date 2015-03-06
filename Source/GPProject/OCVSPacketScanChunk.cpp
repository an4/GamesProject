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

//OCVSPacketScanChunk::OCVSPacketScanChunk(uint32_t index, cv::Point2f *corners)
//	: index(index)
//	, c1_x(corners[0].x)
//	, c1_y(corners[0].y)
//	, c2_x(corners[2].x)
//	, c2_y(corners[2].y)
//{
//}

// This is not the way to pass an iterator. TODO: NOPE NOPE NOPE
OCVSPacketScanChunk::OCVSPacketScanChunk(std::vector<char> &begin, int offset)
{
	// Index, 5 floats.

	// SKIP THE DAMN INDEX

	//int i;


	// Grab floats.
	//char *data = NULL;

	std::vector<char>::iterator it = begin.begin();

	// skip
	it += offset + sizeof(uint32_t);



	// Get pointer and interpret as float.
	float *thefloat;
	thefloat = reinterpret_cast<float *>(&(*it));
	c1_x = *thefloat;

	it += sizeof(float);

	thefloat = reinterpret_cast<float *>(&(*it));
	c1_y = *thefloat;

	it += sizeof(float);

	thefloat = reinterpret_cast<float *>(&(*it));
	c2_x = *thefloat;

	it += sizeof(float);

	thefloat = reinterpret_cast<float *>(&(*it));
	c2_y = *thefloat;
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

	//assert(buff.size() == GetPackedSize());
}


size_t OCVSPacketScanChunk::GetPackedSize() const
{
	// Fixed length of one 32 bit field, 5 32 bit floats
	return 20;
}

