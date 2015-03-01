// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPGameState.h"

AGPGameState::AGPGameState(const class FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	gameState = 1;
}

void AGPGameState::SetState(int32 newState)
{
	gameState = newState;
}

int32 AGPGameState::GetState()
{
	return gameState;
}

