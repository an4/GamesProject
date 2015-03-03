#pragma once
#include "OCVSPacket.h"

class OCVSPacketAck :
	public OCVSPacket
{
public:
	OCVSPacketAck();
	~OCVSPacketAck();

	void Pack(std::vector<char> &buff) override;

	size_t GetPackedSize() const override;

private:
	const char content = (char)0xFF;
};

