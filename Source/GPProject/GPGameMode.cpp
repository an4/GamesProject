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
	//SpawnBuilding(FVector(-230.0f, 100.0f, 300.0f), FVector(2.0f,2.0f,10.0f));
	SpawnBuilding(FVector2D(0.0f, 0.0f), FVector2D(64.0f, 64.0f));
    StartMatch();

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("HELLO WORLD"));
    }
}

void AGPGameMode::SpawnBuilding(FVector2D const a, FVector2D const b)
{
	FVector2D centre2D = (a + b) / 2.0f;
	FVector centre = FVector(centre2D, 0.0f);
	FRotator rot = centre.Rotation();

	// Offset the rotation by 45 as we got this from the diagonal.
	rot.Yaw -= 45.0f;

	// World size TODO: Calculate this!
	float worldx = 1000.0f;
	float worldy = 1000.0f;
	float meshx = 105.0f;
	float meshy = 105.0f;

	float sqx_px = FMath::Max(a.X, b.X) - FMath::Min(a.X, b.X);
	float sqy_px = FMath::Max(a.Y, b.Y) - FMath::Min(a.Y, b.Y);
	float worldx_px = 640.0f;
	float worldy_px = 640.0f; // TODO: Switch this for 480 and resize the world so it matches the aspect ratio.

	float scalex = (sqx_px * worldx) / (worldx_px * meshx);
	float scaley = (sqy_px * worldy) / (worldy_px * meshy);

	return SpawnBuilding(centre, rot, FVector(scalex, scaley, 5.0f));
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
			building->SetActorScale3D(scale);
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("HELLO Building"));
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Not so HELLO Building"));
		}
		//building = (AGPBuilding*)GetWorld()->SpawnActor(AGPBuilding::StaticClass(), buildingLocation);
		//building = (AGPBuilding*)GetWorld()->SpawnActor(AGPBuilding::StaticClass(), &buildingLocation, &buildingRotation, SpawnParams);
	}
}