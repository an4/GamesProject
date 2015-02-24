// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "KinectInterface.h"
#include "AssertionMacros.h" // To give us log access.

KinectInterface::KinectInterface()
{
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
	// Use hardcoded depth values for development.
	for (int i = 0; i < width*height; i++)
	{
		depthArray[i] = 200;
		hasBeenSearched[i] = 0;
	}
	depthArray[2 + 2 * width] = 100;
	depthArray[2 + 3 * width] = 100;
	depthArray[3 + 2 * width] = 100;
	depthArray[3 + 3 * width] = 100;
	depthArray[5 + 4 * width] = 100;
	depthArray[6 + 4 * width] = 100;
	depthArray[7 + 4 * width] = 100;
	depthArray[6 + 5 * width] = 100;
	depthArray[6 + 3 * width] = 100;
	depthArray[8 + 9 * width] = 100;
	depthArray[8 + 10 * width] = 100;
	depthArray[8 + 11 * width] = 100;
	depthArray[9 + 9 * width] = 100;
	depthArray[9 + 10 * width] = 100;
	depthArray[9 + 11 * width] = 100;
	depthArray[10 + 3 * width] = 100;
	depthArray[11 + 3 * width] = 100;
	depthArray[13 + 3 * width] = 100;
	depthArray[14 + 3 * width] = 100;
	depthArray[10 + 4 * width] = 100;
	depthArray[11 + 4 * width] = 100;
	depthArray[12 + 4 * width] = 100;
	depthArray[13 + 4 * width] = 100;
	depthArray[14 + 4 * width] = 100;
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
	//AllocConsole();
	//FILE *stream;
	//freopen_s(&stream, "CONOUT$", "w", stdout);
	Rescan();
	FindSquares();
	//int x;
	//cout << "\n";
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			//cout << hasBeenSearched[j + i*width] << " ";
		}
		//cout << "\n";
	}
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			//cout << depthArray[j + i*width] << " ";
		}
		//cout << "\n";
	}
	//cin >> x;
}