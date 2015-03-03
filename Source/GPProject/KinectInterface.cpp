// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "KinectInterface.h"
#include "AssertionMacros.h" // To give us log access.

//***********************************************************
//Thread Worker Starts as NULL, prior to being instanced
//		This line is essential! Compiler error without it
KinectInterface* KinectInterface::Runnable = NULL;
//***********************************************************

KinectInterface::KinectInterface(AGPPlayerController *pc)
	: ThePC(pc)
	, StopTaskCounter(0)
{
	UE_LOG(LogTemp, Warning, TEXT("CONSTRUCTING KINECT INTERFACE"));
	

	const bool bAutoDeleteSelf = false;
	const bool bAutoDeleteRunnable = false;
	Thread = FRunnableThread::Create(this, TEXT("KinectInterface"), bAutoDeleteSelf, bAutoDeleteRunnable, 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

KinectInterface::~KinectInterface()
{
	delete Thread;
	Thread = NULL;
	//squaresFound.Empty();
}

//Init
bool KinectInterface::Init()
{

	if (ThePC)
	{
		if (!StartTCPReceiver("RamaSocketListener", "127.0.0.1", 8890))
		{
			return false;
		}

		ThePC->ClientMessage("**********************************");
		ThePC->ClientMessage("Listener Started!");
		ThePC->ClientMessage("**********************************");
	}
	return true;
}

bool KinectInterface::StartTCPReceiver(
	const FString& YourChosenSocketName,
	const FString& TheIP,
	const int32 ThePort
	){
	//Rama's CreateTCPConnectionListener
	ListenerSocket = CreateTCPConnectionListener(YourChosenSocketName, TheIP, ThePort);

	//Not created?
	if (!ListenerSocket)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("StartTCPReceiver>> Listen socket could not be created! ~> %s %d"), *TheIP, ThePort));
		return false;
	}

	//Start the Listener! //thread this eventually
	//GetWorldTimerManager().SetTimer(this,
	//	&KinectInterface::TCPConnectionListener, 0.01, true);

	return true;
}

float KinectInterface::GetWidth()
{
	return (float) width;
}

float KinectInterface::GetHeight()
{
	return (float) height;
}

// Refreshes depth array from the Kinect.
void KinectInterface::Rescan()
{
	
}

uint32 KinectInterface::Run()
{
	//Initial wait before starting
	FPlatformProcess::Sleep(0.03);

	UE_LOG(LogTemp, Warning, TEXT("Running Kinect..."));

	//While not told to stop this thread 
	//		and not yet finished finding Prime Numbers
	while (StopTaskCounter.GetValue() == 0)
	{


		FPlatformProcess::Sleep(60.0);
		//Rescan();

		//***************************************
		//Show Incremental Results in Main Game Thread!

		//	Please note you should not create, destroy, or modify UObjects here.
		//	  Do those sort of things after all thread are completed.

		//	  All calcs for making stuff can be done in the threads
		//	     But the actual making/modifying of the UObjects should be done in main game thread.
		//ThePC->ClientMessage(FString::FromInt(PrimeNumbers->Last()));
		//***************************************

		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//prevent thread from using too many resources
		//FPlatformProcess::Sleep(0.01);
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	}

	//Run FPrimeNumberWorker::Shutdown() from the timer in Game Thread that is watching
	//to see when FPrimeNumberWorker::IsThreadFinished()

	return 0;
}

//stop
void KinectInterface::Stop()
{
	StopTaskCounter.Increment();
}

KinectInterface* KinectInterface::JoyInit(AGPPlayerController* IN_PC)
{
	//Create new instance of thread if it does not exist
	//		and the platform supports multi threading!
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new KinectInterface(IN_PC);
	}
	return Runnable;
}


bool KinectInterface::IsThreadFinished()
{
	if (Runnable) return Runnable->IsFinished();
	return true;
}