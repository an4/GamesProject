#pragma once
#include "GPProject.h"
#include "OCVSPacket.h"

class OCVSPacketChallenge :
	public OCVSPacket
{
public:
	OCVSPacketChallenge();

	OCVSPacketChallenge(const std::vector<char> &buff);

	~OCVSPacketChallenge();

	void Pack(std::vector<char> &buff) const override;

	size_t GetPackedSize() const override;

	bool VerifyReceived(const std::vector<char> &buff) const;

private:
	static const size_t PackedSize = 1;
};

