// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPGameState.h"
#include "UnrealNetwork.h"

AGPGameState::AGPGameState(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	gameState = 1;
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

