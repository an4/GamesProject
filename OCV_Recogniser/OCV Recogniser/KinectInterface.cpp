#include <opencv2/opencv.hpp>

#include "KinectInterface.h"

KinectInterface::KinectInterface()
{
}


KinectInterface::~KinectInterface()
{
}


bool KinectInterface::initKinect() {
	// Get a working kinect sensor
	int numSensors;
	if (NuiGetSensorCount(&numSensors) < 0 || numSensors < 1) return false;
	if (NuiCreateSensorByIndex(0, &sensor) < 0) return false;

	// Initialize sensor
	sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_COLOR);
	sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, // Depth camera or rgb camera?
		NUI_IMAGE_RESOLUTION_640x480,                // Image resolution
		0,         // Image stream flags, e.g. near mode
		2,        // Number of frames to buffer
		NULL,     // Event handle
		&depthStream);
	return true;
}

bool KinectInterface::getKinectData(/*GLubyte* dest,*/ int *rawdest, uint8_t *scaled_dest) {
	NUI_IMAGE_FRAME imageFrame;
	NUI_LOCKED_RECT LockedRect;
	if (sensor->NuiImageStreamGetNextFrame(depthStream, 0, &imageFrame) < 0) return false;
	INuiFrameTexture* texture = imageFrame.pFrameTexture;
	texture->LockRect(0, &LockedRect, NULL, 0);
	//int dmax, dmin;
	//dmax = dmin = 0;
	if (LockedRect.Pitch != 0) {
		const USHORT* curr = (const USHORT*)LockedRect.pBits;
		const USHORT* dataEnd = curr + (width*height);
		frameCounter = (frameCounter + 1) % 4;
		while (curr < dataEnd) {
			// Get depth in millimeters
			USHORT depth = NuiDepthPixelToDepth(*curr++);
			//dmax = max(dmax, depth);
			//dmin = min(dmin, depth);

			// TODO: Debug visualisation.
			/*
			if (depth < 800) {
				// Show red for out of lower bound pixels.
				*dest++ = (BYTE)0;
				*dest++ = (BYTE)0;
				*dest++ = (BYTE)255;
			}
			else if (depth > 4000) {
				// Show green for out of upper bound pixels.
				*dest++ = (BYTE)0;
				*dest++ = (BYTE)255;
				*dest++ = (BYTE)0;
			}
			else {
				// Greyscale for valid measurements.
				for (int i = 0; i < 3; ++i)
					*dest++ = (BYTE)(((float)(depth - 800) / 3200.0) * 256.0); // Scale to 800 - 4000 range (max distance of sensor... appears valid experimentally
			}
			*dest++ = 0xff;
			*/

			if (rawdest != NULL) {
				*rawdest++ = depth;
			}
			*scaled_dest++ = (uint8_t)(((float)(depth - 800) / 3200.0) * 256.0); // Scale to 800 - 4000 range (max distance of sensor... appears valid experimentally
		}
	}
	//cout << dmax << ' ' << dmin << std::endl;
	texture->UnlockRect(0);
	sensor->NuiImageStreamReleaseFrame(depthStream, &imageFrame);
	return true;
}

// Filter out 0's to the mode of the surrounding pixels
void KinectInterface::filterArray(int *depthArray, int *filteredData)
{
	int widthBound = width - 1;
	int heightBound = height - 1;
	for (int depthArrayRowIndex = 0; depthArrayRowIndex < height; depthArrayRowIndex++)
	{
		for (int depthArrayColumnIndex = 0; depthArrayColumnIndex < width; depthArrayColumnIndex++)
		{
			int depthIndex = depthArrayColumnIndex + (depthArrayRowIndex * width);
			if (depthArray[depthIndex] == 0)
			{
				int x = depthIndex % width;
				int y = (depthIndex - x) / width;
				int filterCollection[24][2];
				for (int i = 0; i < 24; i++)
				{
					filterCollection[i][0] = 0;
					filterCollection[i][1] = 0;
				}

				int innerBandCount = 0;
				int outerBandCount = 0;

				for (int yi = -2; yi < 3; yi++)
				{
					for (int xi = -2; xi < 3; xi++)
					{
						if (xi != 0 || yi != 0)
						{
							int xSearch = x + xi;
							int ySearch = y + yi;

							if (xSearch >= 0 && xSearch <= widthBound && ySearch >= 0 && ySearch <= heightBound)
							{
								int index = xSearch + (ySearch * width);
								if (depthArray[index] != 0)
								{
									for (int i = 0; i < 24; i++)
									{
										if (filterCollection[i][0] == depthArray[index])
										{
											filterCollection[i][1]++;
											break;
										}
										else if (filterCollection[i][0] == 0)
										{
											filterCollection[i][0] = depthArray[index];
											filterCollection[i][1]++;
											break;
										}
									}

									if (yi != -2 && yi != 2 && xi != -2 && xi != 2)
									{
										innerBandCount++;
									}
									else {
										outerBandCount++;
									}
								}
							}
						}
					}
				}
				short depth = 0;
				if (innerBandCount >= 3 || outerBandCount >= 6)
				{
					short frequency = 0;
					for (int i = 0; i < 24; i++)
					{
						if (filterCollection[i][0] == 0)
						{
							break;
						}
						if (filterCollection[i][1] > frequency)
						{
							depth = filterCollection[i][0];
							frequency = filterCollection[i][1];
						}
					}
				}
				filteredData[depthIndex] = depth;
			}
			else {
				filteredData[depthIndex] = depthArray[depthIndex];
			}
		}
	}
}

void KinectInterface::RunOpenCV(std::vector<cv::RotatedRect> &found) {
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
			found.push_back(box); // TODO: emplace_back!
			cv::Point2f vertices[4]; // The mind boggles why OpenCV doesn't have a function to draw it's own shapes...
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
