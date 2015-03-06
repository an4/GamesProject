// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "KinectInterface.h"
#include "AssertionMacros.h" // To give us log access.
#include <string>

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

	UE_LOG(LogTemp, Warning, TEXT("INIT"));
	if (ThePC)
	{
		UE_LOG(LogTemp, Warning, TEXT("YES PC"));
		if (!StartTCPReceiver("RamaSocketListener", "127.0.0.1", 25599))
		{
			UE_LOG(LogTemp, Warning, TEXT("LISTEN FAIL"));
			return false;
		}

		UE_LOG(LogTemp, Warning, TEXT("LISTEN STARTED"));
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
	TCPConnectionListener(); // TODO: BAD?!?
	//GetWorldTimerManager().SetTimer(this,
	//	&KinectInterface::TCPConnectionListener, 0.01, true);

	return true;
}

//Format IP String as Number Parts
bool KinectInterface::FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4])
{
	//IP Formatting
	TheIP.Replace(TEXT(" "), TEXT(""));

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//						   IP 4 Parts

	//String Parts
	TArray<FString> Parts;
	TheIP.ParseIntoArray(&Parts, TEXT("."), true);
	if (Parts.Num() != 4)
		return false;

	//String to Number Parts
	for (int32 i = 0; i < 4; ++i)
	{
		Out[i] = FCString::Atoi(*Parts[i]);
	}

	return true;
}
//Rama's Create TCP Connection Listener
FSocket* KinectInterface::CreateTCPConnectionListener(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort, const int32 ReceiveBufferSize)
{
	uint8 IP4Nums[4];
	if (!FormatIP4ToNumber(TheIP, IP4Nums))
	{
		//VShow("Invalid IP! Expecting 4 parts separated by .");
		UE_LOG(LogTemp, Warning, TEXT("Invalid IP! Expecting 4 parts separated by ."));
		return false;
	}

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	//Create Socket
	FIPv4Endpoint Endpoint(FIPv4Address(IP4Nums[0], IP4Nums[1], IP4Nums[2], IP4Nums[3]), ThePort);
	FSocket* ListenSocket = FTcpSocketBuilder(*YourChosenSocketName)
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.Listening(8);

	//Set Buffer Size
	int32 NewSize = 0;
	ListenSocket->SetReceiveBufferSize(ReceiveBufferSize, NewSize);

	//Done!
	return ListenSocket;
}
//Rama's TCP Connection Listener
void KinectInterface::TCPConnectionListener()
{
	//~~~~~~~~~~~~~
	if (!ListenerSocket) return;
	//~~~~~~~~~~~~~

	//Remote address
	TSharedRef<FInternetAddr> RemoteAddress = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	bool Pending;

	// handle incoming connections
	if (ListenerSocket->HasPendingConnection(Pending) && Pending)
	{
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		//Already have a Connection? destroy previous
		if (ConnectionSocket)
		{
			ConnectionSocket->Close();
			ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ConnectionSocket);
		}
		//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

		//New Connection receive!
		ConnectionSocket = ListenerSocket->Accept(*RemoteAddress, TEXT("RamaTCP Received Socket Connection"));

		if (ConnectionSocket != NULL)
		{
			//Global cache of current Remote Address
			RemoteAddressForConnection = FIPv4Endpoint(RemoteAddress);

			//UE_LOG "Accepted Connection! WOOOHOOOO!!!";
			TCPSocketListener(); // TODO: THIS IS PROBABLY VERY BAD
			//can thread this too
			//GetWorldTimerManager().SetTimer(this,
			//	&AYourClass::TCPSocketListener, 0.01, true);
		}
	}
}

//Rama's String From Binary Array
//This function requires 
//		#include <string>
FString KinectInterface::StringFromBinaryArray(const TArray<uint8>& BinaryArray)
{
	//Create a string from a byte array!
	std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());
	return FString(cstr.c_str());
}

//Rama's TCP Socket Listener
void KinectInterface::TCPSocketListener()
{
	//~~~~~~~~~~~~~
	if (!ConnectionSocket) return;
	//~~~~~~~~~~~~~


	//Binary Array!
	TArray<uint8> ReceivedData;

	uint32 Size;
	while (ConnectionSocket->HasPendingData(Size))
	{
		ReceivedData.Init(FMath::Min(Size, 65507u));

		int32 Read = 0;
		ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);

		UE_LOG(LogTemp, Warning, TEXT("Data read! %d"), ReceivedData.Num());
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Data Read! %d"), ReceivedData.Num()));
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (ReceivedData.Num() <= 0)
	{
		//No Data Received
		return;
	}

	//VShow("Total Data read!", ReceivedData.Num());
	UE_LOG(LogTemp, Warning, TEXT("Total Data read!"), ReceivedData.Num());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Data Bytes Read ~> %d"), ReceivedData.Num()));


	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//						Rama's String From Binary Array
	const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	//VShow("As String!!!!! ~>", ReceivedUE4String);
	UE_LOG(LogTemp, Warning, TEXT("As String!!!!! ~> %s"), *ReceivedUE4String);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("As String Data ~> %s"), *ReceivedUE4String));
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


		FPlatformProcess::Sleep(0.5);
		Init();
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