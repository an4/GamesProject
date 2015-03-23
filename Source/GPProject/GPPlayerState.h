// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GPGameState.h"
#include "UnrealNetwork.h"
#include "GameFramework/PlayerState.h"
#include "GPPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPPlayerState : public APlayerState
{
	GENERATED_BODY()
	public:
	UPROPERTY(Replicated)
	int8 Team;
	
	void SetupTeam();
};
