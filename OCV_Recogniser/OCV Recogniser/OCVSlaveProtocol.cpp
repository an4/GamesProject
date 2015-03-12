#include <cstdio>
#include <iostream>
#include <asio.hpp>

#include "OCVSPacketAck.h"
#include "OCVSPacketChallenge.h"
#include "OCVSPacketScanHeader.h"
#include "OCVSPacketScanChunk.h"

#include "OCVSlaveProtocol.h"

// Use image file input in case a Kinect is not connected (for debugging use!)
#define FIXED_FALLBACK 1

using asio::ip::tcp;

OCVSlaveProtocol::OCVSlaveProtocol(char *host, char *port)
	: host(host)
	, port(port)
	, kinect(new KinectInterface())
{
	// TODO: Do we really want to be doing this here?
	// TODO: Handle init errors
	initSuccess = kinect->initKinect();
	if (initSuccess) {
		uint8_t *imgarr = (uint8_t *)malloc(640 * 480 * sizeof(uint8_t));
		while (!kinect->getKinectData(NULL, imgarr)) { std::cout << '.'; } // TODO: Sleep here to throttle!
		std::cout << std::endl;
		free(imgarr);
	}
}

OCVSlaveProtocol::~OCVSlaveProtocol()
{
}

void OCVSlaveProtocol::Connect()
{
	try
	{
		std::vector<cv::RotatedRect> found;

		if (initSuccess) {
			uint8_t *imgarr = (uint8_t *)calloc(640 * 480, sizeof(uint8_t));
			if (kinect->getKinectData(NULL, imgarr)) {
				cv::Mat test(480, 640, CV_8U, imgarr);
				cv::imshow("src", test);
				cv::waitKey();
				kinect->RunOpenCV(test, found);
			}
		}
		else if (FIXED_FALLBACK) {
			cv::Mat src = cv::imread("boxbroom_painted_2.png");
			cv::imshow("test", src);
			cv::waitKey();
			// Convert to grayscale
			cv::Mat test;
			cv::cvtColor(src, test, cv::COLOR_BGR2GRAY);
			src.release();
			kinect->RunOpenCV(test, found);
		}
		else {
			throw std::exception();
		}


		OCVSPacketChallenge pktChallenge;
		OCVSPacketAck pktAck;

		// Dummy Scan Data
		std::vector<OCVSPacket *> chunks;
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
				std::cout << "Good response, connected." << std::endl;

				len = socket.read_some(asio::buffer(buf), error);
				if (error == asio::error::eof)
					break; // Connection closed cleanly by peer.
				else if (error)
					throw asio::system_error(error); // Some other error.

				// TODO: Proper checking
				// Assume that if we receive a single byte it is a scan req -> so continue.
				if (len != 1) {
					// NEEDS IMPLEMENTATION
					throw asio::system_error(asio::error_code());
				}

				std::cout << "Request received, ACK'ing and responding." << std::endl;

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

				len = socket.read_some(asio::buffer(buf), error);
				if (error == asio::error::eof)
					break; // Connection closed cleanly by peer.
				else if (error)
					throw asio::system_error(error); // Some other error.

				// TODO: Proper checking
				// Assume that if we receive a single by+te it is an ACK -> so continue.
				if (len != 1) {
					// NEEDS IMPLEMENTATION
					throw asio::system_error(asio::error_code());
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