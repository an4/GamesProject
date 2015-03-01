// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "KinectInterface.h"
#include "AssertionMacros.h" // To give us log access.
#include <opencv2/opencv.hpp> // WARNING: modified types.hpp!!!

KinectInterface::KinectInterface()
{
	UE_LOG(LogTemp, Warning, TEXT("CONSTRUCTING KINECT INTERFACE"));
	// TODO: No more LogTemp!
	// WARNING: We have to build the version string ourselves, as the OpenCV header will use C style (narrow!) strings, and we don't (at least on win64)
	UE_LOG(LogTemp, Warning, TEXT("OpenCV Version %d.%d.%d%s"), CV_VERSION_MAJOR, CV_VERSION_MINOR, CV_VERSION_REVISION, TEXT(CV_VERSION_STATUS));
	squaresFound = TArray<FVector2D>();
}

KinectInterface::~KinectInterface()
{
	squaresFound.Empty();
}

float KinectInterface::GetWidth()
{
	return (float) width;
}

float KinectInterface::GetHeight()
{
	return (float) height;
}

TArray<FVector2D> KinectInterface::GetSquares()
{
	// Return a copy so we don't break things TODO: Iterator? Or?
	return TArray<FVector2D>(squaresFound);
}

// Refreshes depth array from the Kinect.
void KinectInterface::Rescan()
{
	// TODO: Don't bring in from Raw... Package will break!
	UE_LOG(LogTemp, Warning, TEXT("DIR: %s"), *FPaths::Combine(*FPaths::GameDir(), TEXT("Raw"), *FString(TEXT("boxbroom_painted.png"))));

	// Use hardcoded depth map for development.
	cv::Mat src;
	src = cv::imread(  TCHAR_TO_ANSI(*FPaths::Combine(*FPaths::GameDir(), *FString(TEXT("Raw")), *FString(TEXT("boxbroom_painted.png")))) );

	if (src.data)
	{
		/*cv::imshow("test", src);
		cv::waitKey();*/

		// Convert to grayscale
		cv::Mat gray;
		cv::cvtColor(src, gray, cv::COLOR_BGR2GRAY);

		// Free src
		src.release();

		// Convert to binary image using Canny
		cv::Mat bw;
		cv::Canny(gray, bw, 40, 70, 3);

		// Free gray
		gray.release();

		/*cv::imshow("test", bw);
		cv::waitKey();*/

		//cv::Mat contourImg = bw.clone();


		std::vector<std::vector<cv::Point>> contoursFound;
		std::vector<std::vector<cv::Point>> approxFakeContours;
		std::vector<cv::Point> approxFound;
		//cv::OutputArray heirarchy;
		std::vector<cv::Vec4i> heirarchy;

		// Causing segfault - smashing the stack???
		//cv::findContours(bw, contoursFound, heirarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_TC89_KCOS);

		cv::Mat contourImage(bw.size(), CV_8UC3, cv::Scalar(0, 0, 0));
		cv::Mat approxImage(bw.size(), CV_8UC3, cv::Scalar(0, 0, 0));

		bw.release();

		cv::Scalar colors[3];
		colors[0] = cv::Scalar(255, 0, 0);
		colors[1] = cv::Scalar(0, 255, 0);
		colors[2] = cv::Scalar(0, 0, 255);
		for (size_t idx = 0; idx < contoursFound.size(); idx++) {
			//std::cout << contoursFound.at(idx).size() << std::endl;
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

		/*cv::imshow("test", contourImage);
		cv::waitKey();
		cv::imshow("test", approxImage);
		cv::waitKey();*/

		contourImage.release();

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

		approxImage.release();
	}
}

void KinectInterface::FindSquares()
{
	int backgroundMin = depthArray[(width / 2) + (height / 2)*width];
	UE_LOG(LogTemp, Warning, TEXT("Background min: %d"), backgroundMin);
	//cout << backgroundMin << "\n";
	for (int rowIndex = 0; rowIndex < height; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < width; columnIndex++)
		{
			int index = columnIndex + (rowIndex*width);
			int val = depthArray[index];
			if (hasBeenSearched[index] == 0 && val < backgroundMin)
			{
				int upperBound = val + 10;
				int lowerBound = val - 10;
				int count = 0;
				int origIndex = index;
				int newColIndex = columnIndex;
				int newRowIndex = rowIndex;
				int direction = 1;
				//cout << columnIndex << "," << rowIndex << "    ";
				UE_LOG(LogTemp, Warning, TEXT("%d,%d"), columnIndex, rowIndex); // Auto newline!
				while (newRowIndex < height && newRowIndex >= 0)
				{
					// Parse along the row for similar values
					while (newColIndex < width && newColIndex >= 0)
					{
						int newIndex = newColIndex + (newRowIndex*width);
						int newVal = depthArray[newIndex];
						if (newVal > lowerBound && newVal < upperBound)
						{
							//cout << newColIndex << "," << newRowIndex << " ";
							UE_LOG(LogTemp, Warning, TEXT("=> %d,%d"), newColIndex, newRowIndex);
							count++;
							hasBeenSearched[newIndex] = 1;
						}
						else
						{
							if (direction == -1)
							{
								break;
							}
							else
							{
								direction = -1;
								newColIndex = columnIndex;
							}
						}
						newColIndex += direction;
					}
					newRowIndex++;
					newColIndex = columnIndex;
					direction = 1;
				}
				//cout << count << "\n";
			}
			hasBeenSearched[index] = 1;
		}
	}
}

void KinectInterface::Run()
{
	UE_LOG(LogTemp, Warning, TEXT("Running Kinect..."));

	Rescan();
}