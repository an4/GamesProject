// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "GPGameState.h"
#include "UnrealNetwork.h"
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
		bool hasFlag = false;

	UPROPERTY(Replicated)
		int32 numFlags = 0;

	UFUNCTION()
		void SetHasFlag(bool newState);

	UFUNCTION()
		bool GetHasFlag();

	UFUNCTION()
		void IncrementFlags();
	UFUNCTION()
		void ResetFlags();
	UFUNCTION()
		int32 GetNumFlags();
};