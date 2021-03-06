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

		UPROPERTY(Replicated)
		int32 team1Flags = 0;

		UPROPERTY(Replicated)
		int32 team0Flags = 0;

        UPROPERTY(REPLICATED)
        int32 flagsCaptured = 0;

		UPROPERTY(Replicated)
		bool waitingForRescan = false;

		UFUNCTION()
		void SetState(int32 newState);

		UFUNCTION(BlueprintCallable, Category = "State")
		int32 GetState();

		UFUNCTION()
		void SetWaitingForRescan(bool val);

		UFUNCTION(BlueprintCallable, Category = "Rescan")
		bool GetWaitingForRescan();

		UFUNCTION()
		void UpdateFlagLeader();

        UFUNCTION(BlueprintCallable, Category = "Flag")
		int32 GetFlagLeader();

		UFUNCTION(BlueprintCallable, Category = "Flag")
		int32 GetTeamFlags(int32 team);

		UFUNCTION(BlueprintImplementableEvent, Category = "Game End")
		virtual void GameEndCondition(int32 team);

        UFUNCTION(BlueprintCallable, Category = "Flag")
        int32 GetFlagsCaptured();
};
