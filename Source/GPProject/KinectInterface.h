// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */
class GPPROJECT_API KinectInterface
{
	static const int width = 16;
	static const int height = 12;
	int depthArray[width*height];
	int hasBeenSearched[width*height];

	TArray<FVector2D> squaresFound;

public:
	KinectInterface();
	~KinectInterface();

	float GetWidth();
	float GetHeight();

	TArray<FVector2D> GetSquares();

	void Rescan();

	void FindSquares();

	void Run();
};
