#pragma once
#include "OCVSPacket.h"

class OCVSPacketChallenge :
	public OCVSPacket
{
public:
	OCVSPacketChallenge();

	OCVSPacketChallenge(const std::vector<char> &buff);

	~OCVSPacketChallenge();

	void Pack(std::vector<char> &buff) override;

	size_t GetPackedSize() const override;

private:
	static const size_t PackedSize = 1;
};

