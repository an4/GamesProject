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
		bool canPickupFlag = true;

	UPROPERTY(Replicated)
		int32 numFlags = 0;

	UPROPERTY(Replicated)
		int8 Team;

	UPROPERTY(Replicated)
		bool hadFlag = false;

	void SetupTeam();

	UFUNCTION()
		void SetHasFlag(bool newState);

	UFUNCTION()
		bool GetHasFlag();

	UFUNCTION()
		void SetHadFlag(bool newState);

	UFUNCTION()
		bool GetHadFlag();

	UFUNCTION()
		void SetCanPickupFlag(bool newState);

	UFUNCTION()
		bool GetCanPickupFlag();

	UFUNCTION()
		void IncrementFlags();
	UFUNCTION()
		void ResetFlags();
    UFUNCTION(BlueprintCallable, Category = "Flag")
		int32 GetNumFlags();
};
