// Fill out your copyright notice in the Description page of Project Settings.

#include "GPProject.h"
#include "GPPlayerState.h"


void AGPPlayerState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(AGPPlayerState, Team);
}