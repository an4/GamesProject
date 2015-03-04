#include <opencv2/opencv.hpp>
#include <vector>
#include "OCVSPacket.h"

class OCVSlaveProtocol
{
public:
	OCVSlaveProtocol(char *host, char *port);
	~OCVSlaveProtocol();

	void Connect();

private:
	const char * const host;
	const char * const port;

	std::vector<char> recvBuff;
	std::vector<OCVSPacket> pktSendBuff;
};