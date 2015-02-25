#include "main.h"
#include <iostream>
#include <iomanip>
#include <Windows.h>
using namespace std;

int data[width*height];

struct coord {
	int x;
	int y;
};

coord lookLeft(int* depthArray, int rowIndex, int colIndex, int upperBound, int lowerBound, int backgroundMin)
{
	int index = colIndex + rowIndex*width;
	int val = depthArray[index];
	// Go left until no longer in square
	while (val < upperBound && val > lowerBound)
	{
		depthArray[index] = backgroundMin;
		colIndex--;
		// Check we're not out of bounds before grabbing new value
		if (colIndex < 0)
		{
			break;
		}
		index--;
		val = depthArray[index];
	}
	coord result;
	result.x = colIndex + 1;
	result.y = rowIndex;
	return result;
}

coord lookRight(int* depthArray, int rowIndex, int colIndex, int upperBound, int lowerBound, int backgroundMin)
{
	// Check adding 1 to the colIndex before passing hasn't put us out of bounds
	if (colIndex >= width)
	{
		coord result;
		result.x = colIndex - 1;
		result.y = rowIndex;
		return result;
	}
	int index = colIndex + rowIndex*width;
	int val = depthArray[index];
	// Go right until no longer in the square
	while (val < upperBound && val > lowerBound)
	{
		depthArray[index] = backgroundMin;
		colIndex++;
		// Check we're not out of bounds before grabbing the new value
		if (colIndex >= width)
		{
			break;
		}
		index++;
		val = depthArray[index];
	}
	coord result;
	result.x = colIndex - 1;
	result.y = rowIndex;
	return result;
}

void findSquares(int* depthArray)
{
	// Assume central value is part of background, add a minimum square height to it (should use a kernel to filter a small area around center)
	int backgroundMin = data[(width / 2) + (height / 2)*width]-25;
	cout << backgroundMin << "\n\n";
	for (int rowIndex = 0; rowIndex < height; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < width; columnIndex++)
		{
			
			int index = columnIndex + (rowIndex*width);
			int val = depthArray[index];
			if (val < backgroundMin)
			{
				// Setup variables for new square, set bounds on height so that we don't include neighbouring squares or sides
				int upperBound = val + 10;
				int lowerBound = val - 10;
				int newColIndex = columnIndex;
				int newRowIndex = rowIndex;
				coord topmost;
				coord leftmost;
				coord rightmost;
				coord bottom;
				topmost.x = columnIndex;
				topmost.y = rowIndex;
				leftmost.x = columnIndex;
				leftmost.y = rowIndex;
				rightmost.x = columnIndex;
				rightmost.y = rowIndex;
				bottom.x = columnIndex;
				bottom.y = rowIndex;
				while (val > lowerBound && val < upperBound)
				{
					// Get left and rightmost coordinates in current row, and set row values above threshold for background
					coord left = lookLeft(depthArray, newRowIndex, newColIndex, upperBound, lowerBound, backgroundMin+5);
					coord right = lookRight(depthArray, newRowIndex, newColIndex+1, upperBound, lowerBound, backgroundMin+5);
					// Check if it's a new total left/rightmost
					if (left.x <= leftmost.x)
					{
						leftmost = left;
					}
					if (right.x > rightmost.x)
					{
						rightmost = right;
					}
					// Make the column index for the next row the center of the current row to ensure we're still in the square when moving down
					newColIndex = (left.x + right.x + 1) / 2;
					bottom.x = newColIndex;
					bottom.y = newRowIndex;
					newRowIndex++;
					if (newRowIndex >= height)
					{
						break;
					}
					index = newColIndex + (newRowIndex*width);
					val = depthArray[index];
				}
				cout << "Left: " << leftmost.x << "," << leftmost.y << "\n";
				cout << "Top: " << topmost.x << "," << topmost.y << "\n";
				cout << "Right: " << rightmost.x << "," << rightmost.y << "\n";
				cout << "Bottom: " << bottom.x << "," << bottom.y << "\n";
				cout << "\n";
			}
		}
	}
}

int main(int argc, char* argv[]) {
	AllocConsole();
	FILE *stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);
	for (int i = 0; i < width*height; i++)
	{
		data[i] = 200;
	}
	data[2 + 2 * width] = 100;
	data[2 + 3 * width] = 100;
	data[3 + 2 * width] = 100;
	data[3 + 3 * width] = 100;
	data[5 + 4 * width] = 100;
	data[6 + 4 * width] = 100;
	data[7 + 4 * width] = 100;
	data[6 + 5 * width] = 100;
	data[6 + 3 * width] = 100;
	data[8 + 9 * width] = 100;
	data[8 + 10 * width] = 100;
	data[8 + 11 * width] = 100;
	data[9 + 9 * width] = 100;
	data[9 + 10 * width] = 100;
	data[9 + 11 * width] = 100;
	data[10 + 3 * width] = 100;
	data[11 + 3 * width] = 100;
	data[12 + 3 * width] = 100;
	data[12 + 5 * width] = 100;
	data[11 + 4 * width] = 100;
	data[12 + 4 * width] = 100;
	data[13 + 4 * width] = 100;
	data[13 + 5 * width] = 100;
	findSquares(data);
	int x;
	cout << "\n";
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			cout << data[j + i*width] << " ";
		}
		cout << "\n";
	}
	cin >> x;
}