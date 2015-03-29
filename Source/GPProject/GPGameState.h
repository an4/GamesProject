// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "GPGameState.generated.h"

/**
 * 
 */
UCLASS()
class GPPROJECT_API AGPGameState : public AGameState
{
	GENERATED_BODY()

	UPROPERTY(Replicated)
	int32 flagLeader;
	
	AGPGameState(const class FObjectInitializer& ObjectInitializer);

	int8 TeamPlayerCount[2];
	
	public:
		int8 GetSetTeam();

		UPROPERTY(Replicated)
		int32 gameState;

		UFUNCTION()
		void SetState(int32 newState);

		UFUNCTION()
		int32 GetState();

		UFUNCTION()
		void UpdateFlagLeader();

        UFUNCTION(BlueprintCallable, Category = "Flag")
		int32 GetFlagLeader();

	
};
