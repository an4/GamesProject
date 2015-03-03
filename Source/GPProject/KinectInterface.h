// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include <Runtime/Networking/Public/Networking.h>
#include <Runtime/Sockets/Public/Sockets.h>
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

	/** The PC */
	AGPPlayerController* ThePC;

	/** Stop this thread? Uses Thread Safe Counter */
	FThreadSafeCounter StopTaskCounter;

	static const int width = 640;
	static const int height = 480;

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

	void Rescan();

	FSocket* ListenerSocket;
	FSocket* ConnectionSocket;
	FIPv4Endpoint RemoteAddressForConnection;

	bool StartTCPReceiver(
		const FString& YourChosenSocketName,
		const FString& TheIP,
		const int32 ThePort
		);

	FSocket* CreateTCPConnectionListener(
		const FString& YourChosenSocketName,
		const FString& TheIP,
		const int32 ThePort,
		const int32 ReceiveBufferSize = 2 * 1024 * 1024
		);

	//Timer functions, could be threads
	void TCPConnectionListener(); 	//can thread this eventually
	void TCPSocketListener();		//can thread this eventually


	//Format String IP4 to number array
	bool FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4]);

	//Rama's StringFromBinaryArray
	FString StringFromBinaryArray(const TArray<uint8>& BinaryArray);
};
