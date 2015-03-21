// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPGameMode.h"
#include "GPHUD.h"
#include "GPPlayerController.h"
#include "GPGameState.h"
#include "EngineUtils.h"

#include "GPKinectAPI/OCVSPacketAck.h"
#include "GPKinectAPI/OCVSPacketChallenge.h"
#include "GPKinectAPI/OCVSPacketScanReq.h"
#include "GPKinectAPI/OCVSPacketScanHeader.h"
#include "GPKinectAPI/OCVSPacketScanChunk.h"

AGPGameMode::AGPGameMode(const class FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
	// set the player controller class to our own subclass
	// the controller class handles a player for the entirety of the game, whereas pawns can be replaced (e.g. death and respawn)
	// Controller should hold things like score, team that need to be kept across lives! Should handle input and replication.
	PlayerControllerClass = AGPPlayerController::StaticClass();

	GameStateClass = AGPGameState::StaticClass();

    // set default pawn class to our Blueprinted character
    static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("Blueprint'/Game/Blueprints/BP_GPCharacter.BP_GPCharacter'"));
    if (PlayerPawnObject.Object != NULL)
    {
        DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
    }
	
    HUDClass = AGPHUD::StaticClass();
	tickCount = 0.0;
}

void AGPGameMode::StartPlay()
{
	Super::StartPlay();

	if (Role == ROLE_Authority)
	{
		// Surround the play area with a border of buildings (need to use Unreal coords as we are out of bounds)
		SpawnBuilding(FVector(0.0, -2600.0, 0.0), FRotator::ZeroRotator, FVector(5400. / 200., 1., 7.)); // Use 5400 so we fill in corners
		SpawnBuilding(FVector(0.0, 2600.0, 0.0), FRotator::ZeroRotator, FVector(5400. / 200., 1., 7.));
		SpawnBuilding(FVector(2600., 0., 0.), FRotator::ZeroRotator, FVector(1., 5000. / 200., 7.));
		SpawnBuilding(FVector(-2600., 0., 0.), FRotator::ZeroRotator, FVector(1., 5000. / 200., 7.));

        // Spawn flag
		SpawnFlag();

		// Start listener for Kinect input
		if (!StartTCPReceiver("KinectSocketListener", "127.0.0.1", 25599))
		{
			return;
		}
	}

}


void AGPGameMode::SpawnBuilding(FVector2D ctr, float rot, FVector2D scl)
{
	// World size TODO: Calculate this!
	const float worldx = 5000.0f;
	const float worldy = 5000.0f;

	// World NW corner offset (The origin is currently at the centre of the floor object) TODO: Move to 0,0?
	const FVector worldOffset = FVector(-1.0f * (worldx / 2.0f), -1.0f * (worldy / 2.0f), 0.0f);

	// Building mesh size TODO: Calculate this!
	const float meshx = 200.0f;
	const float meshy = 200.0f;

	// Dimensions of the entire input space (in pixels!)
	const float worldx_px = 640.0f; // TODO: Bring back Kinect Interface with resolution constants
	const float worldy_px = 480.0f; // TODO: Resize the world so it matches the aspect ratio.

	// Scale factors for points in the world.
	const float cscalex = worldx / worldx_px;
	const float cscaley = worldy / worldy_px;

	// Scale factors for mesh scaling.
	const float scalex = (scl.X * worldx) / (worldx_px * meshx);
	const float scaley = (scl.Y * worldy) / (worldy_px * meshy);
	const float scalez = 5.0f;

	// Wrap the rotation into a rotator
	FRotator rotation(0.0f, rot, 0.0f);

	// Scale the centre into the world space.
	FVector centre(ctr.X * cscalex, ctr.Y * cscaley, 0.0f);
	// Offset the centre into the world space.
	centre += worldOffset;

	// Convert the scale to world space.
	FVector scale(scalex, scaley, scalez);

	return SpawnBuilding(centre, rotation, scale);
}

void AGPGameMode::SpawnBuilding(FVector centre, FRotator rotation, FVector scale)
{
	//FRotator const buildingRotation(0.0f, 0.0f, 0.0f);

	const float meshz = 200.0; // TODO: Get rid of me
	centre.Z = 20.0 + scale.Z * meshz * 0.5;

	UWorld* const World = GetWorld();

	if (World)
	{
		FActorSpawnParameters SpawnParams = FActorSpawnParameters();

		SpawnParams.Owner = this;
		SpawnParams.Instigator = NULL;

		AGPBuilding* building = World->SpawnActor<AGPBuilding>(AGPBuilding::StaticClass(), centre, rotation, SpawnParams);

		if (building != NULL)
		{
			building->SetScale(scale);
		}
	}
}

bool AGPGameMode::IsClear(FVector2D centre, FRotator rotation, FVector scale)
{
	FConstPawnIterator pawns = this->GetWorld()->GetPawnIterator();

	const float scaledX = 200 * scale.X / 2.0;
	const float scaledY = 200 * scale.Y / 2.0;

	// Ensure no buildings are in the way.
	for (TActorIterator<AGPBuilding> bIt(GetWorld()); bIt; ++bIt)
	{
		FVector2D loc = FVector2D(bIt->GetActorLocation());
		float dist = FVector2D::DistSquared(loc, centre);

		const float minDist = FMath::Square(scaledX + (200. * bIt->GetActorScale().X)) + FMath::Square(scaledY + (200. * bIt->GetActorScale().Y));
		const float extraGap = 100.0; // Collision capsule r=34... this should give a decent gap to walk through
		const float minDistX = scaledX + (100. * bIt->GetActorScale().X) + extraGap;
		const float minDistY = scaledY + (100. * bIt->GetActorScale().Y) + extraGap;

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Bldg %d dist squared %f"), bIt->GetUniqueID(), dist));
		if (FMath::Abs<float>(loc.X - centre.X) <= minDistX && FMath::Abs<float>(loc.Y - centre.Y) <= minDistY)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Bldg %d at %f %f, and %f %f with min %f %f in the way..."), bIt->GetUniqueID(), loc.X, loc.Y, centre.X, centre.Y, minDistX, minDistY).Append(bIt->GetName()));
			return false;
		}
	}

	const float minPawnDist = FMath::Square(scaledX * 2.0) + FMath::Square(scaledY * 2.0);

	// Ensure no player pawns are in the way.
	for (FConstPawnIterator pIt = GetWorld()->GetPawnIterator(); pIt; ++pIt)
	{
		FVector2D loc = FVector2D(pIt->Get()->GetActorLocation()); //TODO: Check bounds
		const float minPawnDistX = scaledX + 100.;
		const float minPawnDistY = scaledY + 100.;

		// For now ignore rotation and scale and just make sure the bounding circle of the mesh around centre is clear.
		if (FMath::Abs<float>(loc.X - centre.X) <= minPawnDistX && FMath::Abs<float>(loc.Y - centre.Y) <= minPawnDistY)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Pawn %d in the way..."), pIt->Get()->GetUniqueID()));
			return false;
		}
	}

	return true;
}

void AGPGameMode::Tick(float DeltaSeconds)
{
	tickCount += DeltaSeconds;
	if (false && tickCount >= FMath::FRandRange(5.0f, 45.0f)) {
		tickCount = 0.0;

		FVector centre = FMath::RandPointInBox(FBox(FVector(-2500., -2500., 0.), FVector(2500., 2500., 0.)));
		FVector scale = FMath::RandPointInBox(FBox(FVector(0.75, 0.75, 2.0), FVector(4.0, 4.0, 12.0)));

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Spawning at %f %f %f"), centre.X, centre.Y, centre.Z));
		if (IsClear(FVector2D(centre), FRotator::ZeroRotator, scale)) {
			SpawnBuilding(centre, FRotator::ZeroRotator, scale);
		}
	}
}

void AGPGameMode::SpawnFlag()
{
    UWorld* const World = GetWorld();

    if (World)
    {
        FActorSpawnParameters SpawnParams = FActorSpawnParameters();

        SpawnParams.Owner = this;
        SpawnParams.Instigator = NULL;

        FRotator rotation = FRotator(0.f, 0.f, 0.f);
        FVector location = FMath::RandPointInBox(FBox(FVector(-2500., -2500., 21.), FVector(2500., 2500., 21.)));

        AGPFlagPickup* flag = World->SpawnActor<AGPFlagPickup>(AGPFlagPickup::StaticClass(), location, rotation, SpawnParams);

        if (flag == NULL)
        {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Flag is null"));
            }
        }
        else {
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Flag spawned"));
            }
        }
    }
}

void AGPGameMode::ResetBuildings()
{
	bool doOnce = false;
	for (TActorIterator<AGPBuilding> bIt(GetWorld()); bIt; ++bIt)
	{
		// Skip the 4 walls
		if (!doOnce)
		{
			++bIt;
			++bIt;
			++bIt;
			++bIt;
			doOnce = true;
		}
		if (bIt != NULL && bIt)
		{
			bIt->Destroy();
		}
	}
}

/////////////////////////////////////////////////////
//////////////////// HERE BE DRAGONS ////////////////
/////////////////////////////////////////////////////

void AGPGameMode::EndPlay(EEndPlayReason::Type reason)
{
	GetWorldTimerManager().ClearAllTimersForObject(this);

	if (ConnectionSocket != NULL) {
		ConnectionSocket->Close();
		delete ConnectionSocket;
	}

	if (ListenerSocket != NULL) {
		ListenerSocket->Close();
		delete ListenerSocket;
	}
}

bool AGPGameMode::StartTCPReceiver(
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

	//Start the Listener! // TODO: Threading?
	GetWorldTimerManager().SetTimer(this, &AGPGameMode::TCPConnectionListener, 0.01, true);

	return true;
}

//Format IP String as Number Parts
bool AGPGameMode::FormatIP4ToNumber(const FString& TheIP, uint8(&Out)[4])
{
	//IP Formatting
	TheIP.Replace(TEXT(" "), TEXT(""));

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
FSocket* AGPGameMode::CreateTCPConnectionListener(const FString& YourChosenSocketName, const FString& TheIP, const int32 ThePort, const int32 ReceiveBufferSize)
{
	uint8 IP4Nums[4];
	if (!FormatIP4ToNumber(TheIP, IP4Nums))
	{
		return false;
	}

	//Create Socket
	FIPv4Endpoint Endpoint(FIPv4Address(IP4Nums[0], IP4Nums[1], IP4Nums[2], IP4Nums[3]), ThePort);
	FSocket* ListenSocket = FTcpSocketBuilder(*YourChosenSocketName)
		.AsReusable()
		.BoundToEndpoint(Endpoint)
		.Listening(8);

	//Set Buffer Size
	int32 NewSize = 0;
	ListenSocket->SetReceiveBufferSize(ReceiveBufferSize, NewSize);

	return ListenSocket;
}

//Rama's TCP Connection Listener
void AGPGameMode::TCPConnectionListener()
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
			// Need to reset the comms state on disconnect.
			commstate = OCVSProtocolState::INIT;

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

			// Ensure commstate is reset.
			commstate = OCVSProtocolState::INIT;

			//can thread this too
			GetWorldTimerManager().SetTimer(this, &AGPGameMode::TCPSocketListener, 0.01, true);
		}
	}
}

void AGPGameMode::VectorFromTArray(TArray<uint8> &arr, std::vector<char> &vec)
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
void AGPGameMode::TCPSocketListener()
{
	if (!ConnectionSocket || (commstate == OCVSProtocolState::REQUEST && !wantScan)) return; // TODO: We may want to do some keepalive comms whilst in request state

	TArray<uint8> ReceivedData;
	uint32 Size;

	while (ConnectionSocket->HasPendingData(Size))
	{
		ReceivedData.Init(FMath::Min(Size, 65507u));

		int32 Read = 0;
		ConnectionSocket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);
	}

	if (ReceivedData.Num() <= 0 && commstate != OCVSProtocolState::REQUEST)
	{
		//No Data Received
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Data Bytes Read ~> %d"), ReceivedData.Num()));

	// TODO: This needs to be tidied considerably!
	// TODO: Need to anticipate incoming packet size and buffer accordingly.
	switch (commstate)
	{
	case OCVSProtocolState::INIT:
	{
		// Need to send challenge response
		OCVSPacketChallenge pktChallenge;
		std::vector<char> somestuff;
		VectorFromTArray(ReceivedData, somestuff);

		if (pktChallenge.VerifyReceived(somestuff)) {
			int32 sent = 0;
			ConnectionSocket->Send(ReceivedData.GetData(), ReceivedData.Num(), sent);

			check(sent == pktChallenge.GetPackedSize());

			commstate = OCVSProtocolState::REQUEST;
		}
	}
	break;
	case OCVSProtocolState::REQUEST:
	// Need to send request, if we want a scan.
	if (wantScan) {
		OCVSPacketScanReq pktReq;
		std::vector<char> somestuff;
		VectorFromTArray(ReceivedData, somestuff);
		pktReq.Pack(somestuff);

		int32 sent = 0;
		// TODO: This reinterpret cast is nice but smelly...
		ConnectionSocket->Send(reinterpret_cast<uint8 *>(somestuff.data()), somestuff.size(), sent);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Sent bytes ~> %d"), sent));

		commstate = OCVSProtocolState::RECEIVE;
		wantScan = false;
	}
	break;
	case OCVSProtocolState::RECEIVE:
	{
		// Should have received an ACK, then scan head, then the lone scan chunk.
		// TODO: This should be functionality of PacketAck?
		//Eat the ACK byte
		ReceivedData.RemoveAt(0);

		std::vector<char> somestuff;
		VectorFromTArray(ReceivedData, somestuff);

		// Read the scan head.
		OCVSPacketScanHeader scanHd(somestuff);

		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Got Scan with chunks ~> %d"), scanHd.GetChunkCount()));

		int offset = scanHd.GetPackedSize();

		// TODO: Ensure we actually have all of the promised data... or block between chunks

		// Read the chunk(s) TODO: Don't block on it here!!!
		for (int i = 0; i < (int)scanHd.GetChunkCount(); i++) {
			OCVSPacketScanChunk scanChnk(somestuff, offset);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Got Scan with rect at ~> %f,%f rot: %f scale: %f,%f"), scanChnk.centre_x, scanChnk.centre_y, scanChnk.rotation, scanChnk.scale_x, scanChnk.scale_y));

			// Spawn the received building.
			SpawnBuilding(FVector2D(scanChnk.centre_x, scanChnk.centre_y), scanChnk.rotation, FVector2D(scanChnk.scale_x, scanChnk.scale_y));

			// Move the offset by the last chunk.
			offset += scanChnk.GetPackedSize();
		}

		OCVSPacketAck::getInstance()->Pack(somestuff);
		int32 sent = 0;
		// TODO: This reinterpret cast is nice but smelly...
		ConnectionSocket->Send(reinterpret_cast<uint8 *>(somestuff.data()), somestuff.size(), sent);

		commstate = OCVSProtocolState::INIT;
	}
	break;
	default:
	break;
	}
}
