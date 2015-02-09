#include "main.h"
#include <iostream>
#include <iomanip>
#include <Windows.h>
using namespace std;

int data[width*height];
int hasBeenSearched[width*height];

void findSquares(int* depthArray)
{
	int backgroundMin = data[(width / 2) + (height / 2)*width];
	cout << backgroundMin << "\n";
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
				cout << columnIndex << "," << rowIndex << "    ";
				while (newRowIndex < height && newRowIndex >= 0)
				{
					// Parse along the row for similar values
					while (newColIndex < width && newColIndex >= 0)
					{
						int newIndex = newColIndex + (newRowIndex*width);
						int newVal = depthArray[newIndex];
						if (newVal > lowerBound && newVal < upperBound)
						{
							cout << newColIndex << "," << newRowIndex << " ";
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
				cout << count << "\n";
			}
			hasBeenSearched[index] = 1;
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
		hasBeenSearched[i] = 0;
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
	data[13 + 3 * width] = 100;
	data[14 + 3 * width] = 100;
	data[10 + 4 * width] = 100;
	data[11 + 4 * width] = 100;
	data[12 + 4 * width] = 100;
	data[13 + 4 * width] = 100;
	data[14 + 4 * width] = 100;
	findSquares(data);
	int x;
	cout << "\n";
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			cout << hasBeenSearched[j + i*width] << " ";
		}
		cout << "\n";
	}
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