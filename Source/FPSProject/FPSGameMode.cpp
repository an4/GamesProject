// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSProject.h"
#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "GPPlayerController.h"


AFPSGameMode::AFPSGameMode(const class FObjectInitializer& ObjectInitializer)
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
    HUDClass = AFPSHUD::StaticClass();
}

void AFPSGameMode::StartPlay()
{
    Super::StartPlay();

    StartMatch();

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("HELLO WORLD"));
    }
}