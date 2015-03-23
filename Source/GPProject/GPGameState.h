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
	int32 gameState;

	UPROPERTY(Replicated)
	int32 flagLeader;
	
	AGPGameState(const class FObjectInitializer& ObjectInitializer);

	public:
		UFUNCTION()
		void SetState(int32 newState);

		UFUNCTION()
		int32 GetState();

		UFUNCTION()
		void UpdateFlagLeader();

		UFUNCTION()
		int32 GetFlagLeader();

	
	
};
