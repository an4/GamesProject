// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPGameMode.h"
#include "GPHUD.h"
#include "GPPlayerController.h"
#include "EngineUtils.h"
#include "KinectInterface.h"

AGPGameMode::AGPGameMode(const class FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
	// set the player controller class to our own subclass
	// the controller class handles a player for the entirety of the game, whereas pawns can be replaced (e.g. death and respawn)
	// Controller should hold things like score, team that need to be kept across lives! Should handle input and replication.
	PlayerControllerClass = AGPPlayerController::StaticClass();

    // set default pawn class to our Blueprinted character
    static ConstructorHelpers::FObjectFinder<UBlueprint> PlayerPawnObject(TEXT("Blueprint'/Game/Blueprints/BP_GPCharacter.BP_GPCharacter'"));
    if (PlayerPawnObject.Object != NULL)
    {
        DefaultPawnClass = (UClass*)PlayerPawnObject.Object->GeneratedClass;
    }
	
    HUDClass = AGPHUD::StaticClass();
	tickCount = 0.0;

	// Instantiate the Kinect Interface
	kinect = new KinectInterface();
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

		// Run the Kinect code for debugging purposes. TODO: Stop using pointer? Delete? Singleton?
		kinect->Run();
	}

}

void AGPGameMode::SpawnBuilding(FVector2D const a, FVector2D const b)
{
	// World size TODO: Calculate this!
	const float worldx = 5000.0f;
	const float worldy = 5000.0f;

	// World NW corner offset (The origin is currently at the centre of the floor object) TODO: Move to 0,0?
	const FVector worldOffset = FVector(-1.0f * (worldx / 2.0f), -1.0f * (worldy / 2.0f), 0.0f);

	// Building mesh size TODO: Calculate this!
	const float meshx = 200.0f;
	const float meshy = 200.0f;

	// Dimensions of the building rectangle (in pixels!)
	const float sqx_px = FMath::Max(a.X, b.X) - FMath::Min(a.X, b.X);
	const float sqy_px = FMath::Max(a.Y, b.Y) - FMath::Min(a.Y, b.Y);

	// Dimensions of the entire input space (in pixels!)
	const float worldx_px = kinect->GetWidth();
	const float worldy_px = kinect->GetHeight(); // TODO: Resize the world so it matches the aspect ratio.

	// Get the unscaled centre point.
	FVector2D centre2D = (a + b) / 2.0f;
	FVector centre = FVector(centre2D, 0.0f);

	FRotator rot = centre.Rotation();
	// Offset the rotation by 45 as we got this from the diagonal.
	rot.Yaw -= 45.0f;

	// Scale factors for mesh scaling.
	const float scalex = (sqx_px * worldx) / (worldx_px * meshx);
	const float scaley = (sqy_px * worldy) / (worldy_px * meshy);
	const float scalez = 5.0f;

	// Scale factors for points in the world.
	const float cscalex = worldx / worldx_px;
	const float cscaley = worldy / worldy_px;

	// Scale the centre points up into game coordinates.
	centre.X *= cscalex;
	centre.Y *= cscaley;

	// Offset the centre for differing input vs game origins
	centre += worldOffset;

	return SpawnBuilding(centre, rot, FVector(scalex, scaley, scalez));
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