// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPGameMode.h"
#include "GPHUD.h"
#include "GPPlayerController.h"


AGPGameMode::AGPGameMode(const class FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
	UWorld* const World = GetWorld();
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
	FVector const buildingLocation(100.0f, 100.0f, 100.0f);
	FVector const buildingRotation(100.0f, 100.0f, 100.0f);

	if (World)
	{
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;
		//building = (AGPBuilding*)GetWorld()->SpawnActor(AGPBuilding::StaticClass(), NAME_None, buildingLocation);
		//building = (AGPBuilding*)GetWorld()->SpawnActor(AGPBuilding::StaticClass(), &buildingLocation, &buildingRotation, SpawnParams);
		//building = World->SpawnActor<AGPBuilding>(BuildingClass, buildingLocation, buildingRotation, SpawnParams);

	}
    HUDClass = AGPHUD::StaticClass();
}

void AGPGameMode::StartPlay()
{
    Super::StartPlay();

    StartMatch();

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("HELLO WORLD"));
    }
}