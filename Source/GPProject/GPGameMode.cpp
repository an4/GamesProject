// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPGameMode.h"
#include "GPHUD.h"
#include "GPPlayerController.h"


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
}

void AGPGameMode::StartPlay()
{
	Super::StartPlay();

	if (Role == ROLE_Authority)
	{
		//Should spawn a building at the NW and SE corners of the map...
		SpawnBuilding(FVector2D(0.0f, 0.0f), FVector2D(64.0f, 64.0f));
		SpawnBuilding(FVector2D(576.0f, 576.0f), FVector2D(640.0f, 640.0f));
		// Spawn some extra obstacles in game coordinates to test pathfinding
		SpawnBuilding(FVector(0.0f, 500.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(1.0f, 25.0f, 2.0f));
		SpawnBuilding(FVector(-500.0f, -500.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(1.0f, 25.0f, 2.0f));
	}

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("HELLO WORLD"));
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
	const float worldx_px = 640.0f;
	const float worldy_px = 640.0f; // TODO: Switch this for 480 and resize the world so it matches the aspect ratio.

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

	centre.Z += scalez * meshy * 0.5;

	// Offset the centre for differing input vs game origins
	centre += worldOffset;

	return SpawnBuilding(centre, rot, FVector(scalex, scaley, scalez));
}

void AGPGameMode::SpawnBuilding(FVector centre, FRotator rotation, FVector scale)
{
	//FRotator const buildingRotation(0.0f, 0.0f, 0.0f);

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