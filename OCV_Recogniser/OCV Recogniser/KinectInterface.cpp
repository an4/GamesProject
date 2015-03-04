#include <opencv2/opencv.hpp>

#include "KinectInterface.h"

KinectInterface::KinectInterface()
{
}


KinectInterface::~KinectInterface()
{
}

void KinectInterface::RunOpenCV() {
	std::cout << "OpenCV Version: " << CV_VERSION << std::endl;

	cv::Mat src = cv::imread("boxbroom_painted.png");
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
	std::vector<std::vector<cv::Point>> approxFakeContours;
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

		// Approximate a closed poly from contours
		cv::approxPolyDP(cv::Mat(contoursFound.at(idx)), approxFound, 20, true);

		// Stick this closed poly into the list of them
		approxFakeContours.push_back(std::vector<cv::Point>(approxFound));
	}

	for (size_t idx = 0; idx < approxFakeContours.size(); idx++)
	{
		cv::drawContours(approxImage, approxFakeContours, idx, colors[idx % 3]);
	}

	cv::imshow("test", contourImage);
	cv::waitKey();
	cv::imshow("test", approxImage);
	cv::waitKey();

	// Use the min area bounding rectangle to get us a quick approx that we can use. TODO: This is not ideal in the slightest if our bounding contour is off... we should check them!
	for (size_t idx = 0; idx < approxFakeContours.size(); idx++)
	{
		// Only look at contours with 4 corners
		if (approxFakeContours.at(idx).size() == 4)
		{
			cv::Scalar yellow = cv::Scalar(100, 255, 255);
			cv::RotatedRect box = cv::minAreaRect(approxFakeContours.at(idx));
			cv::Point2f vertices[4]; // The mind boggles why OpenCV doesn't have a function to draw it's shapes...
			box.points(vertices);
			for (int i = 0; i < 4; i++) {
				cv::line(approxImage, vertices[i], vertices[(i + 1) % 4], yellow);
			}
			//cv::rectangle(approxImage, box, yellow);
		}
	}

	cv::imshow("test bbox", approxImage);
	cv::waitKey();
}
