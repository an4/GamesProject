// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPPlayerState.h"

void AGPPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AGPPlayerState, hasFlag);
	DOREPLIFETIME(AGPPlayerState, numFlags);
	DOREPLIFETIME(AGPPlayerState, Team);
	DOREPLIFETIME(AGPPlayerState, canPickupFlag);
	DOREPLIFETIME(AGPPlayerState, hadFlag);
}

void AGPPlayerState::SetHasFlag(bool newState)
{
	hasFlag = newState;
}

bool AGPPlayerState::GetHasFlag()
{
	return hasFlag;
}

void AGPPlayerState::SetCanPickupFlag(bool newState)
{
	canPickupFlag = newState;
}

bool AGPPlayerState::GetCanPickupFlag()
{
	return canPickupFlag;
}

// TODO: Add winstate for flag number (add to game mode / game state?)
void AGPPlayerState::IncrementFlags()
{
	numFlags += 1;
}

void AGPPlayerState::ResetFlags()
{
	numFlags = 0;
}

int32 AGPPlayerState::GetNumFlags()
{
	return numFlags;
}

void AGPPlayerState::SetHadFlag(bool newState)
{
	hadFlag = newState;
}

bool AGPPlayerState::GetHadFlag()
{
	return hadFlag;
}