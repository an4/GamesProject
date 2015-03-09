#pragma once
#include <Windows.h>
#include <Ole2.h>

#include <NuiApi.h>
#include <NuiImageCamera.h>
#include <NuiSensor.h>

#include <vector>
#include <stdint.h>

class KinectInterface
{
public:
	KinectInterface();
	~KinectInterface();

	void RunOpenCV(std::vector<cv::RotatedRect> &found);

	bool initKinect();

	// TODO: Decide what to do wrt debug viewing of Kinect input
	bool getKinectData(/*GLubyte* dest,*/ int *rawdest, uint8_t *scaled_dest);

	void filterArray(int *depthArray, int *filteredData);

	const int width = 640;
	const int height = 480;

private:

	// Kinect variables
	HANDLE depthStream;
	INuiSensor* sensor = NULL;

	short frameCounter = 0;
};

