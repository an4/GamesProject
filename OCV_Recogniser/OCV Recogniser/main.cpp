#include "main.h"

void main()
{
	std::cout << "OpenCV Version: " << CV_VERSION << std::endl;

	cv::Mat src = cv::imread("boxbroom_simple_painted.png");
	cv::imshow("test", src);
	cv::waitKey();

	// Convert to grayscale
	cv::Mat gray;
	cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

	// Convert to binary image using Canny
	cv::Mat bw;
	cv::Canny(gray, bw, 40, 70, 3);
	cv::imshow("test", bw);
	cv::waitKey();

	cv::Mat contourImg = bw.clone();
	std::vector<std::vector<cv::Point>> contoursFound;
	std::vector<cv::Point> approxFound;
	//cv::OutputArray heirarchy;
	std::vector<cv::Vec4i> heirarchy;

	cv::findContours(contourImg, contoursFound, heirarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_TC89_KCOS);

	cv::Mat contourImage(src.size(), CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat approxImage(src.size(), CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Scalar colors[3];
	colors[0] = cv::Scalar(255, 0, 0);
	colors[1] = cv::Scalar(0, 255, 0);
	colors[2] = cv::Scalar(0, 0, 255);
	for (size_t idx = 0; idx < contoursFound.size(); idx++) {
		std::cout << contoursFound.at(idx).size() << std::endl;
		cv::drawContours(contourImage, contoursFound, idx, colors[idx % 3]);
		cv::approxPolyDP(cv::Mat(contoursFound.at(idx)), approxFound, 20, true);
	}

	std::vector<std::vector<cv::Point>> approxFakeContours;
	approxFakeContours.push_back(approxFound);
	cv::drawContours(approxImage, approxFakeContours, 0, colors[1]);

	cv::imshow("test", contourImage);
	cv::waitKey();
	cv::imshow("test", approxImage);
	cv::waitKey();

}