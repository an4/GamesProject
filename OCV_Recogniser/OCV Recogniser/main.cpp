#include "main.h"
#include "KinectInterface.h"

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cerr << "Usage: client <host> <port>" << std::endl;
		return 1;
	}

	KinectInterface *k = new KinectInterface();
	int *wut = (int *)calloc(640*480,sizeof(int));
	uint8_t *imgarr = (uint8_t *)calloc(640 * 480, sizeof(uint8_t));
	if (!k->initKinect()) {
		return 1;
	}

	// Loop until we get some kinect data... it takes some time to come up.
	bool haveImg = false;
	while (!k->getKinectData(wut, imgarr)) { std::cout << '.'; } // TODO: Sleep here to throttle!

	cv::Mat test(480, 640, CV_8U, imgarr);
	cv::imshow("src", test);
	cv::waitKey();
	
	k->getKinectData(wut, imgarr);
	test.release(); // Shouldn't delete imgarr

	cv::Mat test2(480, 640, CV_8U, imgarr);
	cv::imshow("src", test2);
	cv::waitKey();

	std::vector<cv::RotatedRect> rect;
	k->RunOpenCV(test2, rect);

	//OCVSlaveProtocol client(argv[1], argv[2]);
	//client.Connect();

	return 0;
}
