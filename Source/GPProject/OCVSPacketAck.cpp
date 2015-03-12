#include "GPProject.h"
#include "OCVSPacketAck.h"

const OCVSPacketAck *OCVSPacketAck::static_instance = NULL;

OCVSPacketAck::OCVSPacketAck()
{
}


OCVSPacketAck::~OCVSPacketAck()
{
}


void OCVSPacketAck::Pack(std::vector<char> &buff) const
{
	// Simply a single byte, 0xFF
	buff.clear();
	buff.push_back(content);
}


size_t OCVSPacketAck::GetPackedSize() const
{
	// Fixed length of a single byte
	return 1;
}

// TODO: Maybe this whole class should become a singleton.
const OCVSPacketAck *OCVSPacketAck::getInstance()
{
	if (static_instance == NULL) {
		static_instance = new OCVSPacketAck();
	}
	return static_instance;
}
