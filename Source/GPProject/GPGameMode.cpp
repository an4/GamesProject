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
	SpawnBuilding();
    StartMatch();

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("HELLO WORLD"));
    }
}

void AGPGameMode::SpawnBuilding()
{
	FVector const buildingLocation(-230.0f, 100.0f, 300.0f);
	FRotator const buildingRotation(100.0f, 100.0f, 100.0f);

	UWorld* const World = GetWorld();

	if (World)
	{
		FActorSpawnParameters SpawnParams = FActorSpawnParameters();

		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;

		AGPBuilding* building = World->SpawnActor<AGPBuilding>(AGPBuilding::StaticClass(), buildingLocation, buildingRotation, SpawnParams);
		if (building != NULL)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("HELLO Building"));
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Not so HELLO Building"));
		}
		//building = (AGPBuilding*)GetWorld()->SpawnActor(AGPBuilding::StaticClass(), buildingLocation);
		//building = (AGPBuilding*)GetWorld()->SpawnActor(AGPBuilding::StaticClass(), &buildingLocation, &buildingRotation, SpawnParams);
	}
}