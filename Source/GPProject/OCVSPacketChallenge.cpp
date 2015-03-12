
#include "GPProject.h"
#include "OCVSPacketChallenge.h"
#include <stdexcept>

OCVSPacketChallenge::OCVSPacketChallenge()
{
}


OCVSPacketChallenge::~OCVSPacketChallenge()
{
}


void OCVSPacketChallenge::Pack(std::vector<char> &buff) const
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


bool OCVSPacketChallenge::VerifyReceived(const std::vector<char> &buff) const
{
	if (buff.size() != PackedSize) {
		return false;
	}

	return buff.at(0) == ProtocolVersion;
}
