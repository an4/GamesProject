// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPGameState.h"
#include "UnrealNetwork.h"

AGPGameState::AGPGameState(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	TeamPlayerCount[0] = 0;
	TeamPlayerCount[1] = 0;
	gameState = 1;
}

int8 AGPGameState::GetSetTeam()
{
	if (TeamPlayerCount[0] == TeamPlayerCount[1])
	{
		UE_LOG(LogTemp, Warning, TEXT("Added player to team 1!"));
		TeamPlayerCount[0]++;
		return 0;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Added player to team 2!"));
		TeamPlayerCount[1]++;
		return 1;
	}
}

void AGPGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone so that we don't have to broadcast server functions
	DOREPLIFETIME(AGPGameState, gameState);
}

void AGPGameState::SetState(int32 newState)
{
	gameState = newState;
}

int32 AGPGameState::GetState()
{
	return gameState;
}