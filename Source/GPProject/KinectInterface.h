// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "GPPlayerController.h"

/**
 * 
 */
class GPPROJECT_API KinectInterface : public FRunnable
{
	/** Singleton instance, can access the thread any time via static accessor, if it is active! */
	static  KinectInterface* Runnable;

	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread;

	/** The Data Ptr */
	TArray<uint32>* PrimeNumbers;

	/** The PC */
	AGPPlayerController* ThePC;

	/** Stop this thread? Uses Thread Safe Counter */
	FThreadSafeCounter StopTaskCounter;

	static const int width = 16;
	static const int height = 12;
	int depthArray[width*height];
	int hasBeenSearched[width*height];

	TArray<FVector2D> squaresFound;

public:
	KinectInterface(AGPPlayerController *pc);
	~KinectInterface();

	bool IsFinished() const
	{
		return false;
	}

	// Begin FRunnable interface.
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	// End FRunnable interface

	/** Makes sure this thread has stopped properly */
	void EnsureCompletion();

	/*
	Start the thread and the worker from static (easy access)!
	This code ensures only 1 Prime Number thread will be able to run at a time.
	This function returns a handle to the newly started instance.
	*/
	static KinectInterface* JoyInit(AGPPlayerController* IN_PC);

	/** Shuts down the thread. Static so it can easily be called from outside the thread context */
	static void Shutdown();

	static bool IsThreadFinished();

	static float GetWidth();
	static float GetHeight();

	TArray<FVector2D> GetSquares();

	void Rescan();

	void FindSquares();

	//void Run();
};
