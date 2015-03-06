// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPPlayerController.h"
#include "GPCharacter.h"
#include "KinectInterface.h"
#include <string>

#include "OCVSPacketAck.h"
#include "OCVSPacketChallenge.h"
#include "OCVSPacketScanHeader.h"
#include "OCVSPacketScanChunk.h"

// The following code is taken from the replication wiki. Details how to update a boolean property on the server from a client.

void AGPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// set up gameplay key bindings
	InputComponent->BindAxis("MoveForward", this, &AGPPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AGPPlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &AGPPlayerController::AddControllerYawInput);
	InputComponent->BindAxis("LookUp", this, &AGPPlayerController::AddControllerPitchInput);

	InputComponent->BindAction("Jump", IE_Pressed, this, &AGPPlayerController::OnStartJump);
	InputComponent->BindAction("Jump", IE_Released, this, &AGPPlayerController::OnStopJump);

	InputComponent->BindAction("Fire", IE_Pressed, this, &AGPPlayerController::OnFire);

	InputComponent->BindAction("RemoteBombPlant", IE_Pressed, this, &AGPPlayerController::OnBombLaunch);
	InputComponent->BindAction("RemoteBombDetonate", IE_Pressed, this, &AGPPlayerController::OnBombDetonate);
}

void AGPPlayerController::MoveForward(float Value)
{
	// TODO: Health test - forward disabled when health gone
    if (GetCharacter() != NULL && (Value != 0.0f) /*&& (Health > 0.0f)*/)
    {
        // find out which way is forward
        FRotator Rotation = GetControlRotation();
        // Limit pitch when walking or falling
		if (GetCharacter()->GetCharacterMovement()->IsMovingOnGround() || GetCharacter()->GetCharacterMovement()->IsFalling())
        {
            Rotation.Pitch = 0.0f;
        }
        // add movement in that direction
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
        GetCharacter()->AddMovementInput(Direction, Value);
    }
}

void AGPPlayerController::MoveRight(float Value)
{
    if ((GetCharacter() != NULL) && (Value != 0.0f))
    {
        // find out which way is right
        const FRotator Rotation = GetControlRotation();
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
        // add movement in that direction
        GetCharacter()->AddMovementInput(Direction, Value);
    }
}

void AGPPlayerController::AddControllerYawInput(float Value)
{
	if ((GetCharacter() != NULL) && (Value != 0.0f))
	{
		GetCharacter()->AddControllerYawInput(Value);
	}
}

void AGPPlayerController::AddControllerPitchInput(float Value)
{
	if ((GetCharacter() != NULL) && (Value != 0.0f))
	{
		GetCharacter()->AddControllerPitchInput(Value);
	}
}

void AGPPlayerController::OnStartJump()
{
	if (GetCharacter() != NULL)
	{
		Cast<AGPCharacter>(GetCharacter())->OnStartJump();
	}
}

void AGPPlayerController::OnStopJump()
{
	if (GetCharacter() != NULL)
	{
		Cast<AGPCharacter>(GetCharacter())->OnStopJump();
	}
}

void AGPPlayerController::OnFire()
{
	if (GetCharacter() != NULL)
	{
		Cast<AGPCharacter>(GetCharacter())->OnFire();
	}
}

void AGPPlayerController::OnBombLaunch()
{
	if (!StartTCPReceiver("RamaSocketListener", "127.0.0.1", 25599))
	{
		//UE_LOG  "TCP Socket Listener Created!"
		return;
	}
		
	//UE_LOG  "TCP Socket Listener Created! Yay!"

	// Instantiate the Kinect Interface
	//KinectInterface::JoyInit(this);
	if (GetCharacter() != NULL)
	{
		Cast<AGPCharacter>(GetCharacter())->OnBombLaunch();
	}
}

void AGPPlayerController::OnBombDetonate()
{
	if (GetCharacter() != NULL)
	{
		Cast<AGPCharacter>(GetCharacter())->OnBombDetonate();
	}
}

//bool AGPPlayerController::ServerOnFire_Validate()
//{
//	// We should check game state down the road to make sure this is valid.
//	return true;
//}
//
//void AGPPlayerController::ServerOnFire_Implementation()
//{
//	if (GetCharacter() != NULL)
//	{
//		Cast<AGPCharacter>(GetCharacter())->OnFire();
//	}
//}

//void AGPPlayerController::SetSomeBool(bool bNewSomeBool)
//{
//	// Change the value of the bSomeBool property
//	bSomeBool = bNewSomeBool;
//
//	// If this next check succeeds, we are *not* the authority, meaning we are a network client.
//	// In this case we also want to call the server function to tell it to change the bSomeBool property as well.
//	if (Role < ROLE_Authority)
//	{
//		ServerSetSomeBool(bNewSomeBool);
//	}
//}
//
//bool AGPPlayerController::ServerSetSomeBool_Validate(bool bNewSomeBool)
//{
//	return true;
//}
//
//void AGPPlayerController::ServerSetSomeBool_Implementation(bool bNewSomeBool)
//{
//	// This function is only called on the server (where Role == ROLE_Authority), called over the network by clients.
//	// We need to call SetSomeBool() to actually change the value of the bool now!
//	// Inside that function, Role == ROLE_Authority, so it won't try to call ServerSetSomeBool() again.
//	SetSomeBool(bNewSomeBool);
//}



/////////////////////////////////////////////////////
//////////////////// HERE BE DRAGONS ////////////////
/////////////////////////////////////////////////////

void AGPPlayerController::EndPlay(EEndPlayReason::Type reason)
{
	/*GetWorldTimerManager().ClearAllTimersForObject(this);
	ConnectionSocket->Close();
	ListenerSocket->Close();
	delete ListenerSocket;
	delete ConnectionSocket;*/
}

//void AGPPlayerController::Laaaaaauuuunch()
//{
//	//IP = 127.0.0.1, Port = 8890 for my Python test case
//	if (!StartTCPReceiver("RamaSocketListener", "127.0.0.1", 8890))
//	{
//		//UE_LOG  "TCP Socket Listener Created!"
//		return;
//	}
//
//	//UE_LOG  "TCP Socket Listener Created! Yay!"
//}

//Rama's Start TCP Receiver
bool AGPPlayerController::StartTCPReceiver(
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
	GetWorldTimerManager().SetTimer(this,
		&AGPPlayerController::TCPConnectionListener, 0.01, true);

	return true;
}
//Format IP String as Number Parts
bool AGPPlayerController::FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4])
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
FSocket* AGPPlayerController::CreateTCPConnectionListener(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort, const int32 ReceiveBufferSize)
{
	uint8 IP4Nums[4];
	if (!FormatIP4ToNumber(TheIP, IP4Nums))
	{
		VShow("Invalid IP! Expecting 4 parts separated by .");
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
void AGPPlayerController::TCPConnectionListener()
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

			//can thread this too
			GetWorldTimerManager().SetTimer(this,
				&AGPPlayerController::TCPSocketListener, 0.01, true);
		}
	}
}

//Rama's String From Binary Array
//This function requires 
//		#include <string>
FString AGPPlayerController::StringFromBinaryArray(const TArray<uint8>& BinaryArray)
{
	//Create a string from a byte array!
	std::string cstr(reinterpret_cast<const char*>(BinaryArray.GetData()), BinaryArray.Num());
	return FString(cstr.c_str());
}

void AGPPlayerController::VectorFromTArray(TArray<uint8> &arr, std::vector<char> &vec)
{
	vec.clear();
	for (auto Iter(arr.CreateConstIterator()); Iter; Iter++)
	{
		// *Iter to access what this iterator is pointing to.
		uint8 the_thing = *Iter;
		vec.push_back((char)the_thing);
	}
}

//Rama's TCP Socket Listener
void AGPPlayerController::TCPSocketListener()
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

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Data Read! %d"), ReceivedData.Num()));
	}
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	if (ReceivedData.Num() <= 0)
	{
		//No Data Received
		return;
	}

	VShow("Total Data read!", ReceivedData.Num());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Data Bytes Read ~> %d"), ReceivedData.Num()));


	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	//						Rama's String From Binary Array
	const FString ReceivedUE4String = StringFromBinaryArray(ReceivedData);
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


	VShow("As String!!!!! ~>", ReceivedUE4String);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("As String Data ~> %s"), *ReceivedUE4String));

	// TODO: This needs to be tidied considerably!
	// TODO: Need to anticipate incoming packet size and buffer accordingly.
	switch (commstate)
	{
	case 0:
		{
			// Need to send challenge response
			OCVSPacketChallenge pktChallenge;
			std::vector<char> somestuff;
			VectorFromTArray(ReceivedData, somestuff);

			if (pktChallenge.VerifyReceived(somestuff)) {
				int32 sent = 0;
				ConnectionSocket->Send(ReceivedData.GetData(), ReceivedData.Num(), sent);

				VShow("Sent bytes ~>", sent);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Sent bytes ~> %d"), sent));

				commstate = 1;
			}
		}
		break;
	case 1:
		{
			// Should have received an ACK, then scan head, then the lone scan chunk.
			// TODO: This should be functionality of PacketAck?
			//Eat the ACK byte
			ReceivedData.RemoveAt(0);

			std::vector<char> somestuff;
			VectorFromTArray(ReceivedData, somestuff);
			
			// Read the scan head.
			OCVSPacketScanHeader scanHd(somestuff);

			VShow("Got Scan with chunks = ~>", scanHd.GetChunkCount());
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Got Scan with chunks ~> %d"), scanHd.GetChunkCount()));

			// Read the chunk(s)
			for (int i = 0; i < (int)scanHd.GetChunkCount(); i++) {
				OCVSPacketScanChunk scanChnk(somestuff, scanHd.GetPackedSize());
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Got Scan with rect at ~> %f,%f rot: %f scale: %f,%f"), scanChnk.centre_x, scanChnk.centre_y, scanChnk.rotation, scanChnk.scale_x, scanChnk.scale_y));
				
			}

			commstate = 0;
		}
		break;
	default:
		break;
	}
}