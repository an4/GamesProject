#include "main.h"
#include "glut.h"

#include <Windows.h>
#include <Ole2.h>

#include <NuiApi.h>
#include <NuiImageCamera.h>
#include <NuiSensor.h>

#include <iostream>
#include <fstream>
#include <iomanip>
#include <ppl.h>

using namespace std;
using namespace concurrency;

// OpenGL Variables
GLuint textureId;
GLubyte data[width*height*4];
int **rawdata;
int rawdata0[width*height];
int rawdata1[width*height];
int rawdata2[width*height];
int rawdata3[width*height];
int filteredData[width*height];
short frameCounter = 0;

// Kinect variables
HANDLE depthStream;
INuiSensor* sensor;

bool initKinect() {
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
    return sensor;
}

void getKinectData(GLubyte* dest, int* rawdest) {
    NUI_IMAGE_FRAME imageFrame;
    NUI_LOCKED_RECT LockedRect;
    if (sensor->NuiImageStreamGetNextFrame(depthStream, 0, &imageFrame) < 0) return;
    INuiFrameTexture* texture = imageFrame.pFrameTexture;
    texture->LockRect(0, &LockedRect, NULL, 0);
	//int dmax, dmin;
	//dmax = dmin = 0;
    if (LockedRect.Pitch != 0) {
        const USHORT* curr = (const USHORT*) LockedRect.pBits;
        const USHORT* dataEnd = curr + (width*height);
		frameCounter = (frameCounter + 1) % 4;
		while (curr < dataEnd) {
			// Get depth in millimeters
			USHORT depth = NuiDepthPixelToDepth(*curr++);
			//dmax = max(dmax, depth);
			//dmin = min(dmin, depth);
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
			*rawdest++ = depth;
		}
    }
	//cout << dmax << ' ' << dmin << std::endl;
    texture->UnlockRect(0);
    sensor->NuiImageStreamReleaseFrame(depthStream, &imageFrame);
}

void findSquares(int* depthArray)
{
	int backgroundMin = depthArray[(width / 2) + (height / 2)*width];
	cout << backgroundMin << "\n";
}

// Filter out 0's to the mode of the surrounding pixels
void filterArray(int* depthArray, int* filteredData, char* file)
{
	ofstream dump(file, ios::trunc);
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
				dump << setw(4) << filteredData[depthIndex];
				dump << " ";
			}
			else {
				filteredData[depthIndex] = depthArray[depthIndex];
				dump << setw(4) << filteredData[depthIndex];
				dump << " ";
			}
		}
		dump << "\n";
	}
}

void smoothFrames(int* smoothedFrames, int** filteredFrames)
{
	for (int rowIndex = 0; rowIndex < height; rowIndex++)
	{
		for (int columnIndex = 0; columnIndex < width; columnIndex++)
		{
			int index = columnIndex + (rowIndex*width);
			smoothedFrames[index] = (filteredFrames[0][index] + filteredFrames[1][index] + filteredFrames[2][index] + filteredFrames[3][index])/4;
		}
	}
}

void dumpData(int** rawdata)
{
	// Grab the next 4 frames to average across
	int **filteredFrames;
	filteredFrames = (int **)calloc(4, sizeof(int *));
	for (int i = 0; i < 4; i++)
	{
		filteredFrames[i] = (int *)calloc(width*height, sizeof(int));
	}
	// Smooth and average
	int *smoothedFrame = (int *)calloc(width*height, sizeof(int));
	//smoothFrames(smoothedFrame, filteredFrames);
	filterArray(rawdata[0], filteredFrames[0], "dump1.txt");
	filterArray(rawdata[1], filteredFrames[1], "dump2.txt");
	filterArray(rawdata[2], filteredFrames[2], "dump3.txt");
	filterArray(rawdata[3], filteredFrames[3], "dump4.txt");
	smoothFrames(smoothedFrame, filteredFrames);
	filterArray(smoothedFrame, smoothedFrame, "dump_s.txt");
	findSquares(smoothedFrame);
	ofstream rawdump("dump.raw", ios::trunc);
	ofstream pgmdump("dump.pgm", ios::trunc);

	if (pgmdump.is_open())
	{
		// Write some headers
		pgmdump << "P2\n"; // PGM
		pgmdump << width << " " << height << "\n"; // dimensions
		pgmdump << "4000\n"; // max pixel value
	}

	if (rawdump.is_open() && pgmdump.is_open())
	{
		// Dump as raw 16 bit word stream
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				int val = ((filteredFrames[0][j + i*width]) / 100) * 1000;
				val = 40000 - val;
				char bytes[2];
				bytes[0] = (val)& 0xFF;
				bytes[1] = (val >> 8) & 0xFF;
				rawdump.write((char*)bytes, 2);
			}
		}
		// Dump in PGM format
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				pgmdump << setw(4) << ((rawdata[0][(j + i*width)]) / 1) * 1;
				pgmdump << " ";
			}
			pgmdump << "\n";
		}
		rawdump.close();
		pgmdump.close();
	}
}

void initiateDump()
{
	dumpData(rawdata);
}

void drawKinectData() {
    glBindTexture(GL_TEXTURE_2D, textureId);
	if (frameCounter == 0)
		getKinectData(data, rawdata0);
	else if (frameCounter == 1)
		getKinectData(data, rawdata1);
	else if (frameCounter == 2)
		getKinectData(data, rawdata2);
	else if (frameCounter == 3)
		getKinectData(data, rawdata3);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*)data);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f);
        glVertex3f(0, 0, 0);
        glTexCoord2f(1.0f, 0.0f);
        glVertex3f(width, 0, 0);
        glTexCoord2f(1.0f, 1.0f);
        glVertex3f(width, height, 0.0f);
        glTexCoord2f(0.0f, 1.0f);
        glVertex3f(0, height, 0.0f);
    glEnd();
}

int main(int argc, char* argv[]) {
    if (!init(argc, argv)) return 1;
    if (!initKinect()) return 1;
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	rawdata = (int **)calloc(4, sizeof(int *));
	rawdata[0] = rawdata0;
	rawdata[1] = rawdata1;
	rawdata[2] = rawdata2;
	rawdata[3] = rawdata3;

    // Initialize textures
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, (GLvoid*) data);
    glBindTexture(GL_TEXTURE_2D, 0);

    // OpenGL setup
    glClearColor(0,0,0,0);
    glClearDepth(1.0f);
    glEnable(GL_TEXTURE_2D);

    // Camera setup
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Main loop
    execute();
    return 0;
}
