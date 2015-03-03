#include "OCVSPacketChallenge.h"
#include <stdexcept>


OCVSPacketChallenge::OCVSPacketChallenge()
{
}


OCVSPacketChallenge::~OCVSPacketChallenge()
{
}


void OCVSPacketChallenge::Pack(std::vector<char> &buff)
{
	buff.clear();
	// Send the protcol version
	buff.push_back(ProtocolVersion);
}


size_t OCVSPacketChallenge::GetPackedSize() const
{
	// Fixed length of a single byte followed by a 32 bit int
	return PackedSize;
}
