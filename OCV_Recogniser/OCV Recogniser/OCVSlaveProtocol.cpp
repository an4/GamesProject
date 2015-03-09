#include <cstdio>
#include <iostream>
#include <asio.hpp>

#include "OCVSPacketAck.h"
#include "OCVSPacketChallenge.h"
#include "OCVSPacketScanHeader.h"
#include "OCVSPacketScanChunk.h"

#include "OCVSlaveProtocol.h"

using asio::ip::tcp;

OCVSlaveProtocol::OCVSlaveProtocol(char *host, char *port)
	: host(host)
	, port(port)
{
}

OCVSlaveProtocol::~OCVSlaveProtocol()
{
}

void OCVSlaveProtocol::Connect()
{
	try
	{
		OCVSPacketChallenge pktChallenge;
		OCVSPacketAck pktAck;

		// Dummy Scan Data
		std::vector<OCVSPacket *> chunks;
		std::vector<cv::RotatedRect> found;
		//kinect.RunOpenCV(found);
		for (size_t i = 0; i < found.size(); i++)
		{
			chunks.push_back(new OCVSPacketScanChunk(i, found.at(i)));
		}

		std::cout << "Found" << found.size() << std::endl;

		OCVSPacketScanHeader pktScanHead(chunks); // TODO: Empty constructor?
		//OCVSPacketScanChunk pktScanChunk(100, cv::RotatedRect(cv::Point2f(50.0, 50.0), cv::Size2f(100.0,50.0), 45.0));

		asio::io_service io_service;
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(host, port);
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		tcp::socket socket(io_service);
		
		asio::connect(socket, endpoint_iterator);

		for (;;)
		{
			std::vector<char> buf(128);
			asio::error_code error;

			std::cout << "Connected, sending challenge packet." << std::endl;

			pktChallenge.Pack(buf);

			socket.write_some(asio::buffer(buf), error);
			if (error == asio::error::eof)
				break; // Connection closed cleanly by peer.
			else if (error)
				throw asio::system_error(error); // Some other error.

			std::cout << "Waiting for response..." << std::endl;

			size_t len = socket.read_some(asio::buffer(buf), error);
			if (error == asio::error::eof)
				break; // Connection closed cleanly by peer.
			else if (error)
				throw asio::system_error(error); // Some other error.

			if (pktChallenge.VerifyReceived(buf)) {
				std::cout << "Good response, ACK'ing." << std::endl;

				pktAck.Pack(buf);
				socket.write_some(asio::buffer(buf), error);
				if (error == asio::error::eof)
					break; // Connection closed cleanly by peer.
				else if (error)
					throw asio::system_error(error); // Some other error.

				pktScanHead.Pack(buf);
				socket.write_some(asio::buffer(buf), error);
				if (error == asio::error::eof)
					break; // Connection closed cleanly by peer.
				else if (error)
					throw asio::system_error(error); // Some other error.

				for (size_t i = 0; i < chunks.size(); i++) {
					chunks.at(i)->Pack(buf);
					socket.write_some(asio::buffer(buf), error);
					if (error == asio::error::eof)
						break; // Connection closed cleanly by peer.
					else if (error)
						throw asio::system_error(error); // Some other error.
				}
			}
			else {
				std::cout << "Bad response, closing." << std::endl;
				socket.shutdown(tcp::socket::shutdown_both);
				socket.close();
			}

			//std::cout.write(buf.data(), len);

		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}